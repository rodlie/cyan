/*
# Copyright Ole-André Rodlie.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
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
    QApplication::setOrganizationDomain(QString("net.fxarena.cyan"));
    QApplication::setApplicationVersion(QString(CYAN_VERSION));

#ifdef USE_FC
    QSplashScreen splash(QIcon(":/splash.png").pixmap(500,333), Qt::WindowStaysOnTopHint);
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
    splash.showMessage(QObject::tr("Setup fontconfig, this might take a while ..."), Qt::AlignBottom|Qt::AlignLeft, Qt::white);
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
