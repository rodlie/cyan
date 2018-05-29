/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#include "viewer.h"
#include <QApplication>
#include <QtPlugin>
#include <Magick++.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName(QString("viewer"));
    QApplication::setOrganizationDomain(QString("org.lumina-desktop"));
    Magick::InitializeMagick(NULL);
    Viewer w;
    w.show();

    return a.exec();
}
