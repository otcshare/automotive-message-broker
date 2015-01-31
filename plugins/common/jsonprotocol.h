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

namespace BasicTypes
{
enum BasicTypeEnum
{
	UInt16,
	UInt32,
	Int16,
	Int32,
	String,
	Double,
	Boolean
};

extern const char * UInt16Str;
extern const char * UInt32Str;
extern const char * Int16Str;
extern const char * Int32Str;
extern const char * StringStr;
extern const char * DoubleStr;
extern const char * BooleanStr;

const std::string fromSignature(std::string const & sig);

} // BasicTypes

std::shared_ptr<AbstractPropertyType> json2Property(const picojson::value& json);

picojson::value property2Json(std::shared_ptr<AbstractPropertyType> property );

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
		return json.is<picojson::object>() && json.contains("type") && !json.contains("name") && json.contains("messageId");
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
		return json.contains("sourceUuid") && json.contains("zone");
	}

	virtual bool fromJson(const picojson::value &json);

	std::string sourceUuid;
	Zone::Type zone;
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

	std::vector<std::shared_ptr<AbstractPropertyType>> objectNames;

	static bool is(const BaseMessage * msg)
	{
		return msg->name == "list";
	}

	static bool is(const picojson::value & json)
	{
		return json.get("type").to_str() != "list" && json.is<picojson::array>();
	}
};

class BaseJsonMessageReader
{
public:
	BaseJsonMessageReader(AbstractIo* io);

	void canHasData();

protected:

	virtual void hasJsonMessage(const picojson::value & message) = 0;

	std::shared_ptr<AbstractIo> mIo;

private:

	std::string incompleteMessage;

};

class AmbRemoteClient: public BaseJsonMessageReader
{
public:
	typedef std::function<void (std::vector<std::shared_ptr<AbstractPropertyType>>)> ListCallback;
	typedef std::function<void (MapPropertyType<> *)> ObjectCallback;
	typedef std::function<void (bool)> SetCallback;

	AmbRemoteClient(AbstractIo* io);

	void list(ListCallback cb);

	void get(const std::string & objectName, ObjectCallback cb);

	void get(const std::string & objectName, const std::string & sourceUuid, ObjectCallback cb);

	void get(const std::string & objectName, Zone::Type zone, ObjectCallback cb);

	void get(const std::string & objectName, const std::string & sourceUuid, Zone::Type zone, ObjectCallback cb);

	void set(const std::string & objectName, MapPropertyType<>* value, ObjectCallback cb);

	void set(const std::string & objectName, MapPropertyType<>* value, const std::string & sourceUuid, Zone::Type zone, ObjectCallback cb);

	void listen(const std::string & objectName, const std::string & sourceUuid, Zone::Type zone, ObjectCallback cb);

	void listen(const std::string & objectName, ObjectCallback cb);

protected:

private:

	void hasJsonMessage(const picojson::value & message);

	std::string createSubscriptionId(const std::string & objectName,  const std::string & sourceUuid, Zone::Type zone);
	std::unordered_map<std::string, ListCallback> mListCalls;
	std::unordered_map<std::string, AsyncPropertyReply*> mMethodCalls;
	std::unordered_map<std::string, std::vector<ObjectCallback>> mSubsriptions;
};

class AmbRemoteServer : public BaseJsonMessageReader
{
public:
	AmbRemoteServer(AbstractIo* io);

protected:

	/*!
	 * \brief list called when a ListMessageCall was received
	 */
	virtual void list(ListMethodCall & call);

	/*!
	 * \brief get called when a GetMessageCall was received
	 */
	virtual void get();

	/*!
	 * \brief set called when SetMessageCall was received
	 */
	virtual void set();
	/*!
	 * \brief listen called when ListenMessageCall was received
	 */
	virtual void listen();

	void hasJsonMessage(const picojson::value & json);

	void send(BaseMessage* msg);
};

} //namespace amb

#endif
