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

PluginLoader::PluginLoader(string configFile, AbstractRoutingEngine* re): f_create(NULL), routingEngine(re)
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
	
	gchar** members = json_reader_list_members(reader);
	
	for(int i=0; i< json_reader_count_members(reader); i++)
	{
		cout<<"member: "<<members[i]<<endl;
	}
	
	json_reader_read_member(reader,"sources");
	
	const GError * srcReadError = json_reader_get_error(reader);
	
	if(srcReadError != nullptr)
	{
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

SinkList PluginLoader::sinks()
{
	return mSinks;
}

SourceList PluginLoader::sources()
{
	return mSources;
}



std::string PluginLoader::errorString()
{
	return mErrorString;
}

