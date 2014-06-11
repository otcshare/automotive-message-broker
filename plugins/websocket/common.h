#ifndef COMMON_WEBSOCKET_H_
#define COMMON_WEBSOCKET_H_

#include <libwebsockets.h>
#include <QByteArray>
#include <QVariantMap>
#include <QJsonDocument>
#include <memory>

extern bool doBinary;

void cleanJson(QByteArray &json);

// libwebsocket_write helper function
int lwsWrite(struct libwebsocket *lws, QByteArray d, int len);

#endif
