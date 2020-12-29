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

    QLabel *textLabel = new QLabel(this);
    textLabel->setWordWrap(true);
    textLabel->setOpenExternalLinks(true);

    QWidget *footerWidget = new QWidget(this);
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

    QString textString = QString("<h2 align=\"center\">%1 %2</h2><h3 align=\"center\" style=\"text-transform: uppercase;\">%3</h3>")
            .arg(qApp->applicationName())
            .arg(CYAN_VERSION)
            .arg(tr("Pixel Viewer, Editor and Converter."));
   textString.append(QString("<p style=\"text-align:justify;\">%3 %4</p>"
                             "<p style=\"text-align:center;\">"
                             "%1 &copy; 2020 <a href=\"https://github.com/rodlie\">Ole-André Rodlie</a>. %2."
                             "</p>"
                             "<p style=\"text-align:center;\">"
                             "%9 <a href=\"http://www.littlecms.com/\">Little CMS %10</a><br>%1 &copy; Marti Maria Saguer.<br><br>"
                             "%9 <a href=\"%5\">%6</a> %7<br>%8.<br>"
                             "<table>"
                             "<tr><td><b>%11</b></td><td>&nbsp;:&nbsp;</td><td>%13</td></tr>"
                             "<tr><td><b>%12</b></td><td>&nbsp;:&nbsp;</td><td>%14</td></tr>"
                             "</table>"
                             "</p>")
                     .arg(tr("Copyright")) /* 1 */
                     .arg(tr("All rights reserved")) /* 2 */
                     .arg(tr("This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2.0 of the License, or (at your option) any later version.")) /* 3 */
                     .arg(tr("This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.")) /* 4 */
                     .arg(MagickCore::GetMagickLicense()) /* 5 */
                     .arg(magickInfo) /* 6 */
                     .arg(magickDepth) /* 7 */
                     .arg(MagickCore::GetMagickCopyright()) /* 8 */
                     .arg(tr("Powered by")) /* 9 */
                     .arg(QString::number(LCMS_VERSION).insert(1,".").remove(2,1).remove(3,1)) /* 10 */
                     .arg(tr("Memory limit")) /* 11 */
                     .arg(tr("Thread limit")) /* 12 */
                     .arg(CyanCommon::humanFileSize(Magick::ResourceLimits::memory())) /* 13 */
                     .arg(Magick::ResourceLimits::thread()) /* 14 */
                     );
    textLabel->setText(textString);
    headerLayout->addWidget(textLabel);
    headerLayout->addStretch();
    footerLayout->addStretch();
    footerLayout->addWidget(ghButton);
    footerLayout->addWidget(qtButton);
    mainLayout->addWidget(headerContainer);
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
