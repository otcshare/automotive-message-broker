Wheel source plugin
Version: 0.13

The wheel source plugin uses the Logitech G27 (or similar) racing wheel for data.

The wheel source plugin is enabled by default.


To use the wheel source plugin, add the following to the "sources" array in /etc/ambd/config:

{
	"name" : "WheelSource",
	"path" : "/usr/lib/x86_64-linux-gnu/automotive-message-broker/wheelsourceplugin.so",
	"device" : "/dev/input/js0"
}

Configuration Key Definitions:

"name"
name of plugin.  This key is not used by the plugin at this moment.

"path"
path to plugin on the filesystem.

"device"
Path to the racing wheel device

default: "/dev/input/js0"

