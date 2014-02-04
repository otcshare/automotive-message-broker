#ifndef COMMON_WEBSOCKET_H_
#define COMMON_WEBSOCKET_H_

#include <libwebsockets.h>
#include <QByteArray>
#include <QVariantMap>
#include <QtMath>
#include <QJsonDocument>
#include <memory>

static bool doBinary = false;

static void cleanJson(QByteArray &json)
{
	json.replace(" ", "");
	json.replace("\n", "");
	json.replace("\t", "");
}

// libwebsocket_write helper function
static int lwsWrite(struct libwebsocket *lws, QByteArray d, int len)
{
	int retval = -1;

	QByteArray temp = d;

	int numframes = 1;
	int framesize = 122;

	if(d.length() > framesize)
	{
		numframes = qCeil((double)d.length() / 122.0);
		QVariantMap multiFrameMessage;
		multiFrameMessage["type"] = "multiframe";
		multiFrameMessage["frames"] = numframes;

		QByteArray msg;

		if(doBinary)
			msg = QJsonDocument::fromVariant(multiFrameMessage).toBinaryData();
		else
		{
			msg = QJsonDocument::fromVariant(multiFrameMessage).toJson();
			cleanJson(msg);
		}

		lwsWrite(lws, msg, msg.length());
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
			std::unique_ptr<char[]> buffer(new char[LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING]);
			char *buf = buffer.get() + LWS_SEND_BUFFER_PRE_PADDING;
			memcpy(buf, strToWrite, toWrite.length());

			retval = libwebsocket_write(lws, (unsigned char*)strToWrite, toWrite.length(), LWS_WRITE_TEXT);
		}
	}
		return retval;
}

#endif
