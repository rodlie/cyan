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
#include <QApplication>
#include <QPushButton>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

#include <Magick++.h>

#include "CyanCommon.h"

CyanAboutDialog::CyanAboutDialog(QWidget *parent) :
    QDialog (parent)
{
    setWindowTitle(tr("About Cyan"));
    setAttribute(Qt::WA_DeleteOnClose);

    bool hasHdri= QString(MagickCore::GetMagickFeatures()).contains("HDRI");
    size_t magickQ;
    size_t magickV;
    MagickCore::GetMagickQuantumDepth(&magickQ);
    QString magickInfo = QString(MagickCore::GetMagickVersion(&magickV)).split("Q").takeFirst();
    QString magickDepth = QString("Q%1").arg(magickQ);
    if (hasHdri) { magickDepth.append("HDRI"); }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *headerWidget = new QWidget(this);
    headerWidget->setContentsMargins(0,0,0,0);
    headerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0,0,0,0);

    QLabel *textLabel = new QLabel(this);
    textLabel->setWordWrap(true);
    textLabel->setOpenExternalLinks(true);
    textLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    QWidget *footerWidget = new QWidget(this);
    footerWidget->setContentsMargins(0,0,0,0);
    footerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *footerLayout = new QHBoxLayout(footerWidget);

    QPushButton *ghButton = new QPushButton(this);
    ghButton->setIcon(QIcon(":/icons/GitHub-Mark-Light-32px.png"));
    ghButton->setIconSize(QSize(32, 32));
    ghButton->setFlat(true);
    ghButton->setContentsMargins(0,0,0,0);
    ghButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QPushButton *qtButton = new QPushButton(this);
    qtButton->setIcon(QIcon(":/icons/built-with-Qt_Horizontal.png"));
    qtButton->setIconSize(QSize(133, 32));
    qtButton->setFlat(true);
    qtButton->setContentsMargins(0,0,0,0);
    qtButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QString cyanPatch = CYAN_VERSION_PATCH;
    if (cyanPatch.toInt() < 1) { cyanPatch = ""; }

    QString textString = QString("<p style=\"text-align:center;\"><img src=\":/icons/hicolor/128x128/apps/Cyan.png\"></p><h2 align=\"center\" style=\"font-weight:normal;\">%1<br><span style=\"font-size:medium;font-weight:bold\">Version %2 Q%3 %4</span><br><span style=\"font-size:medium;font-weight:bold\">%5</span></h2>")
            .arg(qApp->applicationName())
            .arg(CYAN_VERSION_MAJOR)
            .arg(CYAN_VERSION_MINOR)
            .arg(cyanPatch)
            .arg(CYAN_EDITION);
   textString.append(QString("<p style=\"text-align:center;font-size:small;\">"
                             "Cyan is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2.0 of the License, or (at your option) any later version."
                             " The copyright holders of Cyan hereby grant permission for non-GPL compatible plug-ins and add-ons to be used and distributed together with Cyan, provided that you also meet the terms and conditions of the licenses of those plug-ins and add-ons."
                             " Cyan is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."
                             "<br><br>"
                             "Cyan %1 &copy; <a href=\"https://github.com/rodlie\" style=\"text-decoration:none;\">Ole-André Rodlie</a>. %2.<br>"
                             "%7 <a href=\"%3\" style=\"text-decoration:none;\">%4</a> %5<br>[ %12 ]<br>%6.<br>"
                             "<table>"
                             "<tr><td><b>%8</b></td><td>&nbsp;:&nbsp;</td><td>%10</td></tr>"
                             "<tr><td><b>%9</b></td><td>&nbsp;:&nbsp;</td><td>%11</td></tr>"
                             "</table>"
                             "</p>")
                     .arg(tr("is copyright")) /* 1 */
                     .arg(tr("All rights reserved")) /* 2 */
                     .arg(MagickCore::GetMagickLicense()) /* 3 */
                     .arg(magickInfo) /* 4 */
                     .arg(magickDepth) /* 5 */
                     .arg(MagickCore::GetMagickCopyright()) /* 6 */
                     .arg(tr("Powered by")) /* 7 */
                     .arg(tr("Memory limit")) /* 8 */
                     .arg(tr("Thread limit")) /* 9 */
                     .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::memory())) /* 10 */
                     .arg(Magick::ResourceLimits::thread()) /* 11 */
                     .arg(MagickCore::GetMagickFeatures()) /* 12 */
                     );
    textLabel->setText(textString);

    headerLayout->addWidget(textLabel);
    headerLayout->addStretch();
    footerLayout->addWidget(ghButton);
    footerLayout->addStretch();
    footerLayout->addWidget(qtButton);

    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(footerWidget);

    connect(ghButton, SIGNAL(clicked()), this, SLOT(ghButtonTriggered()));
    connect(qtButton, SIGNAL(clicked()), this, SLOT(qtButtonTriggered()));
}

void CyanAboutDialog::qtButtonTriggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void CyanAboutDialog::ghButtonTriggered()
{
   QDesktopServices::openUrl(QUrl::fromUserInput("https://github.com/rodlie/cyan"));
}
