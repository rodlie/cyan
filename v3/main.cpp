/*
#
# Cyan Graphics - https://cyan.graphics - https://github.com/rodlie/cyan
#
# Copyright (c) Ole-André Rodlie <ole.andre.rodlie@gmail.com>. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#include "engine.h"
#include "cyan.h"
#include <Magick++.h>

#include <QApplication>
#include <iostream>

#ifdef BUNDLE_FONT
#include <QFont>
#include <QFontDatabase>
#endif

using namespace Cyan;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGuiApplication::setApplicationDisplayName( QString("Cyan") );
    QCoreApplication::setApplicationName( QString("Cyan") );
    QCoreApplication::setOrganizationName( QString("Cyan") );
    QCoreApplication::setOrganizationDomain( QString("cyan.graphics") );
    QCoreApplication::setApplicationVersion( QString(CYAN_VERSION) );

    Magick::InitializeMagick(nullptr);
#ifdef DISABLE_DISK_RESOURCE
    Engine::setDiskResource(0);
#endif
    Engine::setMemoryResource( Engine::getTotalRam(75) ); // TODO: setting

    Window window;

#ifdef BUNDLE_FONT
    int fontId = QFontDatabase::addApplicationFont( QString(":/fonts/DejaVuSans.ttf") );
    if (fontId != -1) {
        QFont font( QString("DejaVuSans") );
        if (font.pointSize() == 12) { font.setPointSize(10); }
        a.setFont(font);
    }
#endif

    window.show();
    return a.exec();
}
