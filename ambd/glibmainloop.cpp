#include "glibmainloop.h"
#include <glib.h>
#include <stdlib.h>

GlibMainLoop::GlibMainLoop(int argc, char **argv)
	:IMainLoop(argc,argv)
{
	mainLoop = g_main_loop_new(NULL, false);
}

GlibMainLoop::~GlibMainLoop()
{
	g_main_loop_quit(mainLoop);
	//exit(0);
}

int GlibMainLoop::exec()
{
	g_main_loop_run(mainLoop);
}

