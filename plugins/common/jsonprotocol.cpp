#include "jsonprotocol.h"

#include <jsonhelper.h>
#include <listplusplus.h>

#include <glib.h>

bool readCallback(GIOChannel *source, GIOCondition condition, gpointer data)
{
	if(condition & G_IO_ERR)
	{
		DebugOut(DebugOut::Error)<<"BaseJsonReader polling error."<<endl;
	}

	if (condition & G_IO_HUP)
	{
		//Hang up. Returning false closes out the GIOChannel.
		DebugOut(DebugOut::Warning)<<"socket hangup event..."<<endl;
		return false;
	}

	amb::BaseJsonMessageReader * p = static_cast<amb::BaseJsonMessageReader*>(data);

	p->canHasData();

	return true;
}

amb::AmbRemoteClient::AmbRemoteClient(AbstractIo *io)
	:BaseJsonMessageReader(io), serverTimeOffset(0)
{
	TimeSyncMessage timeSyncRequest;

	send(timeSyncRequest);
}

void amb::AmbRemoteClient::list(amb::ListCallback cb)
{
	ListMethodCall::Ptr methodCall = ListMethodCall::create();
	methodCall->replyCallback = cb;
	mListCalls.push_back(methodCall);

	send(methodCall);
}

void amb::AmbRemoteClient::get(const string &objectName, amb::ObjectCallback cb)
{
	get(objectName, "", Zone::None, cb);
}

void amb::AmbRemoteClient::get(const string &objectName, const string &sourceUuid, amb::ObjectCallback cb)
{
	get(objectName, sourceUuid, Zone::None, cb);
}

void amb::AmbRemoteClient::get(const string &objectName, Zone::Type zone, amb::ObjectCallback cb)
{
	get(objectName, "", zone, cb);
}

void amb::AmbRemoteClient::get(const string &objectName, const string &sourceUuid, Zone::Type zone, amb::ObjectCallback cb)
{
	GetMethodCall::Ptr getCall = GetMethodCall::create();
	getCall->sourceUuid = sourceUuid;
	getCall->zone = zone;
	getCall->value = amb::make_shared(new Object(objectName));
	getCall->replyCallback = cb;

	mGetMethodCalls.push_back(getCall);

	send(getCall);
}

void amb::AmbRemoteClient::set(const string &objectName, Object::Ptr value, SetCallback cb)
{
	set(objectName, value, "", Zone::None, cb);
}

void amb::AmbRemoteClient::set(const string &objectName, Object::Ptr value, const string &sourceUuid, Zone::Type zone, SetCallback cb)
{
	SetMethodCall::Ptr setCall = SetMethodCall::create();
	setCall->sourceUuid = sourceUuid;
	setCall->zone = zone;
	setCall->value = value;
	setCall->replyCallback = cb;

	mSetMethodCalls.push_back(setCall);

	send(setCall);
}

const string amb::AmbRemoteClient::subscribe(const string &objectName, const string &sourceUuid, Zone::Type zone, amb::ObjectCallback cb)
{
	std::string subscription = createSubscriptionId(objectName, sourceUuid, zone);

	SubscribeMethodCall call(objectName);

	Subscription sub(call, cb);

	mSubscriptions[subscription].push_back(sub);

	send(call);

	return call.messageId;
}

void amb::AmbRemoteClient::subscribe(const string &objectName, amb::ObjectCallback cb)
{
	subscribe(objectName, "", Zone::None, cb);
}

void amb::AmbRemoteClient::unsubscribe(const string &subscribeId)
{
	for(auto i : mSubscriptions)
	{
		auto subscriptions = &i.second;
		for(auto n : *subscriptions)
		{
			if(n.subscriptionId() == subscribeId)
			{
				removeOne(subscriptions, n);

				if(!subscriptions->size())
				{
					UnsubscribeMethodCall call(n.call);

					send(call);
				}
			}
		}
	}
}

