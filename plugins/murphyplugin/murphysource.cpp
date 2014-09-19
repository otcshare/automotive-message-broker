/*
	Copyright (C) 2012  Intel Corporation

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

#include "murphysource.h"

#include <iostream>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <glib.h>
#include <sstream>
#include <listplusplus.h>
#include <timestamp.h>

#include <murphy/common.h>

/*#ifdef USE_QT_CORE
#include <murphy/qt/qt-glue.h>
#else*/

#include <murphy/glib/glib-glue.h>
//#endif

// #include <vehicleproperty.h>
// #include <abstractpropertytype.h>

#include "debugout.h"


void MurphySource::processValue(string propertyName, AbstractPropertyType *prop)
{
	if (murphyProperties.find(propertyName) != murphyProperties.end())
	{
		delete murphyProperties[propertyName];
		murphyProperties[propertyName] = prop;
	}
	else
	{
		murphyProperties[propertyName] = prop;
		m_re->updateSupported(supported(), PropertyList(), this);
	}

	// cout << "updating property!" << endl;
	m_re->updateProperty(prop, uuid());
}

bool MurphySource::hasProperty(string propertyName)
{
	PropertyList props = supported();
	return contains(props,propertyName);
}

static void recvfrom_msg(mrp_transport_t *transp, mrp_msg_t *msg,
						 mrp_sockaddr_t *addr, socklen_t addrlen,
						 void *user_data)
{
	MurphySource *s = (MurphySource *) user_data;

	void *cursor = NULL;
	uint16_t tag = 0;
	uint16_t type = 0;
	mrp_msg_value_t value;
	size_t size;

	std::string property_name;

	DebugOut()<<"Received a message from Murphy!"<<endl;

	if (!mrp_msg_iterate(msg, &cursor, &tag, &type, &value, &size))
		return;

	if (tag == 1 && type == MRP_MSG_FIELD_STRING)
		property_name = value.str;

	if (!mrp_msg_iterate(msg, &cursor, &tag, &type, &value, &size))
		return;

	if (tag != 2)
		return;

	DebugOut() << "Property '" << property_name << "' with value: " <<endl;

	bool hasProp = contains(VehicleProperty::capabilities(), property_name);

	stringstream val;

	switch (type)
	{
		case MRP_MSG_FIELD_STRING:
		{
			val << value.str;
			if (!hasProp)
			{
				VehicleProperty::registerProperty(property_name,
												  [property_name](){return new StringPropertyType(property_name, "");});
			}

			DebugOut() << "string:" << value.str << std::endl;
			break;
		}
		case MRP_MSG_FIELD_DOUBLE:
		{
			val << value.dbl;

			if (!hasProp)
			{
				VehicleProperty::registerProperty(property_name,
												  [property_name](){return new BasicPropertyType<double>(property_name, 0);});
			}

			DebugOut() << "double:" << value.dbl << std::endl;
			break;
		}
		case MRP_MSG_FIELD_BOOL:
		{
			val << value.bln;

			if (!hasProp)
			{
				VehicleProperty::registerProperty(property_name,
												  [property_name](){return new BasicPropertyType<bool>(property_name, FALSE);});
			}

			DebugOut() << "boolean:" << value.bln << std::endl;
			break;
		}
		case MRP_MSG_FIELD_UINT32:
		{
			val << value.u32;

			if (!hasProp)
			{
				VehicleProperty::registerProperty(property_name,
												  [property_name](){return new BasicPropertyType<uint32_t>(property_name, 0);});
			}

			DebugOut() << "uint32:" << value.u32 << std::endl;
			break;
		}
		case MRP_MSG_FIELD_UINT16:
		{
			val << value.u16;

			if (!hasProp)
			{
				VehicleProperty::registerProperty(property_name,
												  [property_name](){return new BasicPropertyType<uint16_t>(property_name, 0);});
			}

			DebugOut() << "uint16:" << value.u16 << std::endl;

			break;
		}
		case MRP_MSG_FIELD_INT32:
		{
			val << value.s32;

			if (!hasProp)
			{
				VehicleProperty::registerProperty(property_name,
												  [property_name](){return new BasicPropertyType<int32_t>(property_name, 0);});
			}

			DebugOut() << "int32:" << value.s32 << std::endl;
			break;
		}
		case MRP_MSG_FIELD_INT16:
		{
			val << value.s16;

			if (!hasProp)
			{
				VehicleProperty::registerProperty(property_name,
												  [property_name](){return new BasicPropertyType<int16_t>(property_name, 0);});
			}

			DebugOut() << "int16:" << value.s16 << std::endl;
			break;
		}
		default:
			DebugOut()<<"Unknown type"<<endl;
	}

	AbstractPropertyType* prop = VehicleProperty::getPropertyTypeForPropertyNameValue(property_name);

	prop->fromString(val.str());
	s->processValue(property_name, prop);
}

static void recv_msg(mrp_transport_t *transp, mrp_msg_t *msg, void *user_data)
{
	return recvfrom_msg(transp, msg, NULL, 0, user_data);
}

static void closed_evt(mrp_transport_t *t, int error, void *user_data)
{
   /* TODO: should process the error somehow */

	MurphySource *s = (MurphySource *) user_data;

	s->setState(MRP_PROCESS_STATE_NOT_READY);
	s->setConnected(FALSE);
}


void MurphySource::setConnected(bool connected)
{
	m_connected = connected;
}


