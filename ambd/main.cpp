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
#include <debugout.h>

using namespace std;

IMainLoop* mainloop = nullptr;

void interrupt(int sign)
{
	signal(sign, SIG_IGN);
	cout<<"Signal caught. Exiting gracefully.\n"<<endl;
	
	/// this will cause the application to terminate and clean up:
	delete mainloop;
}

void daemonize();

void printhelp(const char *argv0);

static const char shortopts[] = "hvDc:d:l:";

static const struct option longopts[] = {
	{ "help", no_argument, NULL, 'h' }, ///< Print the help text
	{ "version", no_argument, NULL, 'v' }, ///< Print the version text
	{ "daemonise", no_argument, NULL, 'D' }, ///< Daemonise
	{ "config", required_argument, NULL, 'c' },
	{ "debug", required_argument, NULL, 'd' },
	{ "log", required_argument, NULL, 'l' },
	{ NULL, 0, NULL, 0 } ///< End
};

void printVersion()
{
	DebugOut(0)<<PROJECT_NAME<<endl;
	DebugOut(0)<<"Version: "<<PROJECT_VERSION<<" ("<<PROJECT_CODENAME<<")"<<endl;
}

int main(int argc, char **argv) 
{

	bool isdeamonize=false;
	int optc;
	int th = 0;
	string config="/etc/ambd/config";
	ofstream logfile;
	string logfn;

	while ((optc = getopt_long (argc, argv, shortopts, longopts, NULL)) != -1)
	{
		switch (optc)
		{
			case 'D':
				isdeamonize = true;
				break;
				
			case 'v':
				printVersion();
				return (0);
				break;
			case 'c':
				DebugOut(0)<<"Config: "<<optarg<<endl;
				config=optarg;
				break;
			case 'd':
				th = atoi(optarg);
				DebugOut::setDebugThreshhold(th);
				break;
			case 'l':
				logfn = optarg;
				break;
			default:
				DebugOut(0)<<"Unknown option "<<optc<<endl;
				printhelp(argv[0]);
				return (0);
				break;
		}
	}
	
	if(isdeamonize)
		daemonize();
	
	if(!logfn.empty())
	{
		logfile.open(logfn, ios::out | ios::trunc);
		DebugOut::setOutput(logfile);
	}

	VehicleProperty::factory();
	
	Core routingEngine;

	PluginLoader loader(config, &routingEngine , argc, argv);
	
	if(!loader.sources().size())
	{
		throw std::runtime_error("No sources present. aborting");
	}
		
	mainloop = loader.mainloop();

	/* Register signal handler */
	signal(SIGINT, interrupt);
	signal(SIGTERM, interrupt);

	mainloop->exec();
	
	if(logfile.is_open())
		logfile.close();

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
		   "   [-D|--daemonise]\n"
		   "   [-v|--version]\n"
		   "   [-c|--config </path/to/config> \t]\n"
		   "   [-d|--debug <level (0-5)>\t]\n"
		   "   [-l]--log </path/to/logfile>\t]\n"
		   "   [-h|--help]\n"
		   , argv0);
}

