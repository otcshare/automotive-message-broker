#include "jsonprotocol.h"

#include <glib.h>

const char * amb::BasicTypes::UInt16Str = "UInt16";
const char * amb::BasicTypes::UInt32Str = "UInt32";
const char * amb::BasicTypes::Int16Str = "Int16";
const char * amb::BasicTypes::Int32Str = "Int32";
const char * amb::BasicTypes::StringStr = "String";
const char * amb::BasicTypes::DoubleStr = "Double";
const char * amb::BasicTypes::BooleanStr = "Boolean";

const std::string amb::BasicTypes::fromSignature(const string &sig)
{
	if(sig.empty()) return "";

	char c = sig[0];

	if(c == G_VARIANT_CLASS_BOOLEAN)
		return BooleanStr;

	else if(c == G_VARIANT_CLASS_BYTE)
		return "";

	else if(c == G_VARIANT_CLASS_INT16)
		return Int16Str;

	else if(c == G_VARIANT_CLASS_UINT16)
		return UInt16Str;

	else if(c == G_VARIANT_CLASS_INT32)
		return Int32Str;

	else if(c ==  G_VARIANT_CLASS_UINT32)
		return UInt32Str;

	else if(c == G_VARIANT_CLASS_INT64)
		return "";

	else if(c == G_VARIANT_CLASS_UINT64)
		return "";

	else if(c == G_VARIANT_CLASS_DOUBLE)
		return DoubleStr;

	else if(c == G_VARIANT_CLASS_STRING)
		return StringStr;

	else if(c == G_VARIANT_CLASS_ARRAY)
	{
		///TODO support array and map
		return "";
	}
	return "";
}

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

}

void amb::AmbRemoteClient::get(const string &objectName, const string &sourceUuid, amb::AmbRemoteClient::ObjectCallback cb)
{

}

void amb::AmbRemoteClient::get(const string &objectName, Zone::Type zone, amb::AmbRemoteClient::ObjectCallback cb)
{

}

void amb::AmbRemoteClient::get(const string &objectName, const string &sourceUuid, Zone::Type zone, amb::AmbRemoteClient::ObjectCallback cb)
{

}

void amb::AmbRemoteClient::set(const string &objectName, MapPropertyType<> *value, amb::AmbRemoteClient::ObjectCallback cb)
{

}

void amb::AmbRemoteClient::set(const string &objectName, MapPropertyType<> *value, const string &sourceUuid, Zone::Type zone, amb::AmbRemoteClient::ObjectCallback cb)
{

}

void amb::AmbRemoteClient::listen(const string &objectName, const string &sourceUuid, Zone::Type zone, amb::AmbRemoteClient::ObjectCallback cb)
{

}

void amb::AmbRemoteClient::listen(const string &objectName, amb::AmbRemoteClient::ObjectCallback cb)
{

}

void amb::AmbRemoteClient::hasJsonMessage(const picojson::value &json)
{
	BaseMessage message;

	message.fromJson(json);

	if(message.is<MethodCall>())
	{
		MethodCall methodCall(message);
		methodCall.fromJson(json);

		if(message.is<ListMethodCall>())
		{
			ListMethodCall listMethodCall(methodCall);

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

	if(data.contains("data"))
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
		list.push_back(property2Json(i));
	}

	v["data"] = picojson::value(list);

	return picojson::value(v);
}

bool amb::ListMethodCall::fromJson(const picojson::value &json)
{
	if(!MethodCall::fromJson(json) || json.get("type").to_str() != "list" || !data.is<picojson::array>())
	{
		DebugOut(DebugOut::Error) << "type not 'list' or data not type json array";
		return false;
	}

	objectNames.clear();

	for(auto i : data.get<picojson::array>())
	{
		objectNames.push_back(json2Property(i));
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

	std::string::size_type start = d.find("{");

	if(start == std::string::npos)
	{
		incompleteMessage += d;
		return;
	}

	if(incompleteMessage.empty() && start > 0)
	{
		DebugOut(7) << "We have an incomplete message at the beginning.  Toss it away." << endl;
		d = d.substr(start-1);
	}


	std::string::size_type end = d.find_last_of("}");

	if(end == std::string::npos)
	{
		incompleteMessage += d;
		return;
	}

	std::string tryMessage = incompleteMessage + d.substr(0, end+1);

	DebugOut(6) << "Trying to parse message: " << tryMessage << endl;

	picojson::value doc;

	picojson::parse(doc, tryMessage);

	std::string parseError = picojson::get_last_error();

	if(!parseError.empty())
	{
		DebugOut(7) << "Invalid or incomplete message" << endl;
		DebugOut(7) << parseError << endl;
		incompleteMessage += d;
		return;
	}

	incompleteMessage = end == d.length()-1 ? "" : d.substr(end);

	hasJsonMessage(doc);
}


bool amb::MethodCall::fromJson(const picojson::value &json)
{

	return true;
}


std::shared_ptr<AbstractPropertyType> amb::json2Property(const picojson::value &json)
{
	Zone::Type zone = json.get("zone").get<double>();
	std::string name = json.get("property").to_str();
	std::string type = json.get("type").to_str();
	std::string source = json.get("source").to_str();

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

	t->sourceUuid = source;
	t->zone = zone;

	return std::shared_ptr<AbstractPropertyType>(t);
}


picojson::value amb::property2Json(std::shared_ptr<AbstractPropertyType> property)
{
	std::string signature = property->signature();
	const std::string basicType = amb::BasicTypes::fromSignature(signature);

	picojson::object map;
	map["zone"] = picojson::value((double)property->zone);
	map["property"] = picojson::value(property->name);
	map["type"] = picojson::value(basicType);
	map["source"] = picojson::value(property->sourceUuid);

	return picojson::value(map);
}


amb::AmbRemoteServer::AmbRemoteServer(AbstractIo *io)
	:BaseJsonMessageReader(io)
{

}

void amb::AmbRemoteServer::list(ListMethodCall &call)
{

}

void amb::AmbRemoteServer::get()
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
	if(!BaseMessage::validate(json))
	{
		DebugOut(DebugOut::Warning) << "not a valid message" << endl;
	}

	if(BaseMessage::is<MethodCall>(json))
	{
		if(BaseMessage::is<ListMethodCall>(json))
		{
			ListMethodCall listCall;

			listCall.fromJson(json);

			list(listCall);

			send(&listCall);
		}
	}
}

void amb::AmbRemoteServer::send(amb::BaseMessage *msg)
{
	mIo->write(msg->toJson().serialize());
}
