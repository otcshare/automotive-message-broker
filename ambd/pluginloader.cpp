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

#include <iostream>
#include <stdexcept>
#include <json-glib/json-glib.h>


using namespace std;

/********************************************
 * Example JSON config:
 * {
 * 	sources: [ path1, path2, path3 ]
 * 	sinks: [ path1, path2, path3 ]
 * }
 * 
**********************************************/

PluginLoader::PluginLoader(string configFile, AbstractRoutingEngine* re, int argc, char** argv): f_create(NULL), routingEngine(re), mMainLoop(nullptr)
{
	DebugOut()<<"Loading config file: "<<configFile<<endl;
	
	JsonParser* parser = json_parser_new();
	GError* error = nullptr;
	if(!json_parser_load_from_file(parser, configFile.c_str(), &error))
	{
		DebugOut()<<"Failed to load config: "<<error->message;
		throw std::runtime_error("Failed to load config");
	}
	
	JsonNode* node = json_parser_get_root(parser);
	
	if(node == nullptr)
		throw std::runtime_error("Unable to get JSON root object");
	
	JsonReader* reader = json_reader_new(node);
	
	if(reader == nullptr)
		throw std::runtime_error("Unable to create JSON reader");
	
	DebugOut()<<"Config members: "<<json_reader_count_members(reader)<<endl;

	if(json_reader_read_member(reader,"mainloop"))
	{
		/// there is a mainloop entry.  Load the plugin:

		string mainloopstr = json_reader_get_string_value(reader);

		mMainLoop = loadMainLoop(mainloopstr,argc, argv);

		if(!mMainLoop)
		{
			DebugOut(0)<<"Failed to load main loop plugin."<<endl;
		}
	}
	else if(!mMainLoop)
	{
		/// there is no mainloop entry, use default glib
		DebugOut()<<"No mainloop specified in config.  Using glib by default."<<endl;
		mMainLoop = new GlibMainLoop(argc,argv);
	}
	
	json_reader_end_member(reader);

	if(!json_reader_read_member(reader,"sources"))
	{

		const GError * srcReadError = json_reader_get_error(reader);

		DebugOut()<<"Error getting sources member: "<<srcReadError->message<<endl;
		throw std::runtime_error("Error getting sources member");
	}
	
	g_assert(json_reader_is_array(reader));
	
	
	for(int i=0; i < json_reader_count_elements(reader); i++)
	{
		json_reader_read_element(reader,i);
		
		gchar** srcMembers = json_reader_list_members(reader);

		std::map<std::string, std::string> configurationMap;

		for(int i=0; i< json_reader_count_members(reader); i++)
		{
			json_reader_read_member(reader,srcMembers[i]);
			configurationMap[srcMembers[i]] = json_reader_get_string_value(reader);
			DebugOut()<<"plugin config key: "<<srcMembers[i]<<" value: "<<configurationMap[srcMembers[i]]<<endl;
			json_reader_end_member(reader);
		}

		json_reader_read_member(reader, "path");
		string path = json_reader_get_string_value(reader);
		json_reader_end_member(reader);

		AbstractSource* plugin = loadPlugin<AbstractSource*>(path,configurationMap);
		
		if(plugin != nullptr)
		{
			mSources.push_back(plugin);
		}

		json_reader_end_element(reader);
	}
			
	json_reader_end_member(reader);

	///read the sinks:
		
	json_reader_read_member(reader,"sinks");

	for(int i=0; i < json_reader_count_elements(reader); i++)
	{
		json_reader_read_element(reader,i);

		gchar** srcMembers = json_reader_list_members(reader);

		std::map<std::string, std::string> configurationMap;

		for(int i=0; i< json_reader_count_members(reader); i++)
		{
			json_reader_read_member(reader,srcMembers[i]);
			configurationMap[srcMembers[i]] = json_reader_get_string_value(reader);
			json_reader_end_member(reader);
		}

		json_reader_read_member(reader, "path");
		string path = json_reader_get_string_value(reader);
		json_reader_end_member(reader);

		AbstractSinkManager* plugin = loadPlugin<AbstractSinkManager*>(path, configurationMap);

		if(plugin == nullptr)
		{
			throw std::runtime_error("plugin is not a SinkManager");
		}

		json_reader_end_element(reader);
	}

	json_reader_end_member(reader);
	
	///TODO: this will probably explode:
	
	if(error) g_error_free(error);
	
	g_object_unref(reader);
	g_object_unref(parser);
	
}

PluginLoader::~PluginLoader()
{
	for(auto itr = mSinks.begin(); itr != mSinks.end(); itr++)
	{
		delete *itr;
	}

	for(auto itr = mSources.begin(); itr != mSources.end(); itr++)
	{
		delete *itr;
	}
}

SinkList PluginLoader::sinks()
{
	return mSinks;
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

