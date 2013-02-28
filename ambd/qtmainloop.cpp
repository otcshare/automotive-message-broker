#include "qtmainloop.h"

extern "C" IMainLoop * create(int argc, char** argv)
{
	return new QtMainLoop(argc, argv);
}

QtMainLoop::QtMainLoop(int argc, char** argv)
	:IMainLoop(argc,argv)
{
	app = new QCoreApplication(argc,argv);
}

QtMainLoop::~QtMainLoop()
{
	app->exit(0);
}

int QtMainLoop::exec()
{
	app->exec();
}
