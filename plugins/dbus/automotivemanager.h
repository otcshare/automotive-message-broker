#ifndef AUTOMOTIVEMANGER_H_
#define AUTOMOTIVEMANAGER_H_

#include <gio/gio.h>
#include <map>
#include <list>

class AbstractDBusInterface;

class AutomotiveManager
{
public:

	AutomotiveManager(GDBusConnection* connection);
	~AutomotiveManager();

	std::map<AbstractDBusInterface*, std::list<std::string>> subscribedProcesses;

private:

	GDBusConnection* mConnection;
	guint regId;

};


#endif //AUTOMOTOVEMANAGER_H_
