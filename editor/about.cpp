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
#include <QMessageBox>

void Editor::aboutCyan()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("About Cyan"));

    box.setIconPixmap(QPixmap::fromImage(QImage(":/icons/hicolor/128x128/apps/cyan.png")));

    QString about;
    about.append(QString("<h3>Cyan %1</h3>")
                 .arg(CYAN_VERSION));
    about.append(QString("<p>%1</p>")
                 .arg(tr("Cyan is an open source cross-platform general-purpose image editor.")));
    about.append(QString("<p>%1</p>")
                 .arg(tr("&copy;2019 Ole-André Rodlie. All rights reserved.")));
    about.append(QString("<p>%1</p>")
                 .arg(tr("Cyan is distributed under the %1 license.")
                      .arg(QString("<a href=\"http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.html\">"
                                   "CeCILL v2.1</a>"))));
    box.setText(about);
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

    QString about;
    about.append(QString("<h3>%1 %2%3</h3>")
                 .arg(MagickPackageName)
                 .arg(MagickLibVersionText)
                 .arg(MagickLibAddendum));
    about.append(QString("<p><a href=\"https://imagemagick.org\">ImageMagick®</a> is used to read, create, save, edit, compose, and  convert bitmap images.</p><p>ImageMagick is distributed under the following <a href=\"https://www.imagemagick.org/script/license.php\">license</a>.</p>"));
    about.append(QString("<p>%1</p>").arg(MagickCopyright));

    about.append(QString("<p><strong>Features</strong>:<br><br>%1 %2</p>").arg(MagickQuantumDepth).arg(MagickCore::GetMagickFeatures()));
    about.append(QString("<p><strong>Delegates</strong>:<br><br>%1</p>").arg(MagickCore::GetMagickDelegates()));
    about.append(QString("<p><strong>Disk Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::disk())));
    about.append(QString("<strong>Area Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::area(),false, true)));
    about.append(QString("<strong>Map Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::map())));
    about.append(QString("<strong>Memory Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::memory())));
    about.append(QString("<strong>Width Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::width(), true)));
    about.append(QString("<strong>Height Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::height(), true)));
    about.append(QString("<strong>Thread Limit</strong>: %1</p>").arg(Magick::ResourceLimits::thread()));

    box.setText(about);
    box.exec();
}

void Editor::aboutLcms()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("About Little CMS"));
    box.setIconPixmap(QPixmap::fromImage(QImage(":/icons/lcms_logo.png")));

    QString about;
    about.append(QString("<h3>Little CMS %1</h3>")
                 .arg(QString::number(LCMS_VERSION)
                      .insert(1,".")
                      .remove(2,1)
                      .remove(3,1)));
    about.append(QString("<p><a href=\"http://www.littlecms.com/\">Little CMS</a> is an small-footprint color management engine, with special focus on accuracy and performance.</p><p>Little CMS is distributed under the following <a href=\"https://opensource.org/licenses/mit-license.php\">license</a>.</p>"));
    about.append(QString("<p>Copyright &copy; 2018 Marti Maria Saguer.<br>All rights reserved.</p>"));
    box.setText(about);
    box.exec();
}
