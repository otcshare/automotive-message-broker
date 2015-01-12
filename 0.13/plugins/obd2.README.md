OBD-II Source Plugin
Version: 0.13

The OBD-II Source plugin uses ELM compatible scantools to get vehicle information.
It is known to work with a number of commercial scantools including:

OBDLink MX
OBDLink S
OBDPro USB

To use the OBD-II Source plugin, add the following config to your /etc/ambd/config:


"sources" : [
		{
			"name" : "OBD2Source",
			"path" : "/usr/lib/x86_64-linux-gnu/automotive-message-broker/obd2sourceplugin.so",
			"device" : "/dev/ttyUSB0",
			"baud" : "9600",
			"bluetoothAdapter" : ""
		}
	]

Configuration Key Definitions:

"name"
name of plugin.  This key is not used by the plugin at this moment.

"path"
path to plugin on the filesystem.

"device"
OBD-II Scantool device.  This is usually a serial device.  For bluetooth devices, use the bluetooth address for example:
"00:00:11:aa:bb:cc".

"baud"
baud rate for OBD-II scantool.  OBDLink *-style scantool usually default to 115200.  Refer to your scantool
docutmentation for the correct baud rate.  Note also that some bluetooth scantools, like the OBDLink MX have automatic baud selection and will ignore this value.

"bluetoothAdapter"
Hardware address of the bluetooth adapter to use.  If value is blank (ie "bluetoothAdapter" : ""), the system default
adapter will be used.  The bluetoothAdapter key is only used if the "device" key is also set to a bluetooth device address.


Performance

Performance will vary with the scantool and the vehicle.  Here are the current performance numbers per tested scantool
using the ECUSim2000 from scantool.net:

scantool - properties/second = (pids/s) - baud

OBDLink MX - 120 pids/s - N/A (varies with bluetooth radio)
OBDLink S - 100 pids/s - 115200
OBDPro USB - 43 pids/s - 9600



