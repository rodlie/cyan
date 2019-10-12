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

#include "CyanAboutDialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QApplication>
#include <QTableWidget>
#include <QTextBrowser>
#include <QDebug>
#include <QPushButton>

#include <Magick++.h>

#ifdef WITH_FFMPEG
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#endif

#include "cyan_common.h"

CyanAboutDialog::CyanAboutDialog(QWidget *parent) :
    QDialog (parent)
{
    setWindowTitle(tr("About Cyan"));
    setAttribute(Qt::WA_DeleteOnClose);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    QWidget *headerWidget = new QWidget(this);
    headerWidget->setContentsMargins(0,0,0,0);
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0,0,0,0);

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setAlignment(Qt::AlignHCenter);
    logoLabel->setPixmap(QPixmap::fromImage(QImage(":/icons/hicolor/128x128/apps/Cyan.png")));
    headerLayout->addWidget(logoLabel);

    QLabel *textLabel = new QLabel(this);
    textLabel->setMinimumWidth(200);
    textLabel->setMaximumWidth(200);
    textLabel->setWordWrap(true);
    textLabel->setOpenExternalLinks(true);

    QString typeString = CYAN_VERSION_TYPE;
    QString gitValue = CYAN_VERSION_GIT;
    if (typeString.isEmpty()) { typeString = "Community"; }
    if (!gitValue.isEmpty()) {
        typeString.append(QString(" %1").arg(CYAN_VERSION_GIT));
    }

    QString textString = QString("<h2 align=\"center\">%1 %2<div style=\"font-size:small;text-transform: uppercase;\">%3</div></h2>")
            .arg(qApp->applicationName())
            .arg(CYAN_VERSION)
            .arg(typeString);
   textString.append(QString("<p style=\"text-align:justify;font-size:small;\">%7 &copy; 2018-2019 Ole-André Rodlie, FxArena DA. %6. %1%2%3%4%5</p>")
                 .arg(tr("This program is free software; "))
                 .arg(tr("you can redistribute it and/or modify it under the terms of "))
                 .arg(tr("the GNU Lesser General Public License "))
                 .arg(tr("as published by the Free Software Foundation; "))
                 .arg(tr("either version 2.1 of the License, or (at your option) any later version."))
                 .arg(tr("All rights reserved"))
                 .arg(tr("Copyright")));
    textLabel->setText(textString);

    headerLayout->addWidget(textLabel);
    headerLayout->addStretch();

    QWidget *infoWidget = new QWidget(this);
    infoWidget->setContentsMargins(0,0,0,0);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0,0,0,0);

    QTabWidget *tabs = new QTabWidget(this);
    tabs->setMinimumWidth(400);
    infoLayout->addWidget(tabs);

    QLabel *supportWidget = new QLabel(this);
    supportWidget->setText(QString("<p align=\"center\"><a href=\"https://liberapay.com/rodlie/donate\"><img src=\":/icons/liberapay.png\"></a>&nbsp;<a href=\"https://www.patreon.com/bePatron?u=23266568\"><img src=\":/icons/patron.png\"></a>&nbsp;<a href=\"https://www.paypal.me/orodlie\"><img src=\":/icons/paypal.png\"></a></p>"));
    supportWidget->setOpenExternalLinks(true);
    infoLayout->addWidget(supportWidget);

    QTextBrowser *supportBrowser = new QTextBrowser(this);
    supportBrowser->setAcceptRichText(true);
    supportBrowser->setReadOnly(true);
    supportBrowser->setOpenLinks(true);
    supportBrowser->setOpenExternalLinks(true);
    supportBrowser->setAcceptRichText(true);
    supportBrowser->setSource(QUrl::fromUserInput(":/html/ChangeLog.html"));

    QWidget *thirdpartyWidget = new QWidget(this);
    thirdpartyWidget->setContentsMargins(0,0,0,0);
    QVBoxLayout *thirdpartyLayout = new QVBoxLayout(thirdpartyWidget);
    thirdpartyLayout->setContentsMargins(0,0,0,0);
    QTextBrowser *thirdpartyBrowser = new QTextBrowser(this);
    thirdpartyBrowser->setAcceptRichText(true);
    thirdpartyBrowser->setReadOnly(true);
    thirdpartyBrowser->setOpenLinks(true);
    thirdpartyBrowser->setOpenExternalLinks(true);
    thirdpartyBrowser->setAcceptRichText(true);
    thirdpartyLayout->addWidget(thirdpartyBrowser);
    QPushButton *aboutQtButton = new QPushButton(this);
    aboutQtButton->setFocusPolicy(Qt::NoFocus);
    aboutQtButton->setText(tr("About Qt %1").arg(qVersion()));
    connect(aboutQtButton, SIGNAL(released()), qApp, SLOT(aboutQt()));
    thirdpartyLayout->addWidget(aboutQtButton);

    tabs->addTab(supportBrowser, tr("Documentation"));
    tabs->addTab(thirdpartyWidget, tr("Software"));

    size_t magickQ;
    size_t magickV;
    MagickCore::GetMagickQuantumDepth(&magickQ);
    QString magickInfo = QString(MagickCore::GetMagickVersion(&magickV)).split("Q").takeFirst();

    QString html;
    html.append(QString("<html><head><style>body { font-family: Arial, Verdana, Helvetica; font-size: 10pt; } h1, h2 { font-weight: normal; } ul li a { text-decoration: none; }</style></head><body>"));
    html.append(QString("<img src=\":/icons/magick_logo.png\"><br><h2>%1</h2>").arg(magickInfo));
    html.append(QString("<p><a href=\"https://imagemagick.org\">%2®</a> %3.</p><p>"
                         "%2 %4 <a href=\"%1\">%5</a>.</p>")
                 .arg(MagickCore::GetMagickLicense())
                 .arg(MagickCore::GetMagickPackageName())
                 .arg(tr("is used to read, create, save, edit, compose, and  convert bitmap images"))
                 .arg(tr("is distributed under the following"))
                 .arg(tr("license")));
    html.append(QString("<p>%1</p>").arg(MagickCore::GetMagickCopyright()));

    html.append(QString("<p><strong>%4</strong>:<br><br>Q%1 %2 %3</p>")
                 .arg(magickQ)
                 .arg(MagickCore::GetMagickFeatures())
                 .arg(MagickCore::GetMagickDelegates())
                 .arg(tr("Features")));
    html.append(QString("<p><strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::disk()))
                 .arg(tr("Disk Limit")));
    html.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::area(),false, true))
                 .arg(tr("Area Limit")));
    html.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::map()))
                 .arg(tr("Map Limit")));
    html.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::memory()))
                 .arg(tr("Memory Limit")));
    html.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::width(), true))
                 .arg(tr("Width Limit")));
    html.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::height(), true))
                 .arg(tr("Height Limit")));
    html.append(QString("<strong>%2</strong>: %1</p>")
                 .arg(Magick::ResourceLimits::thread())
                 .arg(tr("Thread Limit")));
    html.append(QString("<hr><br><img src=\":/icons/lcms_logo.png\"><br><h2>Little CMS %1</h2>")
                 .arg(QString::number(LCMS_VERSION)
                      .insert(1,".")
                      .remove(2,1)
                      .remove(3,1)));
    html.append(QString("<p><a href=\"http://www.littlecms.com/\">Little CMS</a> %1.</p><p>Little CMS %2 <a href=\"https://opensource.org/licenses/mit-license.php\">%3</a>.</p>")
                 .arg(tr("is an small-footprint color management engine, with special focus on accuracy and performance"))
                 .arg(tr("is distributed under the following"))
                 .arg(tr("license")));
    html.append(QString("<p>%2 &copy; 2018 Marti Maria Saguer.<br>%1.</p>")
                 .arg(tr("All rights reserved"))
                 .arg(tr("Copyright")));
