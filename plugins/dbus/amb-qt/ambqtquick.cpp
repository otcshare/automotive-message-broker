#include "ambqtquick.h"

#include <QtDeclarative>

void Components::registerTypes(const char *uri)
{
	qmlRegisterType<AmbProperty>(uri,0,1,"AutomotivePropertyItem");
}

void Components::initializeEngine(QDeclarativeEngine *, const char *)
{

}

Q_EXPORT_PLUGIN(Components)
