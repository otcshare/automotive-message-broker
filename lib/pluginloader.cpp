/*
    Copyright 2012 Kevron Rees <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "pluginloader.h"
#include <iostream>

using namespace std;

PluginLoader::PluginLoader(std::string pluginPath)
:mPluginPath(pluginPath),f_create(NULL)
{
	
}

bool PluginLoader::load()
{
	if(lt_dlinit())
	{
		mErrorString = lt_dlerror();
		cerr<<"error initializing libtool: "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
		return false;
	}
	
	lt_dlerror();
	
	lt_dlhandle handle = lt_dlopenext(mPluginPath.c_str());
	
	if(!handle)
	{
		mErrorString = lt_dlerror();
		cerr<<"error opening plugin: "<<mPluginPath<<" in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
		return false;
	}
	
	f_create = (create_t *)lt_dlsym(handle, "create");
	
	//mErrorString = lt_dlerror();
	if(f_create) 
	{
		f_create();
		return true;
	}
	
	return false;
}

std::string PluginLoader::errorString()
{
	return mErrorString;
}

