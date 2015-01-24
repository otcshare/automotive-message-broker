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

#include <gio/gio.h>
#include <picojson.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <boost/concept_check.hpp>

std::string get_file_contents(const char *filename)
{
    std::ifstream in(filename, std::ios::in);
    if(in.fail())
    {
        DebugOut(DebugOut::Error) << "Failed to open file '" << filename << "':' " << strerror(errno) << endl;
        return "";
    }

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
    std::string configBuffer = get_file_contents(configFile.c_str());

    std::string picojsonerr = "";
    picojson::value v;
    picojson::parse(v, configBuffer.begin(), configBuffer.end(), &picojsonerr);

    if(!picojsonerr.empty())
    {
        DebugOut(DebugOut::Error) << "Failed to parse main config! " << picojsonerr << endl;
        throw std::runtime_error("Error parsing config");
    }

    if(v.contains("routingEngine"))
    {
        string restr = v.get("routingEngine").to_str();

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


        for (auto q : {"lowPriorityQueueSize", "normalPriorityQueueSize", "highPriorityQueueSize"})
        {
            if (v.contains(q))
            {
                string restr = v.get(q).to_str();
                settings[q] = restr;
            }
        }

        routingEngine = new Core(settings);
    }


    if(v.contains("plugins"))
    {
        std::string pluginsPath = v.get("plugins").to_str();
        scanPluginDir(pluginsPath);
    }

    if(v.contains("mainloop"))
    {
        /// there is a mainloop entry.  Load the plugin:

        string mainloopstr = v.get("mainloop").to_str();

        mMainLoop = loadMainLoop(mainloopstr, argc, argv);

        if(!mMainLoop)
        {
            DebugOut(DebugOut::Warning)<<"Failed to load main loop plugin."<<endl;
        }
    }
    else if(!mMainLoop)
    {
        /// there is no mainloop entry, use default glib
        DebugOut()<<"No mainloop specified in config.  Using glib by default."<<endl;
        mMainLoop = new GlibMainLoop(argc, argv);
    }


    for (auto q : {"sources", "sinks"})
    {
        if(v.contains("sources"))
        {
            picojson::array list = v.get(q).get<picojson::array>();
            if (!list.size())
            {
                DebugOut() << "Error getting list for " << q << endl;
            }

            for(auto src : list)
            {
                std::map<std::string, std::string> configurationMap;
                for( auto obj : src.get<picojson::object>())
                {
                    string valstr = obj.second.to_str();
                    string key = obj.first;

                    DebugOut() << "plugin config key: " << key << "value:" << valstr << endl;

                    configurationMap[key] = valstr;
                }

                string path = configurationMap["path"];

                if(!loadPlugin(path, configurationMap))
                    DebugOut(DebugOut::Warning) << "Failed to load plugin: " << path <<endl;
            }

        }
    }

    Core* core = static_cast<Core*>(routingEngine);
    if( core != nullptr )
    {
        core->inspectSupported();
    }
}

PluginLoader::~PluginLoader()
{
    for(auto handle : openHandles)
        dlclose(handle);
}

IMainLoop *PluginLoader::mainloop()
{
    return mMainLoop;
}

std::string PluginLoader::errorString()
{
    return mErrorString;
}

void PluginLoader::scanPluginDir(const std::string & dir)
{
    DebugOut() << "Scanning plugin directory: " << dir << endl;

    auto pluginsDirectory = amb::make_gobject(g_file_new_for_path(dir.c_str()));

    GError* enumerateError = nullptr;

    auto enumerator = amb::make_gobject(g_file_enumerate_children(pluginsDirectory.get(), G_FILE_ATTRIBUTE_ID_FILE,
                                                                  G_FILE_QUERY_INFO_NONE, nullptr,
                                                                  &enumerateError));
    auto enumerateErrorPtr = amb::make_super(enumerateError);

    if(enumerateErrorPtr)
    {
        DebugOut(DebugOut::Error) << "Scanning plugin directory: " << enumerateErrorPtr->message << endl;
        return;
    }

    GError* errorGetFile = nullptr;
    while(auto pluginConfig = amb::make_gobject(g_file_enumerator_next_file(enumerator.get(), nullptr, &errorGetFile)))
    {
        std::string name = g_file_info_get_name(pluginConfig.get());

        DebugOut() << "Found file: " << name << endl;
        std::string fullpath = dir + (boost::algorithm::ends_with(dir, "/") ? "":"/") + name;
        std::string data = get_file_contents(fullpath.c_str());

        DebugOut() << "data: " << data << endl;

        if(!readPluginConfig(data))
        {
            DebugOut(DebugOut::Error) << "Reading contentds of file: " << name << endl;
        }
    }

    auto errorGetFilePtr = amb::make_super(errorGetFile);

    if(errorGetFilePtr)
    {
        DebugOut(DebugOut::Error) << "enumerating file: " << errorGetFilePtr->message << endl;
        return;
    }
}

bool PluginLoader::readPluginConfig(const string &configData)
{
    picojson::value v;
    std::string err;

    picojson::parse(v, configData.begin(), configData.end(), &err);

    if (!err.empty())
    {
        DebugOut(DebugOut::Error) << err << endl;
        return false;
    }

    std::string pluginName;
    if(v.contains("name"))
    {
        pluginName = v.get("name").to_str();
    }

    std::string pluginPath;
    if(v.contains("path"))
    {
        pluginPath = v.get("path").to_str();
    }
    else
    {
        DebugOut(DebugOut::Error) << "config missing 'path'." << endl;
        return false;
    }

    bool enabled = false;
    if(v.contains("enabled"))
    {
        enabled = v.get("enabled").get<bool>();
    }
    else
    {
        DebugOut(DebugOut::Error) << "config missing 'enabled'." << endl;
        return false;
    }

    DebugOut() << "Plugin: " << pluginName << endl;
    DebugOut() << "Path: " << pluginPath << endl;
    DebugOut() << "Enabled: " << enabled << endl;

    if(enabled)
    {
        std::map<std::string, std::string> otherConfig;

        picojson::object obj = v.get<picojson::object>();
        for(auto itr : obj)
        {
            if(!itr.second.is<std::string>())
                otherConfig[itr.first] = itr.second.serialize();
            else
                otherConfig[itr.first] = itr.second.to_str();
        }

        loadPlugin(pluginPath, otherConfig);
    }

    return true;
}

