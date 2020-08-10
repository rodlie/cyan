/*
#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
*/

#include "editor.h"

#include <QApplication>
#include <QSplashScreen>
#include <QtMsgHandler>
#include <QFile>
#include <QStringList>
#include <QFile>

#include <fontconfig/fontconfig.h>

#include <stdio.h>
#include <stdlib.h>

void msgHandler(QtMsgType type,
                const QMessageLogContext &context,
                const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    if (localMsg.contains("link outline hasn't been detected!") ||
        localMsg.contains("iCCP: known incorrect sRGB profile") ||
        localMsg.contains("XDG_RUNTIME_DIR")) { return; }
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr,
                "Debug: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        break;
#if QT_VERSION >= 0x050500
    case QtInfoMsg:
        fprintf(stderr,
                "Info: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        break;
#endif
#ifdef SHOW_WARN
    case QtWarningMsg:
        fprintf(stderr,
                "Warning: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        break;
#endif
    case QtCriticalMsg:
        fprintf(stderr,
                "Critical: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr,
                "Fatal: %s (%s:%u, %s)\n",
                localMsg.constData(),
                context.file,
                context.line,
                context.function);
        abort();
    default:;
    }
}

int main(int argc, char *argv[])
{
    // always force fontconfig in pango
    qputenv("PANGOCAIRO_BACKEND", "fc");

    qInstallMessageHandler(msgHandler);

    QApplication a(argc, argv);
    QApplication::setApplicationName(QString("Cyan"));
    QApplication::setOrganizationName(QString("FxArena"));
    QApplication::setOrganizationDomain(QString("fxarena.net"));
    QApplication::setApplicationVersion(QString(CYAN_VERSION));

#ifdef Q_OS_MAC
    // setup fontconfig on mac
    QString fontconfig = QString("%1/../Resources/etc/fonts").arg(QApplication::applicationDirPath());
    if (QFile::exists(fontconfig)) { qputenv("FONTCONFIG_PATH", fontconfig.toUtf8()); }
#endif
#ifdef Q_OS_LINUX
    // setup fontconfig on linux
    QString fontconfig = QString("%1/../etc/fonts").arg(QApplication::applicationDirPath());
    if (QFile::exists(fontconfig)) { qputenv("FONTCONFIG_PATH", fontconfig.toUtf8()); }
#endif

#ifndef Q_OS_LINUX
    // splash
    QSplashScreen splash(QIcon(":/icons/splash.png").pixmap(512,512),
                         Qt::SplashScreen);
    splash.show();
#endif

    // setup imagemagick
    Magick::InitializeMagick(nullptr);

    // setup fontconfig
    FcBool success = FcInit();
    if (success) {
        FcConfig *config = FcInitLoadConfigAndFonts();
        FcConfigDestroy(config);
    }

    // editor
    Editor w;
    w.show();
    QStringList args = QApplication::arguments();
    if (args.size()>1 && QFile::exists(args.at(1))) {
            w.openConsoleImage(args.at(1));
    }

#ifndef Q_OS_LINUX
    splash.finish(&w);
#endif

    return a.exec();
}