double amb::AmbRemoteClient::correctTimeFromServer(double serverTimestamp)
{
	return serverTimestamp - serverTimeOffset;
}

void amb::AmbRemoteClient::hasJsonMessage(const picojson::value &json)
{
	DebugOut(7) << "json: " << json.serialize() << endl;

	if(BaseMessage::is<MethodReply<MethodCall>>(json))
	{
		if(BaseMessage::is<MethodReply<ListMethodCall>>(json))
		{
			MethodReply<ListMethodCall> listMethodReply;
			listMethodReply.fromJson(json);

			const ListMethodCall::Ptr listMethod = listMethodReply.method();

			auto itr = std::find_if(mListCalls.begin(), mListCalls.end(),[&listMethod](auto o)
			{
				return o->messageId == listMethod->messageId;
			});
			if(itr != mListCalls.end())
			{
				auto found = *itr;
				auto cb = found->replyCallback;

				try
				{
					cb(listMethod->objectNames);
				}
				catch(...)
				{
					DebugOut(DebugOut::Warning) << "callback for 'list' is not valid" << endl;
				}

				mListCalls.erase(itr);
			}
		}
		else if(BaseMessage::is<MethodReply<GetMethodCall>>(json))
		{
			MethodReply<GetMethodCall> reply;
			reply.fromJson(json);
			GetMethodCall::Ptr getCall = reply.method();

			auto itr = std::find_if(mGetMethodCalls.begin(), mGetMethodCalls.end(),[&getCall](auto o)
			{
				return o->messageId == getCall->messageId;
			});

			if(itr != mGetMethodCalls.end())
			{
				auto found = *itr;
				auto cb = found->replyCallback;

				try
				{
					cb(getCall->value);
				}
				catch(...)
				{
					DebugOut(DebugOut::Warning) << "Invalid Get callback " << endl;
				}

				mGetMethodCalls.erase(itr);
			}
		}
		else if(BaseMessage::is<MethodReply<SetMethodCall>>(json))
		{
			MethodReply<SetMethodCall> reply;
			reply.fromJson(json);

			auto call = reply.method();

			auto itr = std::find_if(mSetMethodCalls.begin(), mSetMethodCalls.end(),[&call](auto o)
			{
				return o->messageId == call->messageId;
			});

			if(itr != mSetMethodCalls.end())
			{
				auto found = *itr;
				auto cb = found->replyCallback;

				try
				{
					cb(reply.methodSuccess);
				}
				catch(...)
				{
					DebugOut(DebugOut::Warning) << "Invalid Set callback " << endl;
				}
				mSetMethodCalls.erase(itr);
			}
		}
	}
	else if(BaseMessage::is<MethodReply<TimeSyncMessage>>(json))
	{
		DebugOut(7) << "Received time sync message" << endl;
		MethodReply<TimeSyncMessage> reply;
		reply.fromJson(json);

		if(reply.methodSuccess)
		{
			serverTimeOffset = amb::Timestamp::instance()->epochTime() - reply.method()->serverTime;
		}
		else
		{
			DebugOut(DebugOut::Warning) << "Time Sync request failed" << endl;
		}
	}
	else if(BaseMessage::is<EventMessage>(json))
	{
		if(PropertyChangeEvent::is(json))
		{
			DebugOut(7) << "property changed event" << endl;

			PropertyChangeEvent::Ptr obj = PropertyChangeEvent::create();
			if(!obj->fromJson(json))
				return;

			std::string subscribeId = createSubscriptionId(obj->value->interfaceName, obj->sourceUuid, obj->zone);

			if(!amb::containsKey(mSubscriptions, subscribeId))
			{
				DebugOut(DebugOut::Warning) << "We haven't subscribed to this interface at this zone from this source..." << endl;
				return;
			}

			auto list = mSubscriptions[subscribeId];

			for(auto i : list)
			{
				i.callback(obj->value);
			}
		}
	}
	else
	{
		BaseMessage msg;
		msg.fromJson(json);
		DebugOut(DebugOut::Warning) << "Unhandled message: " << msg.name << " type: " << msg.type << endl;
	}
}

