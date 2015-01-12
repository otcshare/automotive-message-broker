Bluemonkey Source Plugin
Version: 0.13

Bluemonkey is a javascript rendering engine that allows the scripting of source plugin behavior in javascript.

To use the OBD-II Source plugin, add the following config to your /etc/ambd/config:


"sources" :
	[
		{
			"name" : "bluemonkey",
			"path" : "/usr/lib/x86_64-linux-gnu/automotive-message-broker/bluemonkeyplugin.so",
			"config" : "/etc/ambd/bluemonkey/config.js"
		}
	]

Configuration Key Definitions:

"name"
name of plugin.  This key is not used by the plugin at this moment.

"path"
path to plugin on the filesystem.

"config"
path to the bluemonkey configuration script.

Bluemonkey Javascript API:

see bluemonkey.idl (installed to /usr/share/doc/packages/automotive-message-broker/plugins/bluemonkey

