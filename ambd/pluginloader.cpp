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

PluginLoader::PluginLoader(string configFile): f_create(NULL)
{
	DebugOut()<<"Loading config file: "<<configFile<<endl;
	
	JsonParser* parser = json_parser_new();
	GError* error = nullptr;
	if(!json_parser_load_from_file(parser, configFile.c_str(), &error))
	{
		DebugOut()<<"Failed to load config: "<<error->message;
		throw std::runtime_error("Failed to load config");
	}
	
	JsonReader* reader = json_reader_new(json_parser_get_root(parser));
	
	if(reader == nullptr)
		throw std::runtime_error("Unable to create JSON reader");
	
	json_reader_read_member(reader,"sources");
	
	JsonNode* sourcesNode = json_reader_get_value(reader);
	
	if(sourcesNode != nullptr)
	{

		JsonArray* sourcesArray = json_node_get_array(sourcesNode);

		int sourcesLength = json_array_get_length(sourcesArray);

		for(int i=0; i < sourcesLength; i++)
		{
			string path = json_array_get_string_element(sourcesArray,i);
			AbstractSource* plugin = loadPlugin<AbstractSource*>(path);
			
			if(plugin != nullptr)
				mSources.push_back(plugin);
		}
			
		json_reader_end_member(reader);
		g_object_unref(sourcesArray);
	}
	
	else 
	{
		DebugOut()<<"Config contains no sources."<<endl;
	}
	
	///read the sinks:
		
	json_reader_read_member(reader,"sinks");
	
	JsonNode* sinksNode = json_reader_get_value(reader);
	
	if(sinksNode != nullptr)
	{
	
		JsonArray* sinksArray = json_node_get_array(sinksNode);
		
		int sinksLength = json_array_get_length(sinksArray);
		
		for(int i=0; i < sinksLength; i++)
		{
			string path = json_array_get_string_element(sinksArray,i);
			AbstractSink* plugin = loadPlugin<AbstractSink*>(path);
			
			if(plugin != nullptr)
				mSinks.push_back(plugin);
		}
		
		json_reader_end_member(reader);
		g_object_unref(sinksArray);
	}
	
	///TODO: this will probably explode:
	
	g_error_free(error);
	g_object_unref(reader);
	g_object_unref(parser);
	g_object_unref(sourcesNode);
	g_object_unref(sinksNode);
	
}

void PluginLoader::setSinkCreatedCb(SinkSignal cb)
{
	sinkCreatedCb = cb;
}

void PluginLoader::setSinkRemovedCb(SinkSignal cb)
{
	sinkRemovedCb = cb;
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