string amb::AmbRemoteClient::createSubscriptionId(const string & objectName, const string & sourceUuid, Zone::Type zone)
{
	std::string str = std::string(objectName + sourceUuid + std::to_string(zone));
	return g_compute_checksum_for_string(G_CHECKSUM_MD5, str.c_str(), str.length());
}

picojson::value amb::BaseMessage::toJson()
{
	picojson::object val;

	val["name"] = picojson::value(name);
	val["type"] = picojson::value(type);
	val["messageId"] = picojson::value(messageId);
	val["data"] = picojson::value(data);

	return picojson::value(val);
}

bool amb::BaseMessage::fromJson(const picojson::value &json)
{
	if(!json.is<picojson::object>() || !json.contains("type") || !json.contains("name") || !json.contains("messageId"))
	{
		DebugOut(DebugOut::Error) << "malformed message: is not json object or does not contain keys 'type', 'name' or 'messageId'." << endl;
		return false;
	}

	picojson::object obj = json.get<picojson::object>();

	type = obj["type"].to_str();
	name = obj["name"].to_str();
	messageId = obj["messageId"].to_str();

	if(json.contains("data"))
	{
		data = json.get("data");
	}

	return true;
}


picojson::value amb::ListMethodCall::toJson()
{
	picojson::object v = MethodCall::toJson().get<picojson::object>();

	picojson::array list;

	for(auto i : objectNames)
	{
		list.push_back(Object::toJson(i));
	}

	v["data"] = picojson::value(list);

	return picojson::value(v);
}

bool amb::ListMethodCall::fromJson(const picojson::value &json)
{
	if(!MethodCall::fromJson(json) || name != "list" || !data.is<picojson::array>())
	{
		DebugOut(DebugOut::Error) << "type not 'list' or data not type json array" << endl;
		return false;
	}

	objectNames.clear();

	picojson::array dataList = json.get("data").get<picojson::array>();

	for(auto i : dataList)
	{
		if(!i.is<picojson::object>())
		{
			DebugOut(DebugOut::Warning) << "Malformed data.  Expected 'object'.  Got '" << i.to_str() << "'" << endl;
			continue;
		}
		picojson::object obj = i.get<picojson::object>();

		Object::Ptr ambObj = Object::fromJson(obj);

		objectNames.push_back(ambObj);
	}

	return true;
}


amb::BaseJsonMessageReader::BaseJsonMessageReader(AbstractIo *io)
	:mIo(io)
{
	GIOChannel *chan = g_io_channel_unix_new(mIo->fileDescriptor());
	g_io_add_watch(chan, GIOCondition(G_IO_IN | G_IO_HUP | G_IO_ERR),(GIOFunc)readCallback, this);
	g_io_channel_set_close_on_unref(chan, true);
	g_io_channel_unref(chan);
}

void amb::BaseJsonMessageReader::canHasData()
{
	std::string d = mIo->read();
	incompleteMessage += d;

	while(hasJson());
}

void amb::BaseJsonMessageReader::closed()
{
	mIo->close();

	if(disconnected)
		disconnected();
}

