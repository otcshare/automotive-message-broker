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

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <string>
#include <functional>
#include <dlfcn.h>
#include <iostream>

#include "abstractsource.h"
#include "abstractsink.h"
#include "abstractroutingengine.h"
#include "debugout.h"
#include "imainloop.h"

typedef void create_t(AbstractRoutingEngine*, map<string, string> );
typedef void* create_mainloop_t(int argc, char** argv);
typedef void* createRoutingEngine(void);

class PluginLoader
{

public:
	PluginLoader(std::string configFile, int argc, char** argv);
	~PluginLoader();

	IMainLoop* mainloop();

	std::string errorString();

	void scanPluginDir(const std::string &);

	bool readPluginConfig(const std::string & configData);

private: ///methods:


	bool loadPlugin(string pluginName, std::map<std::string, std::string> config)
	{
		DebugOut()<<"Loading plugin: "<<pluginName<<endl;

		void* handle = dlopen(pluginName.c_str(), RTLD_LAZY);

		if(!handle)
		{
			mErrorString = dlerror();
			DebugOut(DebugOut::Error)<<"error opening plugin: "<<pluginName<<" in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
			return false;
		}

		openHandles.push_back(handle);

		f_create = (create_t *)dlsym(handle, "create");

		if(f_create)
		{
			f_create(routingEngine, config);
			return true;
		}

		return false;
	}

	IMainLoop* loadMainLoop(string pluginName, int argc, char** argv)
	{
		DebugOut()<<"Loading plugin: "<<pluginName<<endl;

		void* handle = dlopen(pluginName.c_str(), RTLD_LAZY);

		if(!handle)
		{
			mErrorString = dlerror();
			DebugOut(DebugOut::Error)<<"error opening plugin: "<<pluginName<<" in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
			return nullptr;
		}

		openHandles.push_back(handle);

		m_create = (create_mainloop_t *)dlsym(handle, "create");

		if(m_create)
		{
			void* obj = m_create(argc, argv);
			return static_cast<IMainLoop*>( obj );
		}

		return nullptr;
	}
	AbstractRoutingEngine* loadRoutingEngine(string pluginName)
	{
		DebugOut()<<"Loading plugin: "<<pluginName<<endl;

		void* handle = dlopen(pluginName.c_str(), RTLD_LAZY);

		if(!handle)
		{
			mErrorString = dlerror();
			cerr<<"error opening plugin: "<<pluginName<<" in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
			return nullptr;
		}

		openHandles.push_back(handle);

		r_create = (createRoutingEngine *)dlsym(handle, "create");

		if(r_create)
		{
			void* obj = r_create();
			return static_cast<AbstractRoutingEngine*>( obj );
		}

		return nullptr;
	}

private:

	std::string mPluginPath;
	std::string mErrorString;

	AbstractRoutingEngine* routingEngine;

	create_t * f_create;
	create_mainloop_t * m_create;
	createRoutingEngine * r_create;

	IMainLoop* mMainLoop;

	std::vector<void*> openHandles;
};

#endif // PLUGINLOADER_H
