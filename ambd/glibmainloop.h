#ifndef _GLIBMAINLOOP_H_
#define _GLIBMAINLOOP_H_

#include "imainloop.h"
#include <glib.h>

class GlibMainLoop: public IMainLoop
{
public:

	GlibMainLoop(int argc, char** argv);
	~GlibMainLoop();

	int exec();

private:
	GMainLoop* mainLoop;
};


#endif
