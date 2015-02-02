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
	:BaseJsonMessageReader(io)
{

}

void amb::AmbRemoteClient::list(amb::AmbRemoteClient::ListCallback cb)
{
	ListMethodCall methodCall;

	mListCalls[methodCall.messageId] = cb;

	mIo->write(methodCall.toJson().serialize());
}

void amb::AmbRemoteClient::get(const string &objectName, amb::AmbRemoteClient::ObjectCallback cb)
{
	get(objectName, "", Zone::None, cb);
}

void amb::AmbRemoteClient::get(const string &objectName, const string &sourceUuid, amb::AmbRemoteClient::ObjectCallback cb)
{
	get(objectName, sourceUuid, Zone::None, cb);
}

void amb::AmbRemoteClient::get(const string &objectName, Zone::Type zone, amb::AmbRemoteClient::ObjectCallback cb)
{
	get(objectName, "", zone, cb);
}

void amb::AmbRemoteClient::get(const string &objectName, const string &sourceUuid, Zone::Type zone, amb::AmbRemoteClient::ObjectCallback cb)
{
	GetMethodCall getCall;
	getCall.sourceUuid = sourceUuid;
	getCall.zone = zone;

	mGetMethodCalls[getCall.messageId] = cb;

	send(getCall);
}

void amb::AmbRemoteClient::set(const string &objectName, const Object & value, SetCallback cb)
{
	set(objectName, value, "", Zone::None, cb);
}

void amb::AmbRemoteClient::set(const string &objectName, const Object & value, const string &sourceUuid, Zone::Type zone, SetCallback cb)
{
	SetMethodCall setCall;
	setCall.sourceUuid = sourceUuid;
	setCall.zone = zone;
	setCall.value = value;

	mSetMethodCalls[setCall.messageId] = cb;

	send(setCall);
}

void amb::AmbRemoteClient::listen(const string &objectName, const string &sourceUuid, Zone::Type zone, amb::AmbRemoteClient::ObjectCallback cb)
{

}

void amb::AmbRemoteClient::listen(const string &objectName, amb::AmbRemoteClient::ObjectCallback cb)
{

}

