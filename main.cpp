#include <iostream>

#ifdef USE_QT_CORE

#include <QCoreApplication>

#else 

#include <glib.h>

#endif

#include "pluginloader.h"
#include "dbusinterfacemanager.h"

using namespace std;

int main(int argc, char **argv) {
    
#ifdef USE_QT_CORE

	QCoreApplication app(argc,argv);

#else

	GMainLoop* mainLoop = g_main_loop_new(NULL, false);
	
#endif
	
	PluginLoader loader("plugins/nobdyplugin.so");
	
	if(!loader.load())
		return -1;
	
	DBusInterfaceManager manager;

#ifdef USE_QT_CORE
	
	app.exec();
	
#else
	
	g_main_loop_run(mainLoop);
	
#endif
	
	return 0;
}
