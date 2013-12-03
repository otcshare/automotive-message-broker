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
#include <ltdl.h>
#include <iostream>

#include "abstractsource.h"
#include "abstractsink.h"
#include "abstractroutingengine.h"
#include "debugout.h"
#include "imainloop.h"


using namespace std;

typedef void* create_t(AbstractRoutingEngine*, map<string, string> );
typedef void* create_mainloop_t(int argc, char** argv);

class PluginLoader
{

public:
	PluginLoader(string configFile, AbstractRoutingEngine* routingEngine, int argc, char** argv);
	~PluginLoader();

	SourceList sources();

	IMainLoop* mainloop();

	std::string errorString();
        
	
private: ///methods:
	
	template<class T>
	T loadPlugin(string pluginName, map<string, string> config)
	{
		DebugOut()<<"Loading plugin: "<<pluginName<<endl;
		
		if(lt_dlinit())
		{
			mErrorString = lt_dlerror();
			cerr<<"error initializing libtool: "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
			return nullptr;
		}
		
		lt_dlerror();
		
		lt_dlhandle handle = lt_dlopenext(pluginName.c_str());
		
		if(!handle)
		{
			mErrorString = lt_dlerror();
			cerr<<"error opening plugin: "<<pluginName<<" in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
			return nullptr;
		}
		
		f_create = (create_t *)lt_dlsym(handle, "create");
		
		//mErrorString = lt_dlerror();
		
		if(f_create) 
		{
			void* obj = f_create(routingEngine, config);
			return static_cast<T>( obj );
		}
		
		return nullptr;
	}

	IMainLoop* loadMainLoop(string pluginName, int argc, char** argv)
		{
			DebugOut()<<"Loading plugin: "<<pluginName<<endl;

			lt_dlhandle handle = lt_dlopenext(pluginName.c_str());

			if(!handle)
			{
				mErrorString = lt_dlerror();
				cerr<<"error opening plugin: "<<pluginName<<" in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
				return nullptr;
			}
			openHandles.push_back(handle);

			m_create = (create_mainloop_t *)lt_dlsym(handle, "create");

			if(m_create)
			{
				void* obj = m_create(argc, argv);
				return static_cast<IMainLoop*>( obj );
			}

			return nullptr;
		}
	
private:
	
	std::string mPluginPath;
	std::string mErrorString;
	
	AbstractRoutingEngine* routingEngine;
	
	SourceList mSources;
	list<AbstractSinkManager*> mSinkManagers;
	
	create_t * f_create;
	create_mainloop_t * m_create;


	IMainLoop* mMainLoop;

	std::vector<lt_dlhandle> openHandles;
};

#endif // PLUGINLOADER_H
