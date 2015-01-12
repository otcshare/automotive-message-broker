example source and sink plugins
Version: 0.13

The example plugins are source and sink plugin that provide a simple example of how to create a plugin.
They are useful for testing AMB but only support a limited number of properties.

The example plugins is enabled by default.


To use the example source plugin, add the following to the "sources" array in /etc/ambd/config:

{
	  "name" : "ExampleSouce",
	  "path" : "/usr/lib/x86_64-linux-gnu/automotive-message-broker/examplesourceplugin.so",
	  "delay" : "1"
}

To use the example sink, add the following to the "sinks" array in /etc/ambd/config:

{
	  "name" : "ExampleSink",
	  "path" : "/usr/lib/x86_64-linux-gnu/automotive-message-broker/examplesinkplugin.so"
}

Configuration Key Definitions:

"name"
name of plugin.  This key is not used by the plugin at this moment.

"path"
path to plugin on the filesystem.

"delay" (source plugin only)
Delay in miliseconds when the source will generate a changed event

default: 1000
