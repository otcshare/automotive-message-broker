#include "ambqtquick.h"
#include <QtQml/qqml.h>

void Components::registerTypes(const char *uri)
{
	qmlRegisterType<AmbProperty>(uri,0,1,"AutomotivePropertyItem");

}

void Components::initializeEngine(QQmlEngine *, const char *)
{

}

