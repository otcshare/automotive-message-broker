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

template <class T>
class PtrMaker
{
public:
	typedef std::shared_ptr<T> Ptr;

	static std::shared_ptr<T> create()
	{
		return Ptr(new T());
	}
};

class Object : public std::unordered_map<std::string, std::shared_ptr<AbstractPropertyType>>, public PtrMaker<Object>
{
public:
	Object(): std::unordered_map<std::string, std::shared_ptr<AbstractPropertyType>>() { }
	Object(const std::string & ifaceName): std::unordered_map<std::string, std::shared_ptr<AbstractPropertyType>>(),
		interfaceName(ifaceName)
	{

	}

	static Object::Ptr fromJson(const picojson::object & obj);

	static picojson::value toJson(const Object::Ptr & obj);

	std::string interfaceName;
};


class BaseMessage
{
public:
	BaseMessage():BaseMessage("", "message") { }

	BaseMessage(std::string t): BaseMessage("", t) {}

	BaseMessage(std::string n, std::string t)
		: name(n), type(t)
	{
		messageId = amb::createUuid();
	}

	BaseMessage(const BaseMessage & other)
		: BaseMessage(other.name, other.type)
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
		:BaseMessage(name, "method"), zone(Zone::None)
	{

	}

	MethodCall(const BaseMessage & other)
		:BaseMessage(other), zone(Zone::None)
	{
		name = other.name;
	}

	MethodCall(const MethodCall & other)
		:MethodCall(other.name)
	{
		sourceUuid = other.sourceUuid;
		zone = other.zone;
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
};

template <class T>
class MethodReply
{
public:

	MethodReply(): MethodReply(nullptr, false) {}
	MethodReply(std::shared_ptr<T> t, bool success): mMethod(t), methodSuccess(success), err(AsyncPropertyReply::NoError) { }
	bool methodSuccess;

	picojson::value toJson()
	{
		picojson::value v = mMethod->toJson();

		picojson::object obj = v.get<picojson::object>();
		obj["methodSuccess"] = picojson::value(methodSuccess);
		obj["error"] = picojson::value((double)err);

		return picojson::value(obj);
	}

	bool fromJson(const picojson::value & json)
	{
		if(!mMethod) mMethod = std::shared_ptr<T>(new T());
		mMethod->fromJson(json);
		methodSuccess = json.get("methodSuccess").get<bool>();
		err = AsyncPropertyReply::Error(json.get("error").get<double>());

		return true;
	}

	static bool is(const picojson::value & v)
	{
		return v.contains("methodSuccess") && v.contains("error") && v.get("methodSuccess").is<bool>()
				&& v.get("error").is<double>() && T::is(v);
	}

	AsyncPropertyReply::Error error()
	{
		return err;
	}

	const std::string errorString() { return AsyncPropertyReply::errorToStr(err); }

	const std::shared_ptr<T> method() { return mMethod; }

protected:
	std::shared_ptr<T> mMethod;
	AsyncPropertyReply::Error err;
};

typedef std::function<void (std::vector<Object::Ptr>)> ListCallback;

class ListMethodCall : public MethodCall, public PtrMaker<ListMethodCall>
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

	std::vector<Object::Ptr> objectNames;

	ListCallback replyCallback;

	static bool is(const BaseMessage * msg)
	{
		return msg->name == "list";
	}

	static bool is(const picojson::value & json)
	{
		return json.get("name").to_str() == "list";
	}
};

typedef std::function<void (Object::Ptr)> ObjectCallback;

class GetMethodCall : public MethodCall, public PtrMaker<GetMethodCall>
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

	Object::Ptr value;

	ObjectCallback replyCallback;
};

typedef std::function<void (bool)> SetCallback;

class SetMethodCall : public MethodCall, public PtrMaker<SetMethodCall>
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
		return json.get("name").to_str() == "set";
	}

	Object::Ptr value;
	SetCallback replyCallback;
};

class SubscribeMethodCall : virtual public MethodCall, public PtrMaker<SubscribeMethodCall>
{
public:
	SubscribeMethodCall()
		:SubscribeMethodCall("")
	{

	}
	SubscribeMethodCall(const std::string & ifaceName)
		:MethodCall("subscribe"), interfaceName(ifaceName)
	{

	}

	picojson::value toJson();
	bool fromJson(const picojson::value &json);

	static bool is(const BaseMessage *msg)
	{
		return msg->name == "subscribe";
	}

	static bool is(const picojson::value & json)
	{
		return json.get("name").to_str() == "subscribe";
	}

	std::string interfaceName;
};

class UnsubscribeMethodCall : public MethodCall, public PtrMaker<UnsubscribeMethodCall>
{
public:
	UnsubscribeMethodCall()
		:UnsubscribeMethodCall("")
	{

	}

	UnsubscribeMethodCall(const SubscribeMethodCall & call)
		: UnsubscribeMethodCall(call.interfaceName)
	{
		sourceUuid = call.sourceUuid;
		zone = call.zone;
	}