void amb::AmbRemoteClient::hasJsonMessage(const picojson::value &json)
{
	DebugOut(7) << "json: " << json.serialize() << endl;

	if(BaseMessage::is<MethodCall>(json))
	{
		if(BaseMessage::is<ListMethodCall>(json))
		{
			ListMethodCall listMethodCall;
			listMethodCall.fromJson(json);

			if(mListCalls.find(listMethodCall.messageId) != mListCalls.end())
			{
				auto cb = mListCalls[listMethodCall.messageId];

				try
				{
					cb(listMethodCall.objectNames);
				}
				catch(...)
				{
					DebugOut(DebugOut::Warning) << "callback for 'list' is not valid" << endl;
				}

				mListCalls.erase(listMethodCall.messageId);
			}
		}
		else if(BaseMessage::is<GetMethodCall>(json))
		{
			GetMethodCall getCall;
			getCall.fromJson(json);

			if(amb::containsKey(mGetMethodCalls, getCall.messageId))
			{
				auto cb = mGetMethodCalls[getCall.messageId];

				try
				{
					cb(getCall.value);
				}
				catch(...)
				{
					DebugOut(DebugOut::Warning) << "Invalid Get callback " << endl;
				}

				mGetMethodCalls.erase(getCall.messageId);
			}
		}
		else if(BaseMessage::is<SetMethodCall>(json))
		{

		}
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

		Object ambObj = Object::fromJson(obj);

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

	while(hasJson(incompleteMessage));
}

bool amb::BaseJsonMessageReader::hasJson(string & d)
{

	std::string::size_type start = d.find("{");

	if(start == std::string::npos && incompleteMessage.empty())
	{
		return false;
	}

	if(start > 0)
	{
		DebugOut(7) << "We have an incomplete message at the beginning.  Toss it away." << endl;
		d = d.substr(start-1);
	}


	std::string::size_type end = d.find_last_of("}");

	if(end == std::string::npos)
	{
		return false;
	}

	std::string tryMessage = d.substr(0, end+1);

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

	incompleteMessage = end == d.length()-1 ? "" : d.substr(end);

	hasJsonMessage(doc);
	return true;
}


picojson::value amb::MethodCall::toJson()
{
	picojson::value value = BaseMessage::toJson();

	picojson::object obj = value.get<picojson::object>();

	obj["source"] = picojson::value(sourceUuid);
	obj["zone"] = picojson::value((double)zone);
	obj["methodSuccess"] = picojson::value(success);

	return picojson::value(obj);
}

bool amb::MethodCall::fromJson(const picojson::value &json)
{
	if(!BaseMessage::fromJson(json))
		return false;

	sourceUuid = json.get("source").to_str();
	zone = json.get("zone").get<double>();

	if(json.contains("success"))
		success = json.get("methodSuccess").get<bool>();

	return true;
}


std::shared_ptr<AbstractPropertyType> amb::jsonToProperty(const picojson::value &json)
{
	std::string name = json.get("name").to_str();
	std::string type = json.get("type").to_str();

	auto t = VehicleProperty::getPropertyTypeForPropertyNameValue(name);

	if(!t)
	{
		bool ret = VehicleProperty::registerProperty(name, [name, type]() -> AbstractPropertyType* {
			if(type == amb::BasicTypes::UInt16Str)
			{
				return new BasicPropertyType<uint16_t>(name, 0);
			}
			else if(type == amb::BasicTypes::Int16Str)
			{
				return new BasicPropertyType<int16_t>(name, 0);
			}
			else if(type == amb::BasicTypes::UInt32Str)
			{
				return new BasicPropertyType<uint32_t>(name, 0);
			}
			else if(type == amb::BasicTypes::Int32Str)
			{
				return new BasicPropertyType<int32_t>(name, 0);
			}
			else if(type == amb::BasicTypes::StringStr)
			{
				return new StringPropertyType(name);
			}
			else if(type == amb::BasicTypes::DoubleStr)
			{
				return new BasicPropertyType<double>(name, 0);
			}
			else if(type == amb::BasicTypes::BooleanStr)
			{
				return new BasicPropertyType<bool>(name, false);
			}
			DebugOut(DebugOut::Warning) << "Unknown or unsupported type: " << type << endl;
			return nullptr;
		});

		if(!ret)
		{
			DebugOut(DebugOut::Error) << "failed to register property: " << name << endl;
			return nullptr;
		}

		t = VehicleProperty::getPropertyTypeForPropertyNameValue(name);
	}

	t->fromJson(json);

	return std::shared_ptr<AbstractPropertyType>(t);
}


amb::AmbRemoteServer::AmbRemoteServer(AbstractIo *io, AbstractRoutingEngine *re)
	:BaseJsonMessageReader(io), routingEngine(re)
{

}

void amb::AmbRemoteServer::list(ListMethodCall &call)
{

}

void amb::AmbRemoteServer::get(GetMethodCall & get)
{

}

void amb::AmbRemoteServer::set()
{

}

void amb::AmbRemoteServer::listen()
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
			ListMethodCall listCall;

			listCall.fromJson(json);

			list(listCall);
		}
		else if(BaseMessage::is<GetMethodCall>(json))
		{
			GetMethodCall getCall;
			getCall.fromJson(json);

			get(getCall);
		}
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

	value = Object::fromJson(json.get<picojson::object>());
}


amb::Object amb::Object::fromJson(const picojson::object &obj)
{
	if(!amb::containsKey(obj, "interfaceName"))
	{
		DebugOut(DebugOut::Warning) << "object missing interfaceName" << endl;
		return Object();
	}
	Object ambObj(obj.at("interfaceName").to_str());

	for(auto i : obj)
	{
		if(i.second.is<picojson::object>())
		{
			ambObj[i.first] = std::shared_ptr<AbstractPropertyType>(amb::jsonToProperty(i.second));
		}
	}

	return ambObj;
}

picojson::value amb::Object::toJson(const amb::Object &obj)
{
	picojson::object jsonObj;
	jsonObj["interfaceName"] = picojson::value(obj.interfaceName);
	for(auto i : obj)
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

	value = Object::fromJson(json.get<picojson::object>());
}
