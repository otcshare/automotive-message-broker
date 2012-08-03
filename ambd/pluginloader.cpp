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
#include <json-glib/json-glib.h>

using namespace std;

PluginLoader::PluginLoader(string configFile):f_create(NULL)
{
	JsonParser* parser = json_parser_new();
	GError* error = nullptr;
	if(!json_parser_load_from_file(parser, configFile.c_str(), error))
	{
		throw -1;
	}
	
	JsonReader* reader = json_reader_new(json_parser_get_root(parser));
	
	g_error_free(error);
}



std::string PluginLoader::errorString()
{
	return mErrorString;
}

