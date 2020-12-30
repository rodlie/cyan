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

    QString textString = QString("<h2 align=\"center\">%1 %2</h2>")
            .arg(qApp->applicationName())
            .arg(CYAN_VERSION);
   textString.append(QString("<p style=\"text-align:center;\">"
                             "%1 &copy; 2020 <a href=\"https://github.com/rodlie\">Ole-André Rodlie</a>. %2."
                             "</p>"
                             "<p style=\"text-align:center;\">"
                             "%7 <a href=\"%3\">%4</a> %5<br>%6.<br>"
                             "<table>"
                             "<tr><td><b>%8</b></td><td>&nbsp;:&nbsp;</td><td>%10</td></tr>"
                             "<tr><td><b>%9</b></td><td>&nbsp;:&nbsp;</td><td>%11</td></tr>"
                             "</table>"
                             "</p>")
                     .arg(tr("Copyright")) /* 1 */
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
                     );
    textLabel->setText(textString);
    headerLayout->addWidget(textLabel);
    headerLayout->addStretch();
    footerLayout->addWidget(ghButton);
    footerLayout->addStretch();
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