#ifdef WITH_FFMPEG
    html.append(QString("<hr><br><img src=\":/icons/ffmpeg_logo.png\"><br><h2>FFmpeg</h2>"));
    html.append(QString("<p><a href=\"https://ffmpeg.org\">FFmpeg</a> %1</p>")
                 .arg(tr("is the leading multimedia framework, able to decode pretty much anything that humans and machines have created.")));
    html.append(QString("<p><strong>libavformat:</strong> %1.%2.%3<br>")
                 .arg((avformat_version() >> 16))
                 .arg((avformat_version() >> 8 & 0xff))
                 .arg((avformat_version() & 0xff)));
    html.append(QString("<strong>libavdevice:</strong> %1.%2.%3<br>")
                 .arg((avdevice_version() >> 16))
                 .arg((avdevice_version() >> 8 & 0xff))
                 .arg((avdevice_version() & 0xff)));
    html.append(QString("<strong>libavcodec:</strong> %1.%2.%3<br>")
                 .arg((avcodec_version() >> 16))
                 .arg((avcodec_version() >> 8 & 0xff))
                 .arg((avcodec_version() & 0xff)));
    html.append(QString("<strong>libavutil:</strong> %1.%2.%3<br>")
                 .arg((avutil_version() >> 16))
                 .arg((avutil_version() >> 8 & 0xff))
                 .arg((avutil_version() & 0xff)));
    html.append(QString("<strong>libswscale:</strong> %1.%2.%3</p>")
                 .arg((swscale_version() >> 16))
                 .arg((swscale_version() >> 8 & 0xff))
                 .arg((swscale_version() & 0xff)));
    html.append(QString("<p>%1</p>").arg(tr("FFmpeg is a trademark of Fabrice Bellard, originator of the FFmpeg project.")));
#endif
    html.append(QString("</body></html>"));
    thirdpartyBrowser->setHtml(html);

    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(infoWidget);
}
