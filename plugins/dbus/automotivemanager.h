#ifndef AUTOMOTIVEMANGER_H_
#define AUTOMOTIVEMANAGER_H_

#include <gio/gio.h>

class AutomotiveManager
{
public:

	AutomotiveManager(GDBusConnection* connection);
	~AutomotiveManager();

private:

	GDBusConnection* mConnection;
	guint regId;
};


#endif //AUTOMOTOVEMANAGER_H_