bool amb::BaseJsonMessageReader::hasJson()
{
	std::string::size_type start = incompleteMessage.find("{");

	if(start == std::string::npos && incompleteMessage.empty())
	{
		return false;
	}

	if(start > 0)
	{
		DebugOut(7) << "We have an incomplete message at the beginning.  Toss it away:" << endl;
		DebugOut(7) << incompleteMessage << endl;
		incompleteMessage = incompleteMessage.substr(start-1);
	}

	unsigned int end = incompleteMessage.find("\n");

	if(end == std::string::npos)
	{
		return false;
	}

	std::string tryMessage = incompleteMessage.substr(0, end+1);

	DebugOut(6) << "Trying to parse message: " << tryMessage << endl;

	picojson::value doc;

	picojson::parse(doc, tryMessage);

	std::string parseError = picojson::get_last_error();

	if(!parseError.empty())
	{
		DebugOut(7) << "Invalid or incomplete message" << endl;
		DebugOut(7) << parseError << endl;
		return false;
	}

	incompleteMessage = end == incompleteMessage.length()-1 ? "" : incompleteMessage.substr(end+1);

	hasJsonMessage(doc);
	return true;
}

picojson::value amb::MethodCall::toJson()
{
	picojson::value value = BaseMessage::toJson();

	picojson::object obj = value.get<picojson::object>();

	obj["source"] = picojson::value(sourceUuid);
	obj["zone"] = picojson::value((double)zone);

	return picojson::value(obj);
}

bool amb::MethodCall::fromJson(const picojson::value &json)
{
	if(!BaseMessage::fromJson(json))
		return false;

	sourceUuid = json.get("source").to_str();
	zone = json.get("zone").get<double>();

	return true;
}

amb::AmbRemoteServer::AmbRemoteServer(AbstractIo *io, AbstractRoutingEngine *re)
	:BaseJsonMessageReader(io), routingEngine(re)
{

}

void amb::AmbRemoteServer::list(ListMethodCall::Ptr call)
{

}

void amb::AmbRemoteServer::get(GetMethodCall::Ptr get)
{

}

void amb::AmbRemoteServer::set(SetMethodCall::Ptr set)
{

}

void amb::AmbRemoteServer::subscribe(SubscribeMethodCall::Ptr call)
{

}

void amb::AmbRemoteServer::unsubscribe(amb::UnsubscribeMethodCall::Ptr call)
{

}

void amb::AmbRemoteServer::hasJsonMessage(const picojson::value &json)
{
	DebugOut(7) << "json: " << json.serialize() << endl;

	if(!BaseMessage::validate(json))
	{
		DebugOut(DebugOut::Warning) << "not a valid message: " << json.serialize() << endl;
		return;
	}

	if(BaseMessage::is<MethodCall>(json))
	{
		if(BaseMessage::is<ListMethodCall>(json))
		{
			ListMethodCall::Ptr listCall = ListMethodCall::create();
			listCall->fromJson(json);

			list(listCall);
		}
		else if(BaseMessage::is<GetMethodCall>(json))
		{
			GetMethodCall::Ptr getCall = GetMethodCall::create();
			getCall->fromJson(json);

			get(getCall);
		}
		else if(BaseMessage::is<SetMethodCall>(json))
		{
			SetMethodCall::Ptr setCall = SetMethodCall::create();
			setCall->fromJson(json);

			set(setCall);
		}
		else if(BaseMessage::is<SubscribeMethodCall>(json))
		{
			SubscribeMethodCall::Ptr call = SubscribeMethodCall::create();
			call->fromJson(json);

			subscribe(call);
		}
		else if(BaseMessage::is<UnsubscribeMethodCall>(json))
		{
			UnsubscribeMethodCall::Ptr call = UnsubscribeMethodCall::create();
			call->fromJson(json);

			unsubscribe(call);
		}
		else
		{
			BaseMessage call;
			call.fromJson(json);
			DebugOut(DebugOut::Warning) << "Unhandled method call: " << call.name << endl;
		}
	}
	else if(BaseMessage::is<TimeSyncMessage>(json))
	{
		TimeSyncMessage::Ptr call = TimeSyncMessage::create();
		call->fromJson(json);

		call->serverTime = amb::Timestamp::instance()->epochTime();

		MethodReply<TimeSyncMessage> reply(call, true);

		send(reply);
	}
	else
	{
		BaseMessage message;
		message.fromJson(json);

		DebugOut(DebugOut::Warning) << "Unhandled message: type: " << message.type << " name: " << message.name << endl;
	}
}

