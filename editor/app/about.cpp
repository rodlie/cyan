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

    QString version;
#ifdef CYAN_GIT
    QString git = CYAN_GIT;
    if (git.isEmpty()) { version = CYAN_VERSION; }
    else {
        version = QString("%1 (<a href=\"https://github.com/rodlie/cyan/commit/%2\">%2</a>)")
                  .arg(CYAN_VERSION)
                  .arg(git);
    }
#else
    version = CYAN_VERSION;
#endif
    QString about;
    about.append(QString("<h2>Cyan %1 Alpha</h2>")
                 .arg(version));
    about.append(QString("<p>%1</p>")
                 .arg(tr("Simple general-purpose image editor. Cyan is currently under early development and have many bugs and missing features.")));
    about.append(QString("<p>%1</p>")
                 .arg(tr("&copy;2019 Ole-André Rodlie, FxArena DA. All rights reserved.")));
    about.append(QString("<p>%1</p>")
                 .arg(tr("Cyan is distributed under the %1 license.")
                      .arg(QString("<a href=\"http://www.cecill.info/licences/Licence_CeCILL_V2.1-en.html\">"
                                   "CeCILL v2.1</a>"))));

    box.setText(about);
    box.setInformativeText(QString("<p><a href=\"https://cyan.fxarena.net\">https://cyan.fxarena.net</a>"
                                   "<br><a href=\"https://github.com/rodlie/cyan\">https://github.com/rodlie/cyan</a></p>"));
    box.setStyleSheet("QLabel { min-width: 350px; }");
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
    about.append(QString("<p><a href=\"https://imagemagick.org\">%2®</a> is used to read"
                         ", create, save, edit, compose, and  convert bitmap images.</p><p>"
                         "%2 is distributed under the following <a href=\"%1\">license</a>.</p>")
                 .arg(MagickCore::GetMagickLicense())
                 .arg(MagickCore::GetMagickPackageName()));
    about.append(QString("<p>%1</p>").arg(MagickCore::GetMagickCopyright()));

    about.append(QString("<p><strong>Features</strong>:<br><br>Q%1 %2 %3</p>").arg(magickQ).arg(MagickCore::GetMagickFeatures()).arg(MagickCore::GetMagickDelegates()));
    about.append(QString("<p><strong>Disk Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::disk())));
    about.append(QString("<strong>Area Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::area(),false, true)));
    about.append(QString("<strong>Map Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::map())));
    about.append(QString("<strong>Memory Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::memory())));
    about.append(QString("<strong>Width Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::width(), true)));
    about.append(QString("<strong>Height Limit</strong>: %1<br>").arg(CyanCommon::humanFileSize(Magick::ResourceLimits::height(), true)));
    about.append(QString("<strong>Thread Limit</strong>: %1</p>").arg(Magick::ResourceLimits::thread()));

    box.setText(about);
    box.setStyleSheet("QLabel { min-width: 350px; }");
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
    box.setStyleSheet("QLabel { min-width: 350px; }");
    box.exec();
}
