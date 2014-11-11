/**
 \mainpage Automotive Message Broker Library Documentation

 \section intro Introduction
 AMB Library documentation outlines the internal classes and structures for building
 plugins for AMB.

 \section architecture General Architecture
 AMB has 3 main parts.  Source plugins which provide data, a routing engine that
 routes data and sink plugins that consume the data.

 \section plugins Plugins
 There are two types of plugins: plugins that provide data, called "sources"
 (AbstractSource) and plugins that consume data, called "sinks" (AbstractSink).
 A typical source would get data from the vehicle and then translate the raw data
 into AMB property types.  Sinks then subscribe to the property types and do useful
 things with the data.

 Example plugins can be found in plugins/exampleplugin.{h,cpp} for an example
 source plugin and plugins/examplesink.{h,cpp} for an example sink plugin.  There
 are also many different types of plugins useful for testing and development in the
 plugins/ directory.

 \section routing_engine Routing Engine Plugins
 As of 0.12, the routing engine itself can be exchanged for a plugin.  This allows
 users to swap in routing engines with different behaviors, additional security,
 and custom throttling and filtering features.

 The easiest way to get started creating a routing engine plugin would be to look at
 AbstractRoutingEngine, the base class for all routing engines and the default 
 routing engine in ambd/core.cpp.
**/
