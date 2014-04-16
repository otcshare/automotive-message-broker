Name:       automotive-message-broker
Summary:    Automotive Message Broker is a vehicle network abstraction system
Version:    0.11.809
Release:    1
Group:      Automotive/Service
License:    LGPL-2.1
URL:        https://github.com/otcshare/automotive-message-broker
Source0:    %{name}-%{version}.tar.bz2
Requires: automotive-message-broker-plugins
Requires: automotive-message-broker-plugins-murphy
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires:	libjson
Requires:       libuuid
Requires:       default-ac-domains
BuildRequires:  cmake
BuildRequires:  boost-devel
BuildRequires:  libjson-devel
BuildRequires:  libtool-ltdl-devel
BuildRequires:  libuuid-devel
BuildRequires:  libwebsockets-devel
BuildRequires:  libuuid-devel
BuildRequires:  sqlite-devel
BuildRequires:  glib2-devel
BuildRequires:  opencv-devel
BuildRequires:  murphy
BuildRequires:  murphy-glib-devel
BuildRequires:  murphy-glib
BuildRequires:  dbus-devel
BuildRequires:  qt5-qtcore-devel
BuildRequires:  qt5-qtconcurrent-devel

%description
Automotive Message Broker is a vehicle network abstraction system.
It brokers information from the vehicle to applications.

%package devel
Summary:    Automotive Message Broker development files
Group:      Automotive/API
Requires:   %{name} = %{version}-%{release}
Requires:   libuuid-devel

%description devel
Development files for the automotive-message-broker

%package doc
Summary:    Documentation for the automotive-message-broker API
Group:      Automotive/Documentation
Requires:   %{name} = %{version}-%{release}

%description doc
Document files that describe the D-Bus API exposed by automotive-message-broker

%package plugins
Summary:    Various plugins for automotive-message-broker
Group:      Automotive/Libraries
Requires:   %{name} = %{version}-%{release}

%description plugins
Collection of plugins for automotive-message-broker.  Contains example, demo and dbus plugins.

%package plugins-common
Summary:  Common plugin library
Group:    Automotive/Libraries
Requires: %{name} = %{version}-%{release}

%description plugins-common
library containing a kitchen-sink of common utility functions

%package plugins-qtmainloop
Summary:    qt5 mainloop plugin
Group:      Automotive/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   qt5-qtcore

%description plugins-qtmainloop
Qt mainloop plugin enables qt-based source and sink plugins to run using qt mainloop

%package plugins-obd2
Summary:    OBD-II plugin
Group:      Automotive/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   %{name}-plugins-common = %{version}-%{release}

%description plugins-obd2
OBD-II plugin that uses ELM 327-compatible scantools to access vehicle data

%package plugins-websocket
Summary:    Websocket source and sink plugins
Group:      Automotive/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   libwebsockets
Requires:   qt5-qtcore

%description plugins-websocket
websocket source and sink plugins

%package plugins-wheel
Summary:    Source plugin for using the Logitech G27 racing wheel                                        
Group:      Automotive/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   libwebsockets

%description plugins-wheel
source plugin for using the Logitech G27 racing wheel

%package plugins-database
Summary:    Database logging plugin for automotive-message-broker
Group:      Automotive/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:  sqlite

%description plugins-database
Database logging plugin for automotive-message-broker

%package plugins-opencvlux
Summary:    Plugin for simulating ExteriorBrightness using a common webcam
Group:      Automotive/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   opencv

%description plugins-opencvlux
Plugin for simulating ExteriorBrightness using a common webcam

%package plugins-murphy
Summary:   Plugin for integration with the murphy policy system
Group:     Automotive/Libraries
Requires:  %{name} = %{version}-%{release}
Requires:  murphy

%description plugins-murphy
Plugin for integration with the murphy policy system

%package plugins-gpsnmea
Summary:   Plugin that provides gps data from nmea devices
Group:     Automotive/Libraries
Requires:  %{name} = %{version}-%{release}
Requires: libboost_regex

