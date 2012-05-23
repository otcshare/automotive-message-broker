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


#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <string>
#include <ltdl.h>



typedef void create_t();

class PluginLoader
{

public:
    PluginLoader(std::string pluginPath);
        
    bool load();
    
    std::string errorString();
        
private:
	std::string mPluginPath;
	std::string mErrorString;
	
	create_t * f_create;
};

#endif // PLUGINLOADER_H
