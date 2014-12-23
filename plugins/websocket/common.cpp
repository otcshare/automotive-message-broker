#include "common.h"
#include "debugout.h"
#include <math.h>
#include <glib.h>

#include <QVariantMap>

bool doBinary = false;

const char * amb::BasicTypes::UInt16Str = "UInt16";
const char * amb::BasicTypes::UInt32Str = "UInt32";
const char * amb::BasicTypes::Int16Str = "Int16";
const char * amb::BasicTypes::Int32Str = "Int32";
const char * amb::BasicTypes::StringStr = "String";
const char * amb::BasicTypes::DoubleStr = "Double";
const char * amb::BasicTypes::BooleanStr = "Boolean";

int lwsWrite(libwebsocket *lws, QByteArray d)
{
	if(!lws)
	{
		DebugOut(DebugOut::Error)<<__FUNCTION__<<": libwebsockets is not valid.  Perhaps it has not been initialized?" << endl;
		return -1;
	}

	DebugOut() << "Writing to websocket: " << d.constData() << endl;

	int retval = -1;

	QByteArray temp = d;

	int numframes = 1;
	int framesize = 5012;

	if(d.length() > framesize)
	{
		numframes = ceil((double)d.length() / double(framesize));
		QVariantMap multiFrameMessage;
		multiFrameMessage["type"] = "multiframe";
		multiFrameMessage["frames"] = numframes;

		lwsWriteVariant(lws, multiFrameMessage);
	}

	while(numframes--)
	{
		int range = 0;
		if(temp.length() > framesize)
			range = framesize;
		else range = temp.length();

		QByteArray toWrite = temp.mid(0,range);
		const char* strToWrite = toWrite.data();

		temp = temp.mid(range);

		if(doBinary)
		{
			retval = libwebsocket_write(lws, (unsigned char*)strToWrite, toWrite.length(), LWS_WRITE_BINARY);
		}
		else
		{
			std::unique_ptr<char[]> buffer(new char[LWS_SEND_BUFFER_PRE_PADDING + toWrite.length() + LWS_SEND_BUFFER_POST_PADDING]);
			char *buf = buffer.get() + LWS_SEND_BUFFER_PRE_PADDING;
			memcpy(buf, strToWrite, toWrite.length());

			retval = libwebsocket_write(lws, (unsigned char*)strToWrite, toWrite.length(), LWS_WRITE_TEXT);
		}
	}
	return retval;
}

int lwsWriteVariant(libwebsocket *lws, QVariant d)
{
	QByteArray replystr;
	if(doBinary)
		replystr = QJsonDocument::fromVariant(d).toBinaryData();
	else
	{
		replystr = QJsonDocument::fromVariant(d).toJson();
		cleanJson(replystr);
	}

	lwsWrite(lws, replystr);
}

void cleanJson(QByteArray &json)
{
	json.replace(" ", "");
	json.replace("\n", "");
	json.replace("\t", "");
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
