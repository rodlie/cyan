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

#include "fontscan.h"

#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QDir>

#include <fontconfig/fontconfig.h>

FontScan::FontScan(QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    setWindowIcon(QIcon(":/icons/hicolor/128x128/apps/cyan.png"));
    setWindowTitle(tr("Loading Cyan ..."));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QLabel *iconLabel = new QLabel(this);
    QLabel *infoLabel = new QLabel(this);

    mainLayout->addWidget(iconLabel);
    mainLayout->addWidget(infoLabel);

    iconLabel->setPixmap(QIcon(":/icons/hicolor/128x128/apps/cyan.png")
                         .pixmap(128, 128));

    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setFont(QFont("Arial", 8));
    infoLabel->setStyleSheet("color:white; font-weight: bold;");

    infoLabel->setText(tr("Loading ..."));

    QTimer::singleShot(300, this, SLOT(scan()));
}

void FontScan::scan()
{
    FcBool success = FcInit();
    if (success) {
#ifdef Q_OS_WIN
        FcConfig *config = FcInitLoadConfig();
#else
        FcConfig *config = FcInitLoadConfigAndFonts(); //FcConfigCreate(); //FcInitLoadConfigAndFonts();
#endif
        if (config) {
#ifdef Q_OS_WIN
            FcConfigAppFontAddDir(config,
                                  (const FcChar8 *)QString("%1:\\Windows\\Fonts")
                                  .arg(QDir::root().path()).toUtf8().data());
#else
            FcConfigAppFontAddDir(config, (const FcChar8 *)"/usr/share/fonts");
#endif
            FcPattern *p = FcPatternCreate();
            FcObjectSet *os = FcObjectSetBuild(FC_FAMILY, NULL);
            FcFontSet *fs = FcFontList(config, p, os);
            int fonts = fs->nfont;
            FcFontSetDestroy(fs);
            FcObjectSetDestroy(os);
            FcPatternDestroy(p);
            qWarning() << "Total fonts found" << fonts;
        }
        FcConfigDestroy(config);
    }
    close();
}
