/*
Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <iostream>
#include <string>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdexcept>
#include <glib-object.h>

#ifdef USE_QT_CORE

#include <QCoreApplication>

#else 

#include <glib.h>

#endif

#include "pluginloader.h"
#include "core.h"

using namespace std;

#ifndef USE_QT_CORE

GMainLoop* mainLoop = nullptr;

#endif

void interrupt(int sign)
{
	signal(sign, SIG_IGN);
	cout<<"Signal caught. Exiting gracefully.\n"<<endl;
	
#ifdef USE_QT_CORE
	QCoreApplication::exit(0);
#else
	g_main_loop_quit(mainLoop);
	exit(0);
#endif
}

void daemonize();

void printhelp(const char *argv0);

static const char shortopts[] = "hvdc:";

static const struct option longopts[] = {
	{ "help", no_argument, NULL, 'h' }, ///< Print the help text
	{ "version", no_argument, NULL, 'v' }, ///< Print the version text
	{ "daemonise", no_argument, NULL, 'd' }, ///< Daemonise
	{ "config", required_argument, NULL, 'c' },
	{ NULL, 0, NULL, 0 } ///< End
};

int main(int argc, char **argv) 
{

	bool isdeamonize=false;
	int optc;
	string config;
	
	while ((optc = getopt_long (argc, argv, shortopts, longopts, NULL)) != -1)
	{
		switch (optc)
		{
			case 'd':
				isdeamonize = true;
				break;
				
			case 'v':
				cout<<PROJECT_NAME<<endl;
				cout<<"Version: "<<PROJECT_VERSION<<endl;
				return (0);
				break;
			case 'c':
				cout<<"Config: "<<optarg<<endl;
				config=optarg;
				break;
			default:
				cerr<<"Unknown option "<<optc<<endl;
				printhelp(argv[0]);
				return (0);
				break;
		}
	}
	
	if(isdeamonize)
		daemonize();
	
	
#ifdef USE_QT_CORE

	QCoreApplication app(argc,argv);

#else

	mainLoop = g_main_loop_new(NULL, false);
	
#endif
	
	g_type_init();
	
	PluginLoader loader(config, new Core());
	
	if(!loader.sources().size())
	{
		throw std::runtime_error("No sources present. aborting");
	}
	
	
	
#ifdef USE_QT_CORE
	
	app.exec();
	
#else
	
	g_main_loop_run(mainLoop);
	
#endif
	
	return 0;
}

void daemonize()
{
	int i=0;
	if(getppid() == 1)
	{
		return;	// already a daemon
	}
	if((i = fork()) < 0)
	{
		fprintf(stderr, "%s:%s(%d) - fork error: %s", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
		exit(1);
	}
	if(i > 0)
	{
		exit(0);	// parent exits
	}	// child (daemon) continues
	setsid();	// obtain a new process group
	for(i = getdtablesize(); i >= 0; --i)
	{
		close(i);	// close all descriptors
	}
	{	// handle standard I/O
	i = open("/dev/null", O_RDWR);
	dup(i);
	dup(i);
	}
	// first instance continues
}

void printhelp(const char *argv0)
{
	printf("Usage: %s [args]\n"
	"   [-d|--daemonise]\n"
	"   [-v|--version]\n"
	"   [-p]--plugin <pathtoplugin> \t]"
	"   [-h|--help]\n"
	, argv0);
}

