#include <QQmlExtensionPlugin>
#include "ambqt.h"

class Components : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.automotive.qmlplugin")
public:


	void registerTypes(const char *uri);
	void initializeEngine(QQmlEngine *engine, const char *uri);

};
