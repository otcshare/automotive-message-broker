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

/*!
 * \brief fromSignature get the basic type from gvariant signature
 * \param sig gvariant signature
 * \return string representing the basic type
 */
const std::string fromSignature(std::string const & sig);

/*!
 * \brief fromAbstractProperty get the basic type from AbstractPropertType
 * \param property
 * \return
 */
const std::string fromAbstractProperty(AbstractPropertyType *property);

} // BasicTypes

picojson::value gvariantToJson(GVariant* value);

GVariant * jsonToGVariant(const picojson::value & json, const std::string & signature);

std::shared_ptr<AbstractPropertyType> jsonToProperty(const picojson::value& json);
}

#endif