%description plugins-gpsnmea
Plugin that provides location data from nmea devices including bluetooth

%package plugins-test
Summary:   Plugin that tests AMB code
Group:     Automotive/Libraries
Requires:  %{name} = %{version}-%{release}

%description plugins-test
Plugin that tests some internal AMB code and plugin functionality.  This plugin will assert if something is wrong.

%package plugins-cangen
Summary:   Plugin that generates CAN data
Group:     Automotive/Libraries
Requires:  %{name} = %{version}-%{release}

%description plugins-cangen
CAN frames generator plug-in for the AMB CAN Simulator.

%package plugins-cansim
Summary:   CAN frames listener plug-in for the AMB CAN Simulator.
Group:     Automotive/Libraries
Requires:  %{name} = %{version}-%{release}

%description plugins-cansim
CAN frames listener plug-in for the AMB CAN Simulator.

%prep
%setup -q -n %{name}-%{version}

%build
%cmake -Dqtmainloop=ON -Ddatabase_plugin=ON -Dopencvlux_plugin=ON -Dmurphy_plugin=ON -Dwebsocket_plugin=ON -Dobd2_plugin=ON -Dtest_plugin=ON -Dgpsnmea_plugin=ON -Dcangen_plugin=ON -Dcansim_plugin=ON

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}%{_prefix}/lib/systemd/system/network.target.wants
cp packaging.in/ambd.service.systemd.tizen %{buildroot}%{_prefix}/lib/systemd/system/ambd.service
ln -s ../ambd.service %{buildroot}%{_prefix}/lib/systemd/system/network.target.wants/ambd.service
%install_service multi-user.target.wants ambd.service

cp packaging.in/config.tizen %{buildroot}/etc/ambd/


%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%post plugins -p /sbin/ldconfig
%postun plugins -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest
%config %{_sysconfdir}/ambd/config
%config %{_sysconfdir}/ambd/config.tizen
%config %{_sysconfdir}/ambd/examples/*
%{_bindir}/ambd
%{_libdir}/libamb.so*
%{_prefix}/lib/systemd/system/ambd.service
%{_prefix}/lib/systemd/system/network.target.wants/ambd.service
%{_prefix}/lib/systemd/system/multi-user.target.wants/ambd.service
%{_bindir}/amb-get
%{_bindir}/amb-get-history
%{_bindir}/amb-set
%{_bindir}/amb-listen

%files devel
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_includedir}/amb/*.h
%{_includedir}/amb/*.hpp
%{_libdir}/pkgconfig/*.pc

%files plugins
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/examplesourceplugin.so
%{_libdir}/%{name}/examplesinkplugin.so
%{_libdir}/%{name}/dbussinkplugin.so
%{_libdir}/%{name}/demosinkplugin.so
/etc/dbus-1/system.d/amb.conf

%files plugins-common
%manifest packaging.in/amb.manifest.plugins
%defattr(-,root,root,-)
%{_libdir}/libamb-plugins-common.so

%files plugins-qtmainloop
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/qtmainloopplugin.so

%files plugins-wheel
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/wheelsourceplugin.so

%files plugins-websocket
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/websocketsource.so
%{_libdir}/%{name}/websocketsink.so

%files plugins-obd2
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/obd2sourceplugin.so

%files plugins-database
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/databasesinkplugin.so

%files plugins-opencvlux
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/opencvluxplugin.so

%files plugins-murphy
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/murphysourceplugin.so

%files plugins-gpsnmea
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/gpsnmea.so

%files doc
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%doc %{_docdir}/%{name}/*.txt

%files plugins-test
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/testplugin.so

%files plugins-cangen
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/cangenplugin.so
%{_bindir}/gen-set

%files plugins-cansim
%defattr(-,root,root,-)
%manifest packaging.in/amb.manifest.plugins
%{_libdir}/%{name}/cansimplugin.so

