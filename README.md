DISCONTINUATION OF PROJECT

This project will no longer be maintained by Intel.

Intel has ceased development and contributions including, but not limited to, maintenance, bug fixes, new releases, or updates, to this project.  

Intel no longer accepts patches to this project.

If you have an ongoing need to use this project, are interested in independently developing it, or would like to maintain patches for the open source software community, please create your own fork of this project.  
# Automotive Message Broker Daemon {#ambd}

Version 0.15

## Introduction

Automotive Message Broker is a vehicle network abstraction system.  It brokers information from the vehicle
to applications.  It provides application with a rich API for accessing vehicle data.

Automotive Message Broker is built using CMake and requires libltdl (libtool), libjson-c, and boost packages.

## Git
About the Git Tree:
'master' is expected to be unstable and may not even compile.  If you want something more stable, checkout one of the
release branches (ie, 0.9.0, 0.10, etc)

## Building

To build:

~~~~~~~~~~~~~{.bash}
cd automotive-message-broker
mkdir build
cd build
cmake ..
make
~~~~~~~~~~~~~

## Installing

To install:

~~~~~~~~~~~~~{.bash}
sudo make install
~~~~~~~~~~~~~

## Running
To run:

~~~~~~~~~~~~~{.bash}
ambd
~~~~~~~~~~~~~

*also see **ambd -h** for additional command line options*

## Configuration

For information on ambd's configuration, please see ambd-configuration.idl.

## Running with the Qt mainloop:

Some source and sink plugins may want to use the Qt-based mainloop to take advantage of Qt features. To enable
the Qt mainloop, run cmake with -Duse_qtcore=On:

~~~~~~~~~~~~~{.bash}
cmake .. -Dqtmainloop=On
~~~~~~~~~~~~~

You will also need to edit your config to enable the Qt-based mainloop:

~~~~~~~~~~~~~{.json}
{
	"mainloop" : "/PLUGIN_INSTALL_PATH/qtmainloopplugin.so",
	"plugins" : "/etc/ambd/plugins.d"
}
~~~~~~~~~~~~~

NOTE: by default the glib mainloop will be used.

## Questions/Issues/Comments:

Questions or Comments can be emailed to the amb mailing list:
**amb at lists.01.org**

Issues and Feature requests can be submitted on our github page:
https://github.com/otcshare/automotive-message-broker/issues
