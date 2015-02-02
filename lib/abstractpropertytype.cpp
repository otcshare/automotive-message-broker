#include "abstractpropertytype.h"

const Zone::Type Zone::FrontRight = Zone::Type(Zone::Front | Zone::Right);
const Zone::Type Zone::FrontLeft = Zone::Type(Zone::Front | Zone::Left);
const Zone::Type Zone::MiddleRight = Zone::Type(Zone::Middle | Zone::Right);
const Zone::Type Zone::MiddleLeft = Zone::Type(Zone::Middle | Zone::Left);
const Zone::Type Zone::RearRight = Zone::Type(Zone::Rear | Zone::Right);
const Zone::Type Zone::RearLeft = Zone::Type(Zone::Rear | Zone::Left);


const picojson::value AbstractPropertyType::toJson()
{
	picojson::object obj;

	obj["name"] = picojson::value(name);
	obj["alias"] = picojson::value(alias());
	obj["source"] = picojson::value(sourceUuid);
	obj["zone"] = picojson::value((double)zone);
	obj["timestamp"] = picojson::value(timestamp);
	obj["sequence"] = picojson::value((double)sequence);
	obj["type"] = picojson::value(amb::BasicTypes::fromAbstractProperty(this));

	return picojson::value(obj);
}

void AbstractPropertyType::fromJson(const picojson::value &json)
{
	name = json.get("name").to_str();
	mAlias = json.get("alias").to_str();
	sourceUuid = json.get("source").to_str();
	zone = json.get("zone").get<double>();
	timestamp = json.get("timestamp").get<double>();
	sequence = json.get("sequence").get<double>();
}
