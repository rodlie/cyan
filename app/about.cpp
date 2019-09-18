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
#include <QMessageBox>
#include <QApplication>

#ifdef WITH_FFMPEG
extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#endif

void Editor::aboutCyan()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("About Cyan"));

    box.setIconPixmap(QPixmap::fromImage(QImage(":/icons/hicolor/128x128/apps/Cyan.png")));

    QString about, more;
    about.append(QString("<h2>%1 %2 %3</h2>")
                 .arg(qApp->applicationName())
                 .arg(CYAN_VERSION)
                 .arg(CYAN_VERSION_TYPE));
    about.append(QString("<h4>%1</h4>")
                 .arg(tr("Simple general-purpose image editor")));
    about.append(QString("%2 &copy; 2018-2019 Ole-André Rodlie, FxArena DA.<br>%1.")
                 .arg(tr("All rights reserved"))
                 .arg(tr("Copyright")));
    about.append(QString("<p>%1</p>")
                 .arg(tr("This program is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.")));
    more.append(QString("<p><a href=\"https://liberapay.com/rodlie/donate\"><img src=\":/icons/liberapay.png\"></a>&nbsp;<a href=\"https://www.patreon.com/bePatron?u=23266568\"><img src=\":/icons/patron.png\"></a></p>"));

    box.setText(about);
    box.setInformativeText(more);
    box.setStyleSheet("QLabel { min-width: 450px; }");
    box.exec();
}

void Editor::aboutImageMagick()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("About ImageMagick"));

    try {
        Magick::Image logo;
        logo.read("logo:");
        logo.scale(Magick::Geometry(256, 256));
        logo.magick("BMP");
        Magick::Blob pix;
        logo.write(&pix);
        box.setIconPixmap(QPixmap::fromImage(QImage::fromData(reinterpret_cast<uchar*>(const_cast<void*>(pix.data())),
                                                              static_cast<int>(pix.length()))));
    }
    catch(Magick::Error &error_ ) { emit errorMessage(error_.what()); }
    catch(Magick::Warning &warn_ ) { emit warningMessage(warn_.what()); }

    size_t magickQ;
    size_t magickV;
    MagickCore::GetMagickQuantumDepth(&magickQ);
    QString magickInfo = QString(MagickCore::GetMagickVersion(&magickV)).split("Q").takeFirst();

    QString about;
    about.append(QString("<h2>%1</h2>").arg(magickInfo));
    about.append(QString("<p><a href=\"https://imagemagick.org\">%2®</a> %3.</p><p>"
                         "%2 %4 <a href=\"%1\">%5</a>.</p>")
                 .arg(MagickCore::GetMagickLicense())
                 .arg(MagickCore::GetMagickPackageName())
                 .arg(tr("is used to read, create, save, edit, compose, and  convert bitmap images"))
                 .arg(tr("is distributed under the following"))
                 .arg(tr("license")));
    about.append(QString("<p>%1</p>").arg(MagickCore::GetMagickCopyright()));

    about.append(QString("<p><strong>%4</strong>:<br><br>Q%1 %2 %3</p>")
                 .arg(magickQ)
                 .arg(MagickCore::GetMagickFeatures())
                 .arg(MagickCore::GetMagickDelegates())
                 .arg(tr("Features")));
    about.append(QString("<p><strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::disk()))
                 .arg(tr("Disk Limit")));
    about.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::area(),false, true))
                 .arg(tr("Area Limit")));
    about.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::map()))
                 .arg(tr("Map Limit")));
    about.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::memory()))
                 .arg(tr("Memory Limit")));
    about.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::width(), true))
                 .arg(tr("Width Limit")));
    about.append(QString("<strong>%2</strong>: %1<br>")
                 .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::height(), true))
                 .arg(tr("Height Limit")));
    about.append(QString("<strong>%2</strong>: %1</p>")
                 .arg(Magick::ResourceLimits::thread())
                 .arg(tr("Thread Limit")));

    box.setText(about);
    box.setStyleSheet("QLabel { min-width: 350px; }");
    box.exec();
}

void Editor::aboutLcms()
{
    QMessageBox box(this);
    box.setWindowTitle(QString("%1 Little CMS").arg(tr("About")));
    box.setIconPixmap(QPixmap::fromImage(QImage(":/icons/lcms_logo.png")));

    QString about;
    about.append(QString("<h3>Little CMS %1</h3>")
                 .arg(QString::number(LCMS_VERSION)
                      .insert(1,".")
                      .remove(2,1)
                      .remove(3,1)));
    about.append(QString("<p><a href=\"http://www.littlecms.com/\">Little CMS</a> %1.</p><p>Little CMS %2 <a href=\"https://opensource.org/licenses/mit-license.php\">%3</a>.</p>")
                 .arg(tr("is an small-footprint color management engine, with special focus on accuracy and performance"))
                 .arg(tr("is distributed under the following"))
                 .arg(tr("license")));
    about.append(QString("<p>%2 &copy; 2018 Marti Maria Saguer.<br>%1.</p>")
                 .arg(tr("All rights reserved"))
                 .arg(tr("Copyright")));

    box.setText(about);
    box.setStyleSheet("QLabel { min-width: 350px; }");
    box.exec();
}

#ifdef WITH_FFMPEG
void Editor::aboutFFmpeg()
{
    QMessageBox box(this);
    box.setWindowTitle(QString("%1 FFmpeg").arg(tr("About")));
    box.setIconPixmap(QPixmap::fromImage(QImage(":/icons/ffmpeg_logo.png")));

    QString about;
    about.append(QString("<h3>FFmpeg</h3>"));
    about.append(QString("<p><a href=\"https://ffmpeg.org\">FFmpeg</a> %1</p>")
                 .arg(tr("is the leading multimedia framework, able to decode pretty much anything that humans and machines have created.")));
    about.append(QString("<p><strong>libavformat:</strong> %1.%2.%3<br>")
                 .arg((avformat_version() >> 16))
                 .arg((avformat_version() >> 8 & 0xff))
                 .arg((avformat_version() & 0xff)));
    about.append(QString("<strong>libavdevice:</strong> %1.%2.%3<br>")
                 .arg((avdevice_version() >> 16))
                 .arg((avdevice_version() >> 8 & 0xff))
                 .arg((avdevice_version() & 0xff)));
    about.append(QString("<strong>libavcodec:</strong> %1.%2.%3<br>")
                 .arg((avcodec_version() >> 16))
                 .arg((avcodec_version() >> 8 & 0xff))
                 .arg((avcodec_version() & 0xff)));
    about.append(QString("<strong>libavutil:</strong> %1.%2.%3<br>")
                 .arg((avutil_version() >> 16))
                 .arg((avutil_version() >> 8 & 0xff))
                 .arg((avutil_version() & 0xff)));
    about.append(QString("<strong>libswscale:</strong> %1.%2.%3</p>")
                 .arg((swscale_version() >> 16))
                 .arg((swscale_version() >> 8 & 0xff))
                 .arg((swscale_version() & 0xff)));
    about.append(QString("<p>%1</p>").arg(tr("FFmpeg is a trademark of Fabrice Bellard, originator of the FFmpeg project.")));
    box.setText(about);
    box.setStyleSheet("QLabel { min-width: 350px; }");
    box.exec();
}
#endif
