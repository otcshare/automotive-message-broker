# AMB Release Notes {#release_notes}
Version: 0.14

## New features:
- [DBus] some classes for exporting custom interfaces moved to plugins-common
- [Bluemonkey plugin] support for creating custom dbus interfaces: bluemonkey.exportInterface()
- [ambctl] support for enabling disabling plugins
- [ambd] support plugins.d - now plugin segments can be stuck in there and amb will try to load all plugins that
  are enabled.  See the example segments: /etc/ambd/plugins.d/dbus, examplesink, examplesource
- [xwalk] vehicle extension now dynamically exports all AMB supported objects

## Changes:
- Plugin create() method signature changed.
- Removed AbstractSinkManager class
- Removed deprecated dbus interfaces
- Removed deprecated 'FooChanged' signal introspection description
- Deprecated 'GetFoo' method.  This doesn't contain much more information that cannot already be
  obtained through the Properties interface.

## Fixes:
- [XWalk] Fixed misspelled 'diagnostics' interface
- [DBus] Fixed misspelled 'Diagnostics' interface
