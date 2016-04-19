#ifndef COMMON_WEBSOCKET_H_
#define COMMON_WEBSOCKET_H_

#include <QJsonDocument>
#include <libwebsockets.h>
#include <memory>

extern bool doBinary;

void cleanJson(QByteArray &json);

// libwebsocket_write helper function
int lwsWrite(struct libwebsocket *lws, QByteArray d);
int lwsWriteVariant(struct libwebsocket *lws, QVariant d);

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
} // amb

#endif
