/*
Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "websockets.h"
#include <json/json.h>
#include <json/json_object.h>
#include <json/json_tokener.h>
#include <vehicleproperty.h>
#include <listplusplus.h>

#define __SMALLFILE__ std::string(__FILE__).substr(std::string(__FILE__).rfind("/")+1)

static int websocket_callback(
    libwebsocket_context *context, libwebsocket *wsi, libwebsocket_callback_reasons reason, void *user,void *in, size_t len)
{
    WebSockets *ws = static_cast<WebSockets*>(libwebsocket_context_user (context));
    WebSocketsObserver* observer = &ws->getObserver();
    DebugOut(5) << __SMALLFILE__ << ":" << __LINE__ <<
            "websocket_callback:" << reason << " ,user:" << reinterpret_cast<uint64_t>(user) << endl;
    if(!observer)
        return 0;

    switch (reason)
    {
        case LWS_CALLBACK_CLIENT_WRITEABLE:
        {
            //Connection has been established.
            //printf("Connection established\n");
            break;
        }
        case LWS_CALLBACK_CLOSED:
        {
            //Connection is closed, we need to remove all related sinks
            //sinkManager->disconnectAll(wsi);
            break;
        }
        case LWS_CALLBACK_CLIENT_RECEIVE:
        {
            //printf("Client writable\n");
            break;
        }
        case LWS_CALLBACK_SERVER_WRITEABLE:
        {
            //printf("Server writable\n");
            break;
        }

        case LWS_CALLBACK_RECEIVE:
        {
            //printf("Data Received: %s\n",(char*)in);
            //The lack of a break; here is intentional.
        }
        case LWS_CALLBACK_HTTP:
        {
            //TODO: Verify that ALL requests get sent via LWS_CALLBACK_HTTP, so we can use that instead of LWS_CALLBACK_RECIEVE
            const char *data = static_cast<const char*>(in);
            DebugOut() << __SMALLFILE__ << ":" << __LINE__ << " Requested: " << data << "\n";
            observer->dataReceived(wsi, data, len);
            break;
        }
        case LWS_CALLBACK_ADD_POLL_FD:
        {
            //printf("Adding poll %i\n",sinkManager);
            DebugOut(5) << __SMALLFILE__ <<":"<< __LINE__ << "Adding poll" << endl;
            ws->addPoll(libwebsocket_get_socket_fd(wsi));
            break;
        }
        case LWS_CALLBACK_DEL_POLL_FD:
        {
            ws->removePoll(libwebsocket_get_socket_fd(wsi));
            break;
        }
        case LWS_CALLBACK_SET_MODE_POLL_FD:
        {
            //Set the poll mode
            break;
        }
        case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
        {
            //Don't handle this yet.
            break;
        }
        default:
        {
            //printf("Unhandled callback: %i\n",reason);
            DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Unhandled callback:" << reason << "\n";
            break;
        }
    }
    return 0;
}

static bool gioPollingFunc(GIOChannel *source, GIOCondition condition, gpointer data)
{
    //DebugOut(5) << "Polling..." << condition << endl;
    WebSockets* ws = static_cast<WebSockets*>(data);

    if(condition & G_IO_ERR)
    {
        DebugOut(0)<< __SMALLFILE__ <<":"<< __LINE__ <<" websocketsink polling error."<<endl;
    }

    if (condition & G_IO_HUP)
    {
        //Hang up. Returning false closes out the GIOChannel.
        //printf("Callback on G_IO_HUP\n");
        DebugOut(0)<<"socket hangup event..."<<endl;
        if(ws)
            ws->removePoll(g_io_channel_unix_get_fd(source));
        return false;
    }

    //This is the polling function. If it return false, glib will stop polling this FD.
    //printf("Polling...%i\n",condition);

    lws_tokens token;
    struct pollfd pollstruct;
    int newfd = g_io_channel_unix_get_fd(source);
    pollstruct.fd = newfd;
    pollstruct.events = condition;
    pollstruct.revents = condition;
    //libwebsocket_context* context = sinkManager->lwscontext();
    libwebsocket_service_fd(static_cast<WebSockets*>(data)->getContext(),&pollstruct);

    if(pollstruct.revents & G_IO_ERR)
    {
        DebugOut(0)<< __SMALLFILE__ <<":"<< __LINE__ <<" websocketsink polling error."<<endl;
    }

    if (pollstruct.revents & G_IO_HUP)
    {
        //Hang up. Returning false closes out the GIOChannel.
        //printf("Callback on G_IO_HUP\n");
        DebugOut(0)<<"socket hangup event..."<<endl;
        if(ws)
            ws->removePoll(g_io_channel_unix_get_fd(source));
        return false;
    }
    return true;
}

WebSockets::WebSockets(WebSocketsObserver& observer, Type t, int port, string ip) :
    observer(observer),
    protocollist({ { "http-only", websocket_callback, 0 }, { NULL, NULL, 0 } }),
    context(nullptr, &libwebsocket_context_destroy)
{
    //Protocol list for libwebsockets.
    //protocollist[0] = { "http-only", websocket_callback, 0 };
    //protocollist[1] = { NULL, NULL, 0 };

    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);
    info.iface = "any";
    info.protocols = protocollist;
    info.extensions = libwebsocket_get_internal_extensions();
    info.gid = -1;
    info.uid = -1;
    info.options = 0;
	info.port = port;
    info.user = this;
    //context = libwebsocket_create_context(&info);
    context = lwsContextPtr(libwebsocket_create_context(&info), &libwebsocket_context_destroy);

	if(t == WebSockets::Client)
	{
		libwebsocket_client_connect(context.get(), ip.c_str(), port, false, "/", "localhost", "websocket", protocollist[0].name, -1);
	}
}

WebSockets::~WebSockets()
{
    scoped_lock<interprocess_recursive_mutex> lock(mutex);
    context.reset();
    for (auto it = m_ioChannelMap.begin(); it != m_ioChannelMap.end(); ++it)
    {
        int fd = g_io_channel_unix_get_fd(it->second);
        g_io_channel_shutdown(it->second, false, 0);
        auto sourceIt = m_ioSourceMap.find(fd);
        if( sourceIt != m_ioSourceMap.end() ) {
            g_source_remove(sourceIt->second); //Since the watch owns the GIOChannel, this should unref it enough to dissapear.
            m_ioSourceMap.erase(sourceIt);
        }
    }
    m_ioChannelMap.clear();
    assert(m_ioSourceMap.empty() == true);
}

int WebSockets::Write(struct libwebsocket *lws, const std::string& strToWrite)
{
    std::unique_ptr<char[]> buffer(new char[LWS_SEND_BUFFER_PRE_PADDING + strToWrite.length() + LWS_SEND_BUFFER_POST_PADDING]);

    char *buf = buffer.get() + LWS_SEND_BUFFER_PRE_PADDING;
    strcpy(buf, strToWrite.c_str());

    //NOTE: delete[] on buffer is not needed since std::unique_ptr<char[]> is used
    return lws ? libwebsocket_write(lws, (unsigned char*)buf, strToWrite.length(), LWS_WRITE_TEXT) : 0;
}

void WebSockets::addPoll(int fd)
{
    scoped_lock<interprocess_recursive_mutex> lock(mutex);
    GIOChannel *chan = g_io_channel_unix_new(fd);
    stringstream ss;
    ss << chan;
    DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Adding chanel" << ss.str() << endl;
    guint sourceid = g_io_add_watch(chan, GIOCondition(G_IO_IN | G_IO_HUP | G_IO_ERR),(GIOFunc)gioPollingFunc, this);
    ss.str(std::string());
    ss << sourceid;
    DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Source is" << ss.str() << endl;
    g_io_channel_set_close_on_unref(chan,true);
    g_io_channel_unref(chan); //Pass ownership of the GIOChannel to the watch.
    m_ioChannelMap[fd] = chan;
    m_ioSourceMap[fd] = sourceid;
}

void WebSockets::removePoll(int fd)
{
    scoped_lock<interprocess_recursive_mutex> lock(mutex);
    auto channelIt = m_ioChannelMap.find(fd);
    if( channelIt != m_ioChannelMap.end() ) {
        stringstream ss;
        ss << channelIt->second;
        DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Erasing channel" << ss.str() << endl;
        g_io_channel_shutdown(channelIt->second,false,0);
        m_ioChannelMap.erase(channelIt);

    }
    auto sourceIt = m_ioSourceMap.find(fd);
    if( sourceIt != m_ioSourceMap.end() ) {
        stringstream ss;
        ss << sourceIt->second;
        DebugOut() << __SMALLFILE__ <<":"<< __LINE__ << "Erasing source" << ss.str() << endl;
        g_source_remove(sourceIt->second); //Since the watch owns the GIOChannel, this should unref it enough to dissapear.
        m_ioSourceMap.erase(sourceIt);
    }
}
