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

#endif
