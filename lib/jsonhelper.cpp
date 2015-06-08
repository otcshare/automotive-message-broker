#include "jsonhelper.h"

#include "abstractpropertytype.h"
#include "debugout.h"
#include "vehicleproperty.h"

const char * amb::BasicTypes::UInt16Str = "UInt16";
const char * amb::BasicTypes::UInt32Str = "UInt32";
const char * amb::BasicTypes::Int16Str = "Int16";
const char * amb::BasicTypes::Int32Str = "Int32";
const char * amb::BasicTypes::StringStr = "String";
const char * amb::BasicTypes::DoubleStr = "Double";
const char * amb::BasicTypes::BooleanStr = "Boolean";

picojson::value amb::gvariantToJson(GVariant* value)
{
	std::string type = g_variant_get_type_string(value);
	picojson::value v;

	if (type == "i")
	{
		int tempVal = GVS<int>::value(value);
		v = picojson::value(static_cast<double>(tempVal));
	}
	else if (type == "d")
	{
		v = picojson::value(GVS<double>::value(value));
	}
	else if (type == "q")
	{
		v = picojson::value(static_cast<double>(GVS<uint16_t>::value(value)));
	}
	else if (type == "n")
	{
		v = picojson::value(static_cast<double>(GVS<int16_t>::value(value)));
	}
	else if (type == "y")
	{
		v = picojson::value(static_cast<double>(GVS<char>::value(value)));
	}
	else if (type == "u")
	{
		v = picojson::value(static_cast<double>(GVS<uint32_t>::value(value)));
	}
	else if (type == "x")
	{
		v = picojson::value(static_cast<double>(GVS<int64_t>::value(value)));
	}
	else if (type == "t")
	{
		v = picojson::value(static_cast<double>(GVS<uint64_t>::value(value)));
	}
	else if (type == "b")
	{
		v = picojson::value(GVS<bool>::value(value));
	}
	else if (type == "s")
	{
		v = picojson::value(g_variant_get_string(value, nullptr));
	}
	else
	{
		DebugOut(DebugOut::Error) << "Unsupported type: " << type << endl;
	}

	return v;
}

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

const string amb::BasicTypes::fromAbstractProperty(AbstractPropertyType *property)
{
	return fromSignature(property->signature());
}


GVariant *amb::jsonToGVariant(const picojson::value & value, const std::string& type)
{
	GVariant* v = nullptr;

	if (type == "i") {
		v = g_variant_new(type.c_str(), (int32_t)value.get<double>());
	} else if (type == "d") {
		v = g_variant_new(type.c_str(), value.get<double>());
	} else if (type == "q") {
		v = g_variant_new(type.c_str(), (uint16_t)value.get<double>());
	} else if (type == "n") {
		v = g_variant_new(type.c_str(), (int16_t)value.get<double>());
	} else if (type == "u") {
		v = g_variant_new(type.c_str(), (uint32_t)value.get<double>());
	} else if (type == "x") {
		v = g_variant_new(type.c_str(), (int64_t)value.get<double>());
	} else if (type == "t") {
		v = g_variant_new(type.c_str(), (uint64_t)value.get<double>());
	} else if (type == "b") {
		v = g_variant_new(type.c_str(), value.get<bool>());
	} else if (type == "s") {
		v = g_variant_new(type.c_str(), value.get<std::string>().c_str());
	} else {
		DebugOut(DebugOut::Error) << "Unsupported type: " << type << endl;
	}

	return v;
}
