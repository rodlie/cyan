# RPM package example

Summary: Lumina Desktop Extras
Name: lumina-extra

Version: 1.0
Release: 1
License: BSD

Group: System Environment/Base
Packager: Ole-André Rodlie, <ole.andre.rodlie@gmail.com>
URL: https://github.com/rodlie/lumina-extra

Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: gcc-c++ qt5-qtbase-devel

%description
Additional applications to enchance the Lumina Desktop Enviroment.

%prep
%setup

%build
qmake-qt5 PREFIX=/usr CONFIG+=release
make

%install
make INSTALL_ROOT=%{buildroot} install

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root,-)
/usr/bin/lumina-screensaver-service
/usr/bin/lumina-disk-manager
/etc/xdg/autostart/lumina-screensaver-service.desktop
/etc/xdg/autostart/lumina-disk-manager.desktop
/usr/share/doc/*
%changelog
