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
#include "glibmainloop.h"
#include "core.h"
#include <json.h>
#include <iostream>
#include <stdexcept>
#include <boost/concept_check.hpp>
//#include <json-glib/json-glib.h>


using namespace std;

std::string get_file_contents(const char *filename)
{
	//FILE *in = fopen(filename,"r");

	std::ifstream in(filename, std::ios::in);
	std::string output;
	std::string line;
	while(in.good())
	{
		getline(in,line);
		output.append(line);
	}
	return output;
}
PluginLoader::PluginLoader(string configFile, int argc, char** argv): f_create(NULL), routingEngine(nullptr), mMainLoop(nullptr)
{
	DebugOut()<<"Loading config file: "<<configFile<<endl;
	json_object *rootobject;
	json_tokener *tokener = json_tokener_new();
	std::string configBuffer = get_file_contents(configFile.c_str());
	if(configBuffer == "")
	{
		throw std::runtime_error("No config or config empty");
	}
	enum json_tokener_error err;
	do
	{
		rootobject = json_tokener_parse_ex(tokener, configBuffer.c_str(),configBuffer.length());
	} while ((err = json_tokener_get_error(tokener)) == json_tokener_continue);
	if (err != json_tokener_success)
	{
		fprintf(stderr, "Error: %s\n", json_tokener_error_desc(err));
		// Handle errors, as appropriate for your application.
		throw std::runtime_error("Invalid config");
	}
	if (tokener->char_offset < configFile.length()) // XXX shouldn't access internal fields
	{
		// Handle extra characters after parsed object as desired.
		// e.g. issue an error, parse another object from that point, etc...
	}

	json_object *coreobject = json_object_object_get(rootobject,"routingEngine");
	if (coreobject)
	{
		/// there is a mainloop entry.  Load the plugin:

		string restr = string(json_object_get_string(coreobject));

		routingEngine = loadRoutingEngine(restr);

		if(!routingEngine)
		{
			DebugOut(DebugOut::Warning)<<"Failed to load routing engine plugin: "<<restr<<endl;
		}
	}

	if(!routingEngine)
	{
		/// there is no mainloop entry, use default glib
		DebugOut()<<"No routing engine specified in config.  Using built-in 'core' routing engine by default."<<endl;

		/// core wants some specific configuration settings:
		std::map<std::string,std::string> settings;

		json_object *lpq = json_object_object_get(rootobject,"lowPriorityQueueSize");
		if (lpq)
		{
			/// there is a mainloop entry.  Load the plugin:

			string restr = string(json_object_get_string(lpq));
			settings["lowPriorityQueueSize"] = restr;
		}

		json_object *npq = json_object_object_get(rootobject,"normalPriorityQueueSize");
		if (npq)
		{
			/// there is a mainloop entry.  Load the plugin:

			string restr = string(json_object_get_string(npq));
			settings["normalPriorityQueueSize"] = restr;
		}

		json_object *hpq = json_object_object_get(rootobject,"highPriorityQueueSize");
		if (hpq)
		{
			/// there is a mainloop entry.  Load the plugin:

			string restr = string(json_object_get_string(hpq));
			settings["highPriorityQueueSize"] = restr;
		}

		routingEngine = new Core(settings);
	}



	json_object *mainloopobject = json_object_object_get(rootobject,"mainloop");
	if (mainloopobject)
	{
		/// there is a mainloop entry.  Load the plugin:

		string mainloopstr = string(json_object_get_string(mainloopobject));

		mMainLoop = loadMainLoop(mainloopstr,argc, argv);

		if(!mMainLoop)
		{
			DebugOut(DebugOut::Warning)<<"Failed to load main loop plugin."<<endl;
		}
	}
	else if(!mMainLoop)
	{
		/// there is no mainloop entry, use default glib
		DebugOut()<<"No mainloop specified in config.  Using glib by default."<<endl;
		mMainLoop = new GlibMainLoop(argc,argv);
	}

	json_object *sourcesobject = json_object_object_get(rootobject,"sources");

	if(!sourcesobject)
	{
		DebugOut()<<"Error getting sources member: "<<endl;
		throw std::runtime_error("Error getting sources member");
	}

	//g_assert(json_reader_is_array(reader));
	g_assert(json_object_get_type(sourcesobject)==json_type_array);


	array_list *sourceslist = json_object_get_array(sourcesobject);
	if (!sourceslist)
	{
		DebugOut() << "Error getting source list" << endl;
		throw std::runtime_error("Error getting sources list");
	}

	for(int i=0; i < array_list_length(sourceslist); i++)
	{
		json_object *obj = (json_object*)array_list_get_idx(sourceslist,i); //This is an object

		std::map<std::string, std::string> configurationMap;
		json_object_object_foreach(obj, key, val)
		{
			string valstr = json_object_get_string(val);
			DebugOut() << "plugin config key: " << key << "value:" << valstr << endl;
			configurationMap[key] = valstr;
		}

		string path = configurationMap["path"];

		AbstractSource* plugin = loadPlugin<AbstractSource*>(path,configurationMap);

		if(plugin != nullptr)
		{
			mSources.push_back(plugin);
		}
	}

	//json_object_put(sourcesobject);
	///read the sinks:

	json_object *sinksobject = json_object_object_get(rootobject,"sinks");

	if (!sinksobject)
	{
		DebugOut() << "Error getting sink object" << endl;
		throw std::runtime_error("Error getting sink object");
	}

	array_list *sinkslist = json_object_get_array(sinksobject);


	if (!sinkslist)
	{
		DebugOut() << "Error getting sink list" << endl;
		throw std::runtime_error("Error getting sink list");
	}


	for(int i=0; i < array_list_length(sinkslist); i++)
	{
		json_object *obj = (json_object*)array_list_get_idx(sinkslist,i);

		std::map<std::string, std::string> configurationMap;

		json_object_object_foreach(obj, key, val)
		{
			string valstr = json_object_get_string(val);
			DebugOut() << "plugin config key: " << key << "value:" << valstr << endl;
			configurationMap[key] = valstr;
		}


		string path = configurationMap["path"];

		AbstractSinkManager* plugin = loadPlugin<AbstractSinkManager*>(path, configurationMap);

		if(plugin == nullptr)
		{
			DebugOut()<<"plugin is not a SinkManager"<<endl;

			AbstractSink* sink = loadPlugin<AbstractSink*>(path, configurationMap);

			if(!sink)
			{
				DebugOut(DebugOut::Warning)<<"plugin seems to be invalid: "<<path<<endl;
			}
		}
		else
		{
			mSinkManagers.push_back(plugin);
		}
	}

	//json_object_put(sinksobject);
	json_object_put(rootobject);
	json_tokener_free(tokener);

	Core* core = static_cast<Core*>(routingEngine);
	if( core != nullptr )
	{
		core->inspectSupported();
	}
}

PluginLoader::~PluginLoader()
{
	for(auto i :mSinkManagers)
	{
		delete i;
	}

	for(auto handle : openHandles)
		dlclose(handle);
}

IMainLoop *PluginLoader::mainloop()
{
	return mMainLoop;
}

SourceList PluginLoader::sources()
{
	return mSources;
}

std::string PluginLoader::errorString()
{
	return mErrorString;
}

