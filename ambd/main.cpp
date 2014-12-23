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

static const char shortopts[] = "hvDc:d:l:we";

static const struct option longopts[] = {
	{ "help", no_argument, NULL, 'h' }, ///< Print the help text
	{ "version", no_argument, NULL, 'v' }, ///< Print the version text
	{ "daemonise", no_argument, NULL, 'D' }, ///< Daemonise
	{ "config", required_argument, NULL, 'c' },
	{ "debug", required_argument, NULL, 'd' },
	{ "log", required_argument, NULL, 'l' },
	{ "warn", no_argument, NULL, 'w' },
	{ "err", no_argument, NULL, 'e' },
	{ NULL, 0, NULL, 0 } ///< End
};

void printVersion()
{
	DebugOut(0)<<PROJECT_NAME<<endl;
	DebugOut(0)<<"Version: "<<PROJECT_VERSION<<" ( "<<PROJECT_CODENAME<<" "<<PROJECT_QUALITY<<" )"<<endl;
}

static void glibLogHandler(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data)
{
	if(log_level == G_LOG_LEVEL_CRITICAL)
		DebugOut(DebugOut::Error) << message << endl;
	else if(log_level == G_LOG_LEVEL_WARNING)
		DebugOut(DebugOut::Warning) << message << endl;
	else
		DebugOut() << message << endl;
}

int main(int argc, char **argv)
{

	bool isdeamonize=false;
	int optc;
	int th = 0;
	string config="/etc/ambd/config";
	ofstream logfile;
	string logfn;

	g_log_set_handler(G_LOG_DOMAIN, GLogLevelFlags(G_LOG_LEVEL_WARNING | G_LOG_LEVEL_CRITICAL), glibLogHandler, nullptr);

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
			case 'w':
				DebugOut::setThrowWarn(true);
				break;
			case 'e':
				DebugOut::setThrowErr(true);
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

	printVersion();

	if(getuid() == 0)
	{
		DebugOut(DebugOut::Warning)<<"Running as root.  This is dangerous."<<endl;
	}

#ifndef GLIB_VERSION_2_36
	g_type_init();
#endif

	VehicleProperty::factory();

	PluginLoader loader(config, argc, argv);

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
		   "   [-D|--daemonise] run ambd in daemon mode\n"
		   "   [-v|--version] spit out the version then exit\n"
		   "   [-c|--config </path/to/config> ] specify which config to use\n"
		   "   [-d|--debug <level (0-5)> ] set the debug level\n"
		   "   [-l]--log </path/to/logfile> ] specify an debug output log file\n"
		   "   [-w]--warn] throw on warnings\n"
		   "   [-e]--err] throw on errors\n"
		   "   [-h|--help] print this menu and exit\n"
		   , argv0);
}


