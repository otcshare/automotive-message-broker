/*
Copyright (C) 2015 Intel Corporation

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

#ifndef AMB_JSON_PROTOCOL_H_
#define AMB_JSON_PROTOCOL_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <abstractroutingengine.h>
#include <mappropertytype.hpp>
#include <picojson.h>
#include <uuidhelper.h>
#include <vehicleproperty.h>

#include "abstractio.hpp"


namespace amb
{

class Object : public std::unordered_map<std::string, std::shared_ptr<AbstractPropertyType>>
{
public:
	Object(): std::unordered_map<std::string, std::shared_ptr<AbstractPropertyType>>() { }
	Object(const std::string & ifaceName): std::unordered_map<std::string, std::shared_ptr<AbstractPropertyType>>(),
		interfaceName(ifaceName)
	{

	}

	static Object fromJson(const picojson::object & obj);

	static picojson::value toJson(const Object & obj);

	std::string interfaceName;

};

class BaseMessage
{
public:
	BaseMessage() { }

	BaseMessage(std::string n, std::string t)
		: name(n), type(t)
	{
		messageId = amb::createUuid();
	}

	BaseMessage(const BaseMessage & other)
		: name(other.name), type(other.type), data(other.data)
	{

	}

	std::string name;
	std::string type;

	std::string messageId;

	virtual picojson::value toJson();
	virtual bool fromJson(const picojson::value & json);

	template <typename T>
	bool is()
	{
		return T::is(this);
	}

	static bool validate(const picojson::value & json)
	{
		return json.is<picojson::object>() && json.contains("type") && json.contains("name") && json.contains("messageId");
	}

	template <typename T>
	static bool is(const picojson::value & json)
	{
		return T::is(json);
	}

protected:

	picojson::value data;

private:
};

class MethodCall : public BaseMessage
{
public:
	MethodCall(std::string name)
		:BaseMessage(name, "method"), zone(Zone::None), success(false)
	{

	}

	MethodCall(const BaseMessage & other)
		:BaseMessage(other), zone(Zone::None), success(false)
	{
		name = other.name;
	}

	MethodCall(const MethodCall & other)
		:MethodCall(other.name)
	{
		sourceUuid = other.sourceUuid;
		zone = other.zone;
		success = other.success;
	}

	static bool is(const BaseMessage * msg)
	{
		return (msg->type == "method");
	}

	/*!
	 * \brief is checks if json message is of this message type
	 * Assumes that json is already a valid \ref BaseMessage
	 * \param json
	 * \return
	 */
	static bool is(const picojson::value & json)
	{
		return json.contains("source") && json.get("source").is<std::string>()
				&& json.contains("zone") && json.get("zone").is<double>();
	}

	virtual picojson::value toJson();
	virtual bool fromJson(const picojson::value &json);

	std::string sourceUuid;
	Zone::Type zone;
	bool success;
};

class ListMethodCall : public MethodCall
{
public:
	ListMethodCall(): MethodCall("list") {}
	ListMethodCall(const MethodCall & other)
		:MethodCall(other)
	{
		if(!is(&other))
			throw std::runtime_error("type not list");
	}

	picojson::value toJson();
	bool fromJson(const picojson::value &json);

	std::vector<Object> objectNames;

	static bool is(const BaseMessage * msg)
	{
		return msg->name == "list";
	}

	static bool is(const picojson::value & json)
	{
		return json.get("name").to_str() == "list";
	}
};

class GetMethodCall : public MethodCall
{
public:
	GetMethodCall()
		: MethodCall("get")
	{

	}

	picojson::value toJson();
	bool fromJson(const picojson::value &json);

	static bool is(const BaseMessage * msg)
	{
		return msg->name == "get";
	}

	static bool is(const picojson::value & json)
	{
		return json.get("name").to_str() == "get";
	}

	Object value;
};

class SetMethodCall : public MethodCall
{
public:
	SetMethodCall()
		: MethodCall("set")
	{

	}

	picojson::value toJson();
	bool fromJson(const picojson::value &json);

	static bool is(const BaseMessage * msg)
	{
		return msg->name == "set";
	}

	static bool is(const picojson::value & json)
	{
		return json.get("name").to_str() != "set";
	}

	Object value;
};

class BaseJsonMessageReader
{
public:
	BaseJsonMessageReader(AbstractIo* io);

	void canHasData();

protected:

	virtual void hasJsonMessage(const picojson::value & message) = 0;

	template <class T>
	void send(T & msg)
	{
		std::string buff = msg.toJson().serialize()+"\n";
		DebugOut() << "writing: " << buff << endl;
		mIo->write(buff);
	}

	std::shared_ptr<AbstractIo> mIo;

private:

	bool hasJson();

	std::string incompleteMessage;

};

class AmbRemoteClient: public BaseJsonMessageReader
{
public:
	typedef std::function<void (std::vector<Object>)> ListCallback;
	typedef std::function<void (Object&)> ObjectCallback;
	typedef std::function<void (bool)> SetCallback;

	AmbRemoteClient(AbstractIo* io);

	void list(ListCallback cb);

	void get(const std::string & objectName, ObjectCallback cb);

	void get(const std::string & objectName, const std::string & sourceUuid, ObjectCallback cb);

	void get(const std::string & objectName, Zone::Type zone, ObjectCallback cb);

	void get(const std::string & objectName, const std::string & sourceUuid, Zone::Type zone, ObjectCallback cb);

	void set(const std::string & objectName, const Object & value, SetCallback cb);

	void set(const std::string & objectName, const Object & value, const std::string & sourceUuid, Zone::Type zone, SetCallback cb);

	void listen(const std::string & objectName, const std::string & sourceUuid, Zone::Type zone, ObjectCallback cb);

	void listen(const std::string & objectName, ObjectCallback cb);

protected:

private:

	void hasJsonMessage(const picojson::value & message);

	std::string createSubscriptionId(const std::string & objectName,  const std::string & sourceUuid, Zone::Type zone);
	std::unordered_map<std::string, ListCallback> mListCalls;
	std::unordered_map<std::string, ObjectCallback> mGetMethodCalls;
	std::unordered_map<std::string, SetCallback> mSetMethodCalls;
	std::unordered_map<std::string, std::vector<ObjectCallback>> mSubsriptions;
};

class AmbRemoteServer : public BaseJsonMessageReader
{
public:
	AmbRemoteServer(AbstractIo* io, AbstractRoutingEngine* routingEngine);

protected:

	/*!
	 * \brief list called when a ListMessageCall was received
	 */
	virtual void list(ListMethodCall & call);

	/*!
	 * \brief get called when a GetMessageCall was received
	 */
	virtual void get(GetMethodCall &get);

	/*!
	 * \brief set called when SetMessageCall was received
	 */
	virtual void set();
	/*!
	 * \brief listen called when ListenMessageCall was received
	 */
	virtual void listen();

	void hasJsonMessage(const picojson::value & json);

protected:
	AbstractRoutingEngine* routingEngine;


};

} //namespace amb

#endif
