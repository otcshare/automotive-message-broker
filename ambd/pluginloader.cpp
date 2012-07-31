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


#include "pluginloader.h"
#include <iostream>

using namespace std;

PluginLoader::PluginLoader(std::string pluginPath)
:mPluginPath(pluginPath),f_create(NULL)
{
	
}

AbstractSource* PluginLoader::loadSource()
{
	if(lt_dlinit())
	{
		mErrorString = lt_dlerror();
		cerr<<"error initializing libtool: "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
		return nullptr;
	}
	
	lt_dlerror();
	
	lt_dlhandle handle = lt_dlopenext(mPluginPath.c_str());
	
	if(!handle)
	{
		mErrorString = lt_dlerror();
		cerr<<"error opening plugin: "<<mPluginPath<<" in "<<__FILE__<<" - "<<__FUNCTION__<<":"<<__LINE__<<" "<<mErrorString<<endl;
		return nullptr;
	}
	
	f_create = (create_t *)lt_dlsym(handle, "create");
	
	//mErrorString = lt_dlerror();
	if(f_create) 
	{
		return f_create();
		
	}
	
	return nullptr;
}

std::string PluginLoader::errorString()
{
	return mErrorString;
}

