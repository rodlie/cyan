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

#include <Magick++.h>

#include "CyanCommon.h"

CyanAboutDialog::CyanAboutDialog(QWidget *parent) :
    QDialog (parent)
{
    setWindowTitle(tr("About Cyan"));
    setAttribute(Qt::WA_DeleteOnClose);
    setMaximumSize(770, 420);
    setMinimumSize(770, 420);

    bool hasHdri= QString(MagickCore::GetMagickFeatures()).contains("HDRI");
    size_t magickQ;
    size_t magickV;
    MagickCore::GetMagickQuantumDepth(&magickQ);
    QString magickInfo = QString(MagickCore::GetMagickVersion(&magickV)).split("Q").takeFirst();
    QString magickDepth = QString("Q%1").arg(magickQ);
    if (hasHdri) {
        magickDepth.append("HDRI");
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *headerWidget = new QWidget(this);
    headerWidget->setContentsMargins(0,0,0,0);
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0,0,0,0);

    QWidget *headerContainer = new QWidget(this);
    headerContainer->setContentsMargins(0,0,0,0);
    QHBoxLayout *headerContainerLayout = new QHBoxLayout(headerContainer);
    headerContainerLayout->setContentsMargins(0,0,0,0);
    headerContainerLayout->addWidget(headerWidget);

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setAlignment(Qt::AlignHCenter);
    logoLabel->setPixmap(QPixmap::fromImage(QImage(":/icons/hicolor/128x128/apps/Cyan.png")));
    headerLayout->addWidget(logoLabel);

    QWidget *magickWidget = new QWidget(this);
    magickWidget->setContentsMargins(0,0,0,0);
    magickWidget->setMaximumWidth(300);
    QVBoxLayout *magickLayout = new QVBoxLayout(magickWidget);
    magickLayout->setContentsMargins(0,0,0,0);
    QLabel *magickLabel = new QLabel(this);
    magickLabel->setWordWrap(true);
    magickLabel->setOpenExternalLinks(true);
    magickLabel->setText(QString("<p style=\"text-align:left;font-size:small;\"><table>"
                                 "<tr><td><b>%1</b></td><td>&nbsp;:&nbsp;</td><td<td>%2</td></tr>"
                                 "<tr><td><b>%3</b></td><td>&nbsp;:&nbsp;</td><td>%4</td></tr>"
                                 "<tr><td><b>%5</b></td><td>&nbsp;:&nbsp;</td><td>%6</td></tr>"
                                 "<tr><td><b>%7</b></td><td>&nbsp;:&nbsp;</td><td>%8</td></tr>"
                                 "<tr><td><b>%9</b></td><td>&nbsp;:&nbsp;</td><td<td>%10</td></tr>"
                                 "<tr><td><b>%11</b></td><td>&nbsp;:&nbsp;</td><td>%12</td></tr>"
                                 "<tr><td><b>%13</b></td><td>&nbsp;:&nbsp;</td><td>%14</td></tr>"
                                 "<tr><td><b>%15</b></td><td>&nbsp;:&nbsp;</td><td>%16</td></tr>"
                                 "</table></p>"
                                 "<p style=\"text-align:center;font-size:small;\">"
                                 "%17 <a href=\"%18\">%19</a> %20<br>%21 %22"
                                 "<br><br>"
                                 "%17 <a href=\"http://www.littlecms.com/\">Little CMS %23</a><br>%21 &copy; 2020 Marti Maria Saguer.<br>%24."
                                 "</p>")
                         .arg(tr("Area Limit"))
                         .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::area(),false, true))
                         .arg(tr("Map Limit"))
                         .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::map()))
                         .arg(tr("Memory Limit"))
                         .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::memory()))
                         .arg(tr("Width Limit"))
                         .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::width(), true))
                         .arg(tr("Height Limit"))
                         .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::height(), true))
                         .arg(tr("Thread Limit"))
                         .arg(Magick::ResourceLimits::thread())
                         .arg(tr("Features"))
                         .arg(MagickCore::GetMagickFeatures())
                         .arg(tr("Delegates"))
                         .arg(MagickCore::GetMagickDelegates())
                         .arg(tr("Powered by"))
                         .arg(MagickCore::GetMagickLicense())
                         .arg(magickInfo)
                         .arg(magickDepth)
                         .arg(tr("Copyright"))
                         .arg(MagickCore::GetMagickCopyright())
                         .arg(QString::number(LCMS_VERSION)
                                                   .insert(1,".")
                                                   .remove(2,1)
                                                   .remove(3,1))
                         .arg(tr("All rights reserved"))
                         );
    magickLayout->addWidget(magickLabel);
    magickLayout->addStretch();
    headerContainerLayout->addSpacing(20);
    headerContainerLayout->addWidget(magickWidget);

    QLabel *textLabel = new QLabel(this);
    textLabel->setWordWrap(true);
    textLabel->setOpenExternalLinks(true);

    QLabel *footerLabel = new QLabel(this);
    footerLabel->setWordWrap(true);
    footerLabel->setOpenExternalLinks(true);
    footerLabel->setText("<p style=\"text-align:right;\">"
                         "<a href=\"https://github.com/rodlie/cyan\"><img src=\":/icons/GitHub-Mark-Light-32px.png\"></a>"
                         "&nbsp;&nbsp;&nbsp;"
                         "<a href=\"https://www.paypal.me/orodlie\"><img src=\":/icons/paypal.png\"></a>"
                         "</p>");

    QString textString = QString("<h2 align=\"center\">%1 %2<div style=\"font-size:small;text-transform: uppercase;\">%3</div></h2>")
            .arg(qApp->applicationName())
            .arg(CYAN_VERSION)
            .arg(tr("Pixel Viewer, Editor and Converter."));
   textString.append(QString("<p style=\"text-align:justify;font-size:small;font-weight:normal;\">%3 %4</p>"
                             "<p style=\"text-align:center;font-size:small;/*font-weight:bold;*/\">"
                             "%1 &copy; 2016-2020 <a href=\"https://github.com/rodlie\">Ole-André Rodlie</a>. %2."
                             "</p>")
                     .arg(tr("Copyright"))
                     .arg(tr("All rights reserved"))
                     .arg(tr("This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version."))
                     .arg(tr("This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.")));
    textLabel->setText(textString);
    headerLayout->addWidget(textLabel);
    headerLayout->addStretch();
    mainLayout->addWidget(headerContainer);
    mainLayout->addWidget(footerLabel);
}