	UnsubscribeMethodCall(const std::string & ifaceName)
		:MethodCall("unsubscribe"), interfaceName(ifaceName)
	{

	}

	static bool is(const BaseMessage *msg)
	{
		return msg->name == "unsubscribe";
	}

	static bool is(const picojson::value & json)
	{
		return json.get("name").to_str() == "unsubscribe";
	}

	picojson::value toJson();
	bool fromJson(const picojson::value &json);

	std::string interfaceName;
};

class EventMessage : public BaseMessage
{
public:
	EventMessage(const std::string & name)
		:BaseMessage("event", name) {}

	static bool is(const BaseMessage & msg)
	{
		return msg.type == "event";
	}

	static bool is(const picojson::value &json)
	{
		return json.get("type").to_str() == "event";
	}
};

class TimeSyncMessage : public BaseMessage, public PtrMaker<TimeSyncMessage>
{
public:
	TimeSyncMessage()
		:BaseMessage("timeSync", "message"), serverTime(0)
	{

	}

	double serverTime;

	picojson::value toJson();
	bool fromJson(const picojson::value &json);

	static bool is(const BaseMessage & msg)
	{
		return msg.type == "timeSync" && msg.name == "message";
	}

	static bool is(const picojson::value &json)
	{
		return json.contains("serverTime") && json.get("type").to_str() == "timeSync" && json.get("serverTime").is<double>();
	}
};

class PropertyChangeEvent: public EventMessage, public PtrMaker<PropertyChangeEvent>
{
public:
	PropertyChangeEvent() : EventMessage("propertyChanged") {}


	picojson::value toJson();
	bool fromJson(const picojson::value &json);

	static bool is(const BaseMessage & msg)
	{
		return msg.type == "event" && msg.name == "propertyChanged";
	}

	static bool is(const picojson::value &json)
	{
		return EventMessage::is(json) && json.get("name").to_str() == "propertyChanged" && json.contains("data") && json.get("data").is<picojson::object>();
	}

	Object::Ptr value;
	std::string sourceUuid;
	Zone::Type zone;
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

	template <class T>
	void send(std::shared_ptr<T> msg)
	{
		std::string buff = msg->toJson().serialize()+"\n";
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

	class Subscription
	{
	public:
		Subscription(SubscribeMethodCall subscribeCall, const ObjectCallback & cb)
			:call(subscribeCall), callback(cb) {}
		bool operator ==(const Subscription &rhs)
		{
			return rhs.subscriptionId() == subscriptionId();
		}

		const std::string subscriptionId() const { return call.messageId; }
		SubscribeMethodCall call;
		ObjectCallback callback;
	};

	AmbRemoteClient(AbstractIo* io);

	void list(ListCallback cb);

	void get(const std::string & objectName, ObjectCallback cb);

	void get(const std::string & objectName, const std::string & sourceUuid, ObjectCallback cb);

	void get(const std::string & objectName, Zone::Type zone, ObjectCallback cb);

	void get(const std::string & objectName, const std::string & sourceUuid, Zone::Type zone, ObjectCallback cb);

	void set(const std::string & objectName,  Object::Ptr value, SetCallback cb);

	void set(const std::string & objectName, Object::Ptr value, const std::string & sourceUuid, Zone::Type zone, SetCallback cb);

	const std::string subscribe(const std::string & objectName, const std::string & sourceUuid, Zone::Type zone, ObjectCallback cb);

	void subscribe(const std::string & objectName, ObjectCallback cb);

	void unsubscribe(const std::string & subscribeId);

protected:

	double correctTimeFromServer(double serverTimestamp);

private:

	void hasJsonMessage(const picojson::value & message);

	std::string createSubscriptionId(const std::string & objectName,  const std::string & sourceUuid, Zone::Type zone);
	std::vector<ListMethodCall::Ptr> mListCalls;
	std::vector<GetMethodCall::Ptr> mGetMethodCalls;
	std::vector<SetMethodCall::Ptr> mSetMethodCalls;
	std::unordered_map<std::string, std::vector<Subscription>> mSubscriptions;

	double serverTimeOffset;
};

class AmbRemoteServer : public BaseJsonMessageReader
{
public:
	AmbRemoteServer(AbstractIo* io, AbstractRoutingEngine* routingEngine);

protected:

	/*!
	 * \brief list called when a ListMessageCall was received
	 */
	virtual void list(ListMethodCall::Ptr call);

	/*!
	 * \brief get called when a GetMessageCall was received
	 */
	virtual void get(GetMethodCall::Ptr get);

	/*!
	 * \brief set called when SetMessageCall was received
	 */
	virtual void set(SetMethodCall::Ptr set);
	/*!
	 * \brief listen called when ListenMessageCall was received
	 */
	virtual void subscribe(SubscribeMethodCall::Ptr call);

	virtual void unsubscribe(UnsubscribeMethodCall::Ptr call);

	void hasJsonMessage(const picojson::value & json);

protected:
	AbstractRoutingEngine* routingEngine;


};

} //namespace amb

#endif
