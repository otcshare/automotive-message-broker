Release notes for release 0.13

New features:
- [xwalk extension] implements full W3C automotive BG vehicle and data APIs
- [OpenCV plugin] code updated to the new 3.0 beta API
- [OpenCV plugin] driver drowsiness detection test code
- [Bluemonkey plugin] support for zones in JS API.
- [Bluemonkey plugin] database module.
- [tools] new ambctl. replaces amb-get, amb-set, etc

Changes:
- OpenCV plugin experimental options for cuda and kinect are removed
- Removed unmaintained plugins: tpms, websocket{sink,source}
- Implementing AbstractSource::getPropertyInfo() is now mandatory
- [websocket plugin] protocol changes: getSupportedEventTypes is getSupported.  'data' changed type to array of objects
  instead of array of strings.  See plugins/websocket/protocol
- Removed old ButtonEvent interface as it conflicted with the new ButtonEvent.
- Extensive documentation improvements.  Plugin docs install to /usr/share/doc/packages/automotive-message-broker/plugins/.
- Bluemonkey JS API docs /usr/share/doc/packages/automotive-message-broker/plugins/bluemonkey/html/.

Fixes:
- [DBus plugin] Fix exporting properties in zones that really don't have support
- [xwalk extension] fixed set() for some types did not work because of invalid GVariant creation.
- [websocket plugin] Updated html test page.  Fixed protocol usage.
- [websocket plugin] Source zones properly supported
