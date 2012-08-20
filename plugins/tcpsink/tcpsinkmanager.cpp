/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2012  Michael Carpenter <email>

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


#include "tcpsinkmanager.h"

//Global variables, these will be moved into the class
struct pollfd pollfds[100];
int count_pollfds = 0;
libwebsocket_context *context;
static int websocket_callback(struct libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason, void *user,void *in, size_t len);
bool gioPollingFunc(GIOChannel *source,GIOCondition condition,gpointer data);


TcpSinkManager::TcpSinkManager(AbstractRoutingEngine* engine):AbstractSinkManager(engine)
{
	//new TcpSinkPlugin(engine);
	
	//Protocol list for libwebsockets.
	protocollist[0] = { "http-only", websocket_callback, 0 };
	protocollist[1] = { NULL, NULL, 0 };

	int port = 8080;
	const char *interface = "lo";
	const char *ssl_cert_path = NULL;
	const char *ssl_key_path = NULL;
	int options = 0;
	
	//Create a listening socket on port 8080 on localhost.
	context = libwebsocket_create_context(port, interface, protocollist,libwebsocket_internal_extensions,ssl_cert_path, ssl_key_path, -1, -1, options);
}
extern "C" AbstractSinkManager * create(AbstractRoutingEngine* routingengine)
{
	return new TcpSinkManager(routingengine);
}
static int websocket_callback(struct libwebsocket_context *context,struct libwebsocket *wsi,enum libwebsocket_callback_reasons reason, void *user,void *in, size_t len)
{
	printf("Switch: %i\n",reason);
	switch (reason)
	{
		case LWS_CALLBACK_CLIENT_WRITEABLE:
		{
			//Connection has been established.
			printf("Connection established\n");
			break;
		}
		case LWS_CALLBACK_HTTP:
		{
			//HTTP request
			char *requested_uri = (char *) in;
			printf("requested URI: %s\n", requested_uri);
		  
			if (strcmp(requested_uri, "/") == 0)
			{
				const char *universal_response = "Hello, World!";
				// http://git.warmcat.com/cgi-bin/cgit/libwebsockets/tree/lib/libwebsockets.h#n597
				libwebsocket_write(wsi, (unsigned char*)universal_response, strlen(universal_response), LWS_WRITE_HTTP);
			}
			//We're done, close the connection
			libwebsocket_close_and_free_session(context, wsi,LWS_CLOSE_STATUS_NORMAL);
			break;
		}
		case LWS_CALLBACK_ADD_POLL_FD:
		{
			//Add a FD to the poll list.
			printf("Adding poll\n");
			GIOChannel *chan = g_io_channel_unix_new((int)(long)user);
			g_io_add_watch(chan,G_IO_IN,(GIOFunc)gioPollingFunc,0);
			g_io_add_watch(chan,G_IO_PRI,(GIOFunc)gioPollingFunc,0);
			pollfds[count_pollfds].fd = (int)(long)user;
			pollfds[count_pollfds].events = (int)len;
			pollfds[count_pollfds++].revents = 0;
			break;
		}
		case LWS_CALLBACK_DEL_POLL_FD:
		{
			//Remove FD from the poll list.
			for (int n = 0; n < count_pollfds; n++)
				if (pollfds[n].fd == (int)(long)user)
					while (n < count_pollfds) {
						pollfds[n] = pollfds[n + 1];
						n++;
					}
			count_pollfds--;
			break;
		}
		case LWS_CALLBACK_SET_MODE_POLL_FD:
		{
			//Set the poll mode
			GIOChannel *chan = g_io_channel_unix_new((int)(long)user);
			g_io_add_watch(chan,(GIOCondition)(int)len,(GIOFunc)gioPollingFunc,0);
			break;
		}
		case LWS_CALLBACK_CLEAR_MODE_POLL_FD:
		{
			//Don't handle this yet.
			break;
		}
		default:
		{
			printf("Unhandled callback: %i\n",reason);
			break;
		}
	}
	return 0; 
}

bool gioPollingFunc(GIOChannel *source,GIOCondition condition,gpointer data)
{
	//This is the polling function. If it return false, glib will stop polling this FD.
	printf("Polling...%i\n",condition);
	struct pollfd pollstruct;
	int newfd = g_io_channel_unix_get_fd(source);
	pollstruct.fd = newfd;
	pollstruct.events = condition;
	pollstruct.revents = condition;
	libwebsocket_service_fd(context,&pollstruct);
	if (condition == G_IO_HUP)
	{
	  //Hang up. Returning false closes out the GIOChannel.
	  printf("Callback on G_IO_HUP\n");
	  return false;
	}
	
	return true;
}

