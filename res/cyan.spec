#
# Cyan <http://cyan.fxarena.net> <https://github.com/olear/cyan>,
# Copyright (C) 2016, 2017 Ole-André Rodlie
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
#

Summary: Prepress Toolkit
Name: Cyan

Version: 1.0.0
Release: 1.el7.centos7.fedora19.qt4
License: GPLv2

Group: System Environment/Base
Packager: Ole-André Rodlie, <olear@dracolinux.org>
URL: https://github.com/olear/cyan

Source: %{name}-%{version}.tar.gz
Source1: ImageMagick-6.9.0-0.tar.xz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

BuildRequires: zlib-devel lcms2-devel libpng-devel libtiff-devel libjpeg-turbo-devel gcc-c++ qt-devel bzip2-devel

%description
Open-source cross-platform prepress toolkit.

%prep
%setup
%setup -T -D -a 1

%build
export CYAN_TMP=$(pwd)/tmp
export PKG_CONFIG_PATH=$CYAN_TMP/lib/pkgconfig
export LD_LIBRARY_PATH=$CYAN_TMP/lib:$LD_LIBRARY_PATH
export PATH=$CYAN_TMP/bin:$PATH

cd ImageMagick-6.9.0-0
CFLAGS="-fPIC -march=core2 -mtune=corei7-avx" CXXFLAGS="-fPIC" ./configure \
--prefix=$CYAN_TMP \
--disable-docs \
--disable-deprecated \
--with-magick-plus-plus=yes \
--with-quantum-depth=32 \
--without-dps \
--without-djvu \
--without-fftw \
--without-fpx \
--without-gslib \
--without-gvc \
--without-jbig \
--with-jpeg \
--with-lcms2 \
--without-openjp2 \
--without-lqr \
--without-lzma \
--without-openexr \
--without-pango \
--with-png \
--without-rsvg \
--with-tiff \
--without-webp \
--without-xml \
--with-zlib \
--with-bzlib \
--enable-static \
--disable-shared \
--enable-hdri \
--without-freetype \
--without-fontconfig \
--without-x \
--without-modules
make %{?_smp_mflags} install
cp LICENSE LICENSE.ImageMagick
cd ..

qmake-qt4 PREFIX=/usr CONFIG+=release
make

%install
make INSTALL_ROOT=%{buildroot} install

%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root,-)
/usr/bin/Cyan
/usr/share/applications/cyan.desktop
/usr/share/pixmaps/cyan.png
%doc ImageMagick-6.9.0-0/LICENSE.ImageMagick COPYING README.md

%changelog
