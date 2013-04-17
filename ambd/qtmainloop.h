#ifndef _QTMAINLOOP_H_
#define _QTMAINLOOP_H_

#include <QCoreApplication>
#include "imainloop.h"

class QtMainLoop: public IMainLoop
{
public:

	QtMainLoop(int argc, char **argv);
	~QtMainLoop();

	int exec();
private:
	QCoreApplication* app;
};

#endif
