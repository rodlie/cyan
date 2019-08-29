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

#ifdef USE_FC
#include <fontconfig/fontconfig.h>
#endif

int main(int argc, char *argv[])
{
#ifdef USE_FC
    qputenv("PANGOCAIRO_BACKEND", "fc");
#endif

    QApplication a(argc, argv);
    QApplication::setApplicationName(QString("Cyan"));
    QApplication::setOrganizationName(QString("FxArena"));
    QApplication::setOrganizationDomain(QString("fxarena.net"));
    QApplication::setApplicationVersion(QString(CYAN_VERSION));

#ifdef USE_FC
    QSplashScreen splash(QIcon(":/splash.png").pixmap(500,333), Qt::SplashScreen);
    splash.setStyleSheet("font-weight:bold;");
    splash.show();
#endif

    // setup imagemagick
    Magick::InitializeMagick(nullptr);

    // setup ffmpeg
#ifdef WITH_FFMPEG
    av_register_all();
    avdevice_register_all();
    avcodec_register_all();
    avformat_network_init();
#ifdef QT_NO_DEBUG
    av_log_set_level(AV_LOG_QUIET);
#endif
#endif

#ifdef USE_FC
    // setup fontconfig
    splash.showMessage(QObject::tr("Scanning font cache, this might take a while ..."), Qt::AlignBottom|Qt::AlignLeft, Qt::white);
    FcBool success = FcInit();
    if (success) {
        FcConfig *config = FcInitLoadConfigAndFonts();
        FcConfigDestroy(config);
    }
#endif

    Editor w;
    w.show();

#ifdef USE_FC
    splash.finish(&w);
#endif

    return a.exec();
}