picojson::value amb::GetMethodCall::toJson()
{
	picojson::value val = MethodCall::toJson();

	picojson::object obj = val.get<picojson::object>();

	obj["data"] = Object::toJson(value);

	return picojson::value(obj);
}

bool amb::GetMethodCall::fromJson(const picojson::value &json)
{
	MethodCall::fromJson(json);

	value = Object::fromJson(json.get("data").get<picojson::object>());
	return true;
}


amb::Object::Ptr amb::Object::fromJson(const picojson::object &obj)
{
	if(!amb::containsKey(obj, "interfaceName"))
	{
		DebugOut(DebugOut::Warning) << "object missing interfaceName" << endl;
		return Object::Ptr(new Object());
	}
	Object * ambObj = new Object(obj.at("interfaceName").to_str());

	for(auto i : obj)
	{
		if(i.second.is<picojson::object>())
		{
			(*ambObj)[i.first] = std::shared_ptr<AbstractPropertyType>(amb::jsonToProperty(i.second));
		}
	}

	return Object::Ptr(ambObj);
}

picojson::value amb::Object::toJson(const Object::Ptr &obj)
{
	picojson::object jsonObj;
	jsonObj["interfaceName"] = picojson::value(obj->interfaceName);
	for(auto i : *obj.get())
	{
		jsonObj[i.first] = i.second->toJson();
	}

	return picojson::value(jsonObj);
}


picojson::value amb::SetMethodCall::toJson()
{
	picojson::value val = MethodCall::toJson();

	picojson::object obj = val.get<picojson::object>();

	obj["data"] = Object::toJson(value);

	return picojson::value(obj);
}

bool amb::SetMethodCall::fromJson(const picojson::value &json)
{
	MethodCall::fromJson(json);

	value = Object::fromJson(json.get("data").get<picojson::object>());

	return true;
}


picojson::value amb::SubscribeMethodCall::toJson()
{
	auto json = MethodCall::toJson();

	auto obj = json.get<picojson::object>();

	obj["interfaceName"] = picojson::value(interfaceName);

	return picojson::value(obj);
}

bool amb::SubscribeMethodCall::fromJson(const picojson::value &json)
{
	if(!MethodCall::fromJson(json))
		return false;

	interfaceName = json.get("interfaceName").to_str();

	return true;
}

picojson::value amb::UnsubscribeMethodCall::toJson()
{
	auto json = MethodCall::toJson();

	auto obj = json.get<picojson::object>();

	obj["interfaceName"] = picojson::value(interfaceName);

	return picojson::value(obj);
}

bool amb::UnsubscribeMethodCall::fromJson(const picojson::value &json)
{
	if(!MethodCall::fromJson(json))
		return false;

	interfaceName = json.get("interfaceName").to_str();

	return true;
}


picojson::value amb::TimeSyncMessage::toJson()
{
	auto val = BaseMessage::toJson();

	auto obj = val.get<picojson::object>();

	obj["serverTime"] = picojson::value(serverTime);

	return picojson::value(obj);
}

bool amb::TimeSyncMessage::fromJson(const picojson::value &json)
{
	if(!BaseMessage::fromJson(json))
		return false;

	serverTime = json.get("serverTime").get<double>();

	return true;
}


picojson::value amb::PropertyChangeEvent::toJson()
{
	auto val = EventMessage::toJson();

	auto obj = val.get<picojson::object>();
	obj["data"] = Object::toJson(value);
	obj["zone"] = picojson::value((double)zone);
	obj["source"] = picojson::value(sourceUuid);

	return picojson::value(obj);
}

bool amb::PropertyChangeEvent::fromJson(const picojson::value &json)
{
	if(!EventMessage::fromJson(json))
		return false;

	value = Object::fromJson(json.get("data").get<picojson::object>());

	return true;
}
