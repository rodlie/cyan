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

#include "aboutdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>

#include "engine.h"
#include <Magick++.h>

using namespace Cyan;

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog (parent)
{
    setWindowTitle( tr("About Cyan") );

    bool hasHdri= QString( MagickCore::GetMagickFeatures() ).contains("HDRI");
    size_t magickQ;
    size_t magickV;
    MagickCore::GetMagickQuantumDepth(&magickQ);
    QString magickInfo = QString( MagickCore::GetMagickVersion(&magickV) ).split("Q").takeFirst();
    QString magickDepth = QString("Q%1").arg(magickQ);
    if (hasHdri) { magickDepth.append("HDRI"); }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *headerWidget = new QWidget(this);
    headerWidget->setContentsMargins(0, 0, 0, 0);
    headerWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *textLabel = new QLabel(this);
    textLabel->setWordWrap(true);
    textLabel->setOpenExternalLinks(true);
    textLabel->setSizePolicy(QSizePolicy::MinimumExpanding,
                             QSizePolicy::MinimumExpanding);

    QString textString = QString("<p style=\"text-align:center;\"><img src=\":/icons/hicolor/128x128/apps/Cyan.png\"></p><h2 align=\"center\" style=\"font-weight:normal;\">Cyan %1</h2>")
                         .arg(CYAN_VERSION);
    textString.append(QString("<p style=\"text-align:center;font-size:small;\">"
                              "Cyan is free software; you can redistribute it and/or modify it under the terms of<br>the GNU General Public License as published by the Free Software Foundation;<br>either version 3 of the License, or (at your option) any later version."
                              "<br><br>"
                              "Cyan is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;<br>without even the implied warranty of MERCHANTABILITY<br>or FITNESS FOR A PARTICULAR PURPOSE."
                              "<br><br>"
                              "Cyan %1 &copy; <a href=\"https://github.com/rodlie\" style=\"text-decoration:none;\">Ole-André Rodlie</a>. %2.<br><br>"
                              "%7 <a href=\"%3\" style=\"text-decoration:none;\">%4</a> %5<br>[ %12 ]<br>%6.<br>"
                              "<table>"
                              "<tr><td><b>%8</b></td><td>&nbsp;:&nbsp;</td><td>%10</td></tr>"
                              "<tr><td><b>%9</b></td><td>&nbsp;:&nbsp;</td><td>%11</td></tr>"
                              "</table>"
                              "</p>")
                      .arg( tr("is copyright") )
                      .arg( tr("All rights reserved") )
                      .arg( MagickCore::GetMagickLicense() )
                      .arg(magickInfo)
                      .arg(magickDepth)
                      .arg( MagickCore::GetMagickCopyright() )
                      .arg( tr("Powered by") )
                      .arg( tr("Memory limit") )
                      .arg( tr("Thread limit") )
                      .arg( Engine::humanFileSize( Magick::ResourceLimits::memory() ) )
                      .arg( Magick::ResourceLimits::thread() )
                      .arg( MagickCore::GetMagickFeatures() )
                      );
    textLabel->setText(textString);

    headerLayout->addWidget(textLabel);
    headerLayout->addStretch();

    mainLayout->addWidget(headerWidget);
}
