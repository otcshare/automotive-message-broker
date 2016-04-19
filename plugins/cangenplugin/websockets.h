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

#ifndef _WEBSOCKETS_H_
#define _WEBSOCKETS_H_

#include <map>
#include <memory>
#include <libwebsockets.h>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include "abstractpropertytype.h"

using namespace boost::interprocess;

/*! \addtogroup cangenplugin
 *  @{
 */

/*!
 * \brief WebSockets observer interface.
 *
 * dataReceived callback is called when any data are received.
 * \class WebSocketsObserver
 */
class WebSocketsObserver{
public:
    virtual ~WebSocketsObserver() { } /*LCOV_EXCL_LINE*/

    /*!
     * WebSocketsObserver getter
     * \fn dataReceived
     * \param socket Pointer to raw libwebsocket pointer to be used in reply if needed
     * \param data pointer to buffer where input data are stored
     * \param len length in bytes of the input data in buffer
     */
    virtual void dataReceived(libwebsocket* socket, const char* data, size_t len) = 0;
};

/*!
 * \brief Encapsulation of the libwebsockets library.
 *
 * Listens on \p \b 23001 port. Calls WebSocketsObserver::dataReceived callback when any data are received.
 * \sa <a href="http://libwebsockets.org/trac/libwebsockets">libwebsockets</a>
 *
 * \class WebSockets
 */
class WebSockets {
public:
	enum Type {
		Server,
		Client
	};

    /*!
     * \param observer \link #WebSocketsObserver Observer \endlink to be called when any data are received.
     */
	WebSockets(WebSocketsObserver& observer, Type t=Server, int port = 23001, std::string ip="");

    /*!
     * Copy constructor
     * @internal
     */
    WebSockets(const WebSockets&) = delete;
    /*!
     * Copy assignment
     * @return Reference to WebSockets
     * @internal
     */
    WebSockets& operator=(const WebSockets&) = delete;
    /*!
     * Move constructor
     * @internal
     */
    WebSockets(WebSockets&&) = delete;
    /*!
     * Move assignment
     * @return Reference to WebSockets
     * @internal
     */
    WebSockets& operator=(WebSockets&&) = delete;

    ~WebSockets();

    /*!
     * libwebsocket_write helper function
     * \fn Write
     * \param lws libwebsocket *
     * \param strToWrite Data to be written.
     * \return Number of the data bytes written.
     */
    static int Write(libwebsocket *lws, const std::string& strToWrite);

    /*!
     * libwebsocket helper function, called on LWS_CALLBACK_ADD_POLL_FD
     * \fn addPoll
     * \param fd libwebsocket raw file descriptor
     * \internal
     */
    void addPoll(int fd);

    /*!
     * libwebsocket helper function, called on LWS_CALLBACK_DEL_POLL_FD
     * \fn removePoll
     * \param fd libwebsocket raw file descriptor
     * \internal
     */
    void removePoll(int fd);

    /*!
     * WebSocketsObserver getter
     * \fn getObserver
     * \return \link #WebSocketsObserver Observer \endlink to be called when any data are received
     * \internal
     */
    inline WebSocketsObserver& getObserver() { return observer; }

    /*!
     * libwebsocket_context getter
     * \fn getContext
     * \return  Pointer to libwebsocket_context
     * \internal
     */
    inline libwebsocket_context* getContext() { return context.get(); }

private:

    /*!
     * \link #WebSocketsObserver Observer \endlink to be called when any data are received.
     * \private
     */
    WebSocketsObserver& observer;


    /*!
     * Supported libwebsocket protocols
     * \var protocollist;
     * \private
     */
    struct libwebsocket_protocols protocollist[2];

    /*!
     * libwebsocket_context pointer encapsulated in std::unique_ptr
     * \private
     */
    typedef std::unique_ptr<libwebsocket_context, decltype(&libwebsocket_context_destroy)> lwsContextPtr;

    /*!
     * Pointer to libwebsocket_context
     * \var context
     * \private
     */
    lwsContextPtr context;

    /*!
     * libwebsocket raw file descriptor to GIOChannel pointer map.
     * \var m_ioChannelMap
     * \private
     */
    std::map<int,GIOChannel*> m_ioChannelMap;

    /*!
     * libwebsocket raw file descriptor to glib source object id map.
     * \var m_ioSourceMap
     * \private
     */
    std::map<int,guint> m_ioSourceMap;

    /*!
     * Mutex to protect m_ioChannelMap and m_ioSourceMap containers during callbacks from libwebsockets
     * \var mutex
     * \private
     */
    interprocess_recursive_mutex mutex;
};

#endif // _WEBSOCKETS_H_

/** @} */
