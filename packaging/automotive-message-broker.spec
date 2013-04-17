Name:       automotive-message-broker
Summary:    Automotive Message Broker is a vehicle network abstraction system.
Version:    0.8.0
Release:    1
Group:      System Environment/Daemons
License:    LGPL v2.1
URL:        https://github.com/otcshare/automotive-message-broker
Source0:    %{name}-%{version}.tar.bz2
Source100: ambd
Requires: automotive-message-broker-plugins
Requires: automotive-message-broker-plugins-websocket
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires:	json-glib
BuildRequires:  cmake
BuildRequires:  boost-devel
BuildRequires:  json-glib-devel
BuildRequires:  libtool-ltdl-devel
BuildRequires:  libuuid-devel
BuildRequires:  libwebsockets-devel
BuildRequires:  libuuid-devel
BuildRequires:  sqlite-devel
BuildRequires:  opencv-devel

%description
Automotive Message Broker is a vehicle network abstraction system.
It brokers information from the vehicle to applications.

%package devel
Summary:    automotive-message-broker development files
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Development files for the automotive-message-broker

%package doc
Summary:    Documentation for the automotive-message-broker API
Group:      Documentation
Requires:   %{name} = %{version}-%{release}

%description doc
Document files that describe the D-Bus API exposed by automotive-message-broker

%package plugins
Summary:    Various plugins for automotive-message-broker
Group:      System Environment/Daemons
Requires:   %{name} = %{version}-%{release}

%description plugins
Collection of plugins for automotive-message-broker.  Contains example, demo and dbus plugins.

%package plugins-obd2
Summary:    OBD-II plugin
Group:      System Environment/Daemons
Requires:   %{name} = %{version}-%{release}

%description plugins-obd2
OBD-II plugin that uses ELM 327-compatible scantools to access vehicle data

%package plugins-websocket
Summary:    websocket source and sink plugins
Group:      System Environment/Daemons
Requires:   %{name} = %{version}-%{release}
Requires:   libwebsockets

%description plugins-websocket
websocket source and sink plugins

%package plugins-wheel
Summary:    source plugin for using the Logitech G27 racing wheel                                        
Group:      System Environment/Daemons
Requires:   %{name} = %{version}-%{release}
Requires:   libwebsockets

%description plugins-wheel
source plugin for using the Logitech G27 racing wheel

%package plugins-database
Summary:    Database logging plugin for automotive-message-broker
Group:      System Environment/Daemons  
Requires:   %{name} = %{version}-%{release}
Requires:  sqlite

%description plugins-database
Database logging plugin for automotive-message-broker

%package plugins-opencvlux
Summary:    Plugin for simulating ExteriorBrightness using a common webcam
Group:      System Environment/Daemons
Requires:   %{name} = %{version}-%{release}
Requires:   opencv

%description plugins-opencvlux
Plugin for simulating ExteriorBrightness using a common webcam

%prep
%setup -q -n %{name}-%{version}

%build
%cmake -Ddatabase_plugin=ON -Dopencvlux_plugin=ON

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}%{_libdir}/systemd/system/network.target.wants
ln -s ../ambd.service %{buildroot}%{_libdir}/systemd/system/network.target.wants/ambd.service

#hack because systemd isn't in the image:

mkdir -p %{buildroot}/etc/rc.d/init.d
install -m 755 %{SOURCE100}  %{buildroot}/etc/rc.d/init.d/ambd
mkdir -p %{buildroot}/etc/rc.d/rc3.d
ln -s ../init.d/ambd %{buildroot}/etc/rc.d/rc3.d/S62ambd
mkdir -p %{buildroot}/etc/rc.d/rc5.d
ln -s ../init.d/ambd %{buildroot}/etc/rc.d/rc5.d/S62ambd

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%post plugins -p /sbin/ldconfig
%postun plugins -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%config %{_sysconfdir}/ambd/config
%{_bindir}/*
%{_sysconfdir}/rc.d/init.d/ambd
%{_sysconfdir}/rc.d/rc3.d/S62ambd
%{_sysconfdir}/rc.d/rc5.d/S62ambd
%{_libdir}/libamb.so*
%{_libdir}/systemd/system/*

%files devel
%defattr(-,root,root,-)
%{_libdir}/libamb.so*
%{_includedir}/amb/*.h
%{_libdir}/pkgconfig/*.pc

%files plugins
%defattr(-,root,root,-)
%{_libdir}/%{name}/examplesourceplugin.so
%{_libdir}/%{name}/examplesinkplugin.so
%{_libdir}/%{name}/dbussinkplugin.so
%{_libdir}/%{name}/demosinkplugin.so
/etc/dbus-1/system.d/amb.conf

%files plugins-wheel
%defattr(-,root,root,-)
%{_libdir}/%{name}/wheelsourceplugin.so

%files plugins-websocket
%defattr(-,root,root,-)
%{_libdir}/%{name}/websocketsourceplugin.so
%{_libdir}/%{name}/websocketsinkplugin.so

%files plugins-obd2
%defattr(-,root,root,-)
%{_libdir}/%{name}/obd2sourceplugin.so

%files plugins-database
%defattr(-,root,root,-)
%{_libdir}/%{name}/databasesinkplugin.so

%files plugins-opencvlux
%defattr(-,root,root,-)
%{_libdir}/%{name}/opencvluxplugin.so

%files doc
%defattr(-,root,root,-)
%doc %{_docdir}/%{name}/*.txt
