#include "common.h"
#include "debugout.h"
#include <math.h>
#include <glib.h>

#include <QVariantMap>

bool doBinary = false;

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

