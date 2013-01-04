#include <QDeclarativeExtensionPlugin>
#include "ambqt.h"

class Components : public QDeclarativeExtensionPlugin
{
	Q_OBJECT
public:

	void registerTypes(const char *uri);
	void initializeEngine(QDeclarativeEngine *engine, const char *uri);

};
