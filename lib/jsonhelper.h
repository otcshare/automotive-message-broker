#ifndef _JSON_HELPER_H_
#define _JSON_HELPER_H_

#include "picojson.h"

#include <memory>

#include <glib.h>

class AbstractPropertyType;

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

const std::string fromAbstractProperty(AbstractPropertyType *property);

} // BasicTypes

picojson::value gvariantToJson(GVariant* value);

GVariant * jsonToGVariant(const picojson::value & json, const std::string & signature);

std::shared_ptr<AbstractPropertyType> jsonToProperty(const picojson::value& json);

picojson::value propertyToJson(std::shared_ptr<AbstractPropertyType> property);

void findJson(const std::string & buffer, std::string::size_type beg, std::string::size_type & end);

}

#endif

