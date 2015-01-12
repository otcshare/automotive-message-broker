Database plugin
Version: 0.13

This plugin logs properties in a sqlite database file.  It can also be used as a source to
play back previous logs to AMB.

To enable the Database plugin, run cmake and enable the database_plugin option:

cmake -Ddatabase_plugin=On ..

To use the Database plugin, add the following to the "sources" array in /etc/ambd/config:

{
  "name" : "Database",
  "path" : "/usr/lib/x86_64-linux-gnu/automotive-message-broker/databasesinkplugin.so",
  "databaseFile" : "storage",
  "bufferLength" : "100",
  "properties" : "{ 'properties' : ['VehicleSpeed','EngineSpeed'] }",
  "startOnLoad" : "false",
  "playbackOnLoad" : "false",
  "playbackMultiplier" : "1",
  "frequency" : "1"
}

Configuration Key Definitions:

"name"
name of plugin.  This key is not used by the plugin at this moment.

"path"
path to plugin on the filesystem.

"databaseFile"
path to log file

Default: "storage"

"bufferLength"
Number of logged items to keep in memory before flushing to the database file.
A higher number will reduce writes and improve performance.

Default: 100

"properties"
List of properties to log.

"startOnLoad"
Start logging when AMB starts.

Default: false

"playbackOnLoad"
Play back the specified log (see "databaseFile") when AMB starts.

Default: false

"playbackMultiplier"
Speed at which to play back the specified log.
1 = realtime
2 = 2x speed
etc.

Default: 1

"frequency"
Frequency in Hz in which the database will write contents.  Only the newest values
will be written.  Other values are discarded.  If bufferLength is not full,the
database will not be written to until it is.

Default: 1

AMB Properties:

DatabaseFile
Type: string
File to be used to log or play back.

corresponds to configuration option "databaseFile".

DatabaseLogging
Type: bool
Access: read/write

Indicates whether logging to the database is occuring.  Set to true to turn on
logging.  Set to false to stop logging.  This property is mutually exclusive with
the DatabasePlayback property.  Setting this to true will turn off playback.

Default: false when configuration option "startOnLoad" is not set.

DatabasePlayback
Type: bool
Access: read/write

Indicates whether database playback is occuring.  Set to true to turn on playback.
Set to false to turn off playback. This property is mutually exclusive with the
DatabaseLogging property.  Setting this to true will turn off logging.

Default: false when configuration option "playbackOnLoad" is not set.