int MurphySource::connectToMurphy()
{
	mrp_sockaddr_t addr;
	socklen_t alen;
	int flags = MRP_TRANSPORT_REUSEADDR | MRP_TRANSPORT_MODE_MSG;
	static mrp_transport_evt_t evt;
	const char *atype;

	evt.recvmsg = recv_msg;
	evt.recvmsgfrom = recvfrom_msg;
	evt.closed = closed_evt;

	if (m_ml == NULL || m_address.empty())
		return -1;

	if (m_connected == TRUE)
		return -1;

	alen = mrp_transport_resolve(NULL, m_address.c_str(), &addr, sizeof(addr), &atype);

	if (alen <= 0)
	{
		debugOut("Failed to resolve address");
		return -1;
	}

	m_tport = mrp_transport_create(m_ml, atype, &evt, this, flags);

	if (!m_tport)
	{
		debugOut("Can't create a Murphy transport");
		return -1;
	}

	if (mrp_transport_connect(m_tport, &addr, alen) == 0)
	{
		mrp_transport_destroy(m_tport);
		m_tport = NULL;
		debugOut("Failed to connect to Murphy");
		return -1;
	}

	setConnected(true);

	return 0;
}


MurphySource::MurphySource(AbstractRoutingEngine *re, map<string, string> config) : AbstractSource(re, config)
{
	m_source = this;
	m_re = re;
	m_connected = false;

	// main loop integration

/*#ifdef USE_QT_CORE
	m_ml = mrp_mainloop_qt_get();
	debugOut("Murphy plugin initialized using QT mainloop!");
#else*/
	GMainLoop *g_ml = g_main_loop_new(NULL, TRUE);
	m_ml = mrp_mainloop_glib_get(g_ml);
	debugOut("Murphy plugin initialized using glib mainloop!");
//#endif

	setConfiguration(config);
}

MurphySource::~MurphySource()
{
	mrp_process_set_state("ambd", MRP_PROCESS_STATE_NOT_READY);
	mrp_transport_destroy(m_tport);
	mrp_mainloop_unregister(m_ml);

	map<string, AbstractPropertyType *>::iterator i;

	for (i = murphyProperties.begin(); i != murphyProperties.end(); i++)
	{
		// TODO: unregister VehicleProperty (*i).first

		delete (*i).second;
	}
}

void MurphySource::setState(mrp_process_state_t state)
{
	m_state = state;
}

mrp_process_state_t MurphySource::getState()
{
	return m_state;
}

static void murphy_watch(const char *id, mrp_process_state_t state, void *user_data)
{
	MurphySource *s = (MurphySource *) user_data;

	debugOut("murphy process watch event");

	if (strcmp(id, "murphy-amb") != 0)
		return;

	printf("murphyd state changed to %s\n",
			state == MRP_PROCESS_STATE_READY ? "ready" : "not ready");

	if (state == MRP_PROCESS_STATE_NOT_READY &&
			s->getState() == MRP_PROCESS_STATE_READY)
	{
		DebugOut()<<"lost connection to murphyd"<<endl;
	}

	else if (state == MRP_PROCESS_STATE_READY)
	{
		/* start connecting if needed */
		s->connectToMurphy();
	}

	s->setState(state);
}

void MurphySource::readyToConnect(mrp_mainloop_t *ml)
{
	/* set a watch to follow Murphy status */

	if (mrp_process_set_watch("murphy-amb", ml, murphy_watch, this) < 0)
	{
		DebugOut()<<"failed to set a murphy process watch"<<endl;
		return;
	}

	mrp_process_set_state("ambd", MRP_PROCESS_STATE_READY);

	/* check if Murphy is running */
	m_state = mrp_process_query_state("murphy-amb");

	if (m_state == MRP_PROCESS_STATE_READY)
		connectToMurphy();
}


void MurphySource::setConfiguration(map<string, string> config)
{
	string address;

	for (map<string,string>::iterator i=configuration.begin();i!=configuration.end();i++)
	{
		if ((*i).first == "address")
		{
			address = (*i).second;
			// cout << "address: " << address << endl;

			// TODO: sanity check
			m_address = address;
		}
	}

	// TODO: read supported values from configuration?
	m_re->updateSupported(supported(), PropertyList(), this);

	// set up the connection with Murphy
	if (!m_address.empty())
		readyToConnect(m_ml);
}


PropertyList MurphySource::supported()
{
	// debugOut("> supported");

	PropertyList properties;
	map<string, AbstractPropertyType *>::iterator i;

	for (i = murphyProperties.begin(); i != murphyProperties.end(); i++)
	{
		properties.push_back((*i).first);
	}

	return properties;
}


int MurphySource::supportedOperations()
{
	// debugOut("> supportedOperations");
	return Get; // | Set;
	//we really don't support Set
}


void MurphySource::subscribeToPropertyChanges(VehicleProperty::Property property)
{
	// debugOut("> subscribeToPropertyChanges");
}


void MurphySource::unsubscribeToPropertyChanges(VehicleProperty::Property property)
{
	// debugOut("> unsubscribeToPropertyChanges");
}


void MurphySource::getPropertyAsync(AsyncPropertyReply *reply)
{
	// debugOut("> getPropertyAsync");

	if (murphyProperties.find(reply->property) != murphyProperties.end())
	{
		AbstractPropertyType *prop = murphyProperties[reply->property];
		reply->value = prop;
		reply->success = true;
		reply->completed(reply);
	}
}


void MurphySource::getRangePropertyAsync(AsyncRangePropertyReply *reply)
{
	// debugOut("> getRangePropertyAsync");
}


AsyncPropertyReply *MurphySource::setProperty(AsyncSetPropertyRequest request)
{
	// debugOut("> setProperty");

	processValue(request.property, request.value);

	AsyncPropertyReply* reply = new AsyncPropertyReply(request);
	reply->success = true;
	reply->completed(reply);
	return reply;
}

#if 1
extern "C" AbstractSource *create(AbstractRoutingEngine* routingengine, map<string, string> config)
{
	return new MurphySource(routingengine, config);
}
#endif
