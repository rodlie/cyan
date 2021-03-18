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

#include "CyanCommon.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QAction>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QApplication>
#include <QVector>

#ifdef Q_OS_WIN
#include <Windows.h>
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#endif



#define RESOURCE_BYTE 1050000000

CyanCommon::CyanCommon(QObject *parent): QObject (parent)
{
}

/*QString CyanCommon::timestamp()
{
    QDateTime date;
    return date.currentDateTime().toString("yyyyMMddHHmmsszzz");
}*/


const QString CyanCommon::canvasWindowTitle(Magick::Image image)
{
    // image @ colorspace widthxheight dpixdpi bit
    QString label = QObject::tr("New image");
    if (!QString::fromStdString(image.label()).isEmpty()) {
        label = QString::fromStdString(image.label());
    } else if (!QString::fromStdString(image.fileName()).isEmpty()) {
        label = QString::fromStdString(image.fileName());
    }

    QString profile = CyanCommon::getProfileTag(image.iccColorProfile());
    if (profile.isEmpty()) { profile = QObject::tr("N/A"); }
    QString colorspace = colorspaceToString(image.colorSpace());
    QString result;
    try {
        int width = static_cast<int>(image.columns());
        int height = static_cast<int>(image.rows());
        int depth = static_cast<int>(image.depth());
        int dpiX = static_cast<int>(image.density().x());
        int dpiY = static_cast<int>(image.density().y());
        result = QString("%1 @ %2 (%8) %3x%4 %5x%6 %7-bit")
                        .arg(label)
                        .arg(colorspace)
                        .arg(width)
                        .arg(height)
                        .arg(dpiX)
                         .arg(dpiY)
                        .arg(depth)
                        .arg(profile);
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }

    return result;
}

int CyanCommon::getDiskResource()
{
    return qRound(static_cast<double>(Magick::ResourceLimits::disk()/RESOURCE_BYTE));
}

void CyanCommon::setDiskResource(int gib)
{
    try {
        Magick::ResourceLimits::disk(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
}

int CyanCommon::getMemoryResource()
{
    return qRound(static_cast<double>(Magick::ResourceLimits::memory()/RESOURCE_BYTE));
}

void CyanCommon::setMemoryResource(int gib)
{
    if (gib < 2) { gib = 2; } // 2 is minimum
    try {
        Magick::ResourceLimits::memory(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
        Magick::ResourceLimits::map(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qDebug() << warn_.what(); }
}

int CyanCommon::getTotalRam(int percent)
{
#ifdef Q_OS_WIN
    unsigned long long physicalMemory = 0;
    GetPhysicallyInstalledSystemMemory(&physicalMemory);
    int gib = qRound(static_cast<double>((physicalMemory*1024)/RESOURCE_BYTE));
    return qRound(static_cast<double>((gib*percent)/100));
#elif defined(Q_OS_LINUX)
#if defined _SC_PHYS_PAGES && defined _SC_PAGESIZE
    unsigned long long physicalMemory = sysconf(_SC_PHYS_PAGES)*sysconf(_SC_PAGESIZE);
    int gib = qRound(static_cast<double>(physicalMemory/1024000000));
    return qRound(static_cast<double>((gib*percent)/100));
#endif
#endif
    Q_UNUSED(percent)
    return 4; // fallback to 4
}

void CyanCommon::setThreadResources(int thread)
{
    if (thread < 1) { return; }
    Magick::ResourceLimits::thread(static_cast<qulonglong>(thread));
}


QStringList CyanCommon::getColorProfilesPath()
{
    QStringList folders;
    folders << QDir::rootPath() + "/WINDOWS/System32/spool/drivers/color";
    folders << "/Library/ColorSync/Profiles";
    folders << QDir::homePath() + "/Library/ColorSync/Profiles";
    folders << "/usr/share/color/icc";
    folders << "/usr/local/share/color/icc";
    folders << QDir::homePath() + "/.color/icc";
    folders << qApp->applicationDirPath() + "/profiles";
    folders << qApp->applicationDirPath() + "/../Resources/Profiles";
    folders << qApp->applicationDirPath() + "/../share/Cyan/profiles";
    return folders;
}

QMap<QString, QString> CyanCommon::getColorProfiles(Magick::ColorspaceType colorspace)
{
    QMap<QString, QString> output;
    QStringList folders = getColorProfilesPath();
    bool getAll = colorspace == Magick::ColorspaceType::UndefinedColorspace? true : false;

    for (int i = 0; i < folders.size(); ++i) {
        QStringList filter;
        filter << "*.icc" << "*.icm";
        QDirIterator it(folders.at(i), filter, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString iccFile = it.next();
            QString profile = getProfileTag(iccFile);
            if (iccFile.isEmpty() || profile.isEmpty()) { continue; }
            Magick::ColorspaceType iccColorspace = getProfileColorspace(iccFile);
            switch (iccColorspace) {
            case Magick::RGBColorspace:
            case Magick::scRGBColorspace:
                iccColorspace = Magick::sRGBColorspace;
                break;
            default:;
            }
            switch (colorspace) {
            case Magick::RGBColorspace:
            case Magick::scRGBColorspace:
            case Magick::YCbCrColorspace: /* heic */
                colorspace = Magick::sRGBColorspace;
                break;
            default:;
            }
            if (iccColorspace != colorspace && !getAll) { continue; }
            output[profile] = iccFile;
        }
    }
    return output;
}

Magick::ColorspaceType CyanCommon::getProfileColorspace(const QString &filename)
{
    Magick::ColorspaceType colorspace = Magick::UndefinedColorspace;
    if (!filename.isEmpty()) {
        colorspace = getProfileColorspace(cmsOpenProfileFromFile(filename.toStdString().c_str(), "r"));
    }
    return colorspace;
}

Magick::ColorspaceType CyanCommon::getProfileColorspace(cmsHPROFILE profile)
{
    Magick::ColorspaceType result = Magick::UndefinedColorspace;
    if (profile) {
        if (cmsGetColorSpace(profile) == cmsSigRgbData) {
            result = Magick::sRGBColorspace;
        } else if (cmsGetColorSpace(profile) == cmsSigCmykData) {
            result = Magick::CMYKColorspace;
        } else if (cmsGetColorSpace(profile) == cmsSigGrayData) {
            result = Magick::GRAYColorspace;
        }
    }
    cmsCloseProfile(profile);
    return result;
}

const QString CyanCommon::getProfileTag(Magick::Blob buffer,
                                        CyanCommon::ICCTag tag)
{
    qDebug() << "get profile tag from blob" << buffer.length();
    if (buffer.length()>0) {
        return getProfileTag(cmsOpenProfileFromMem(buffer.data(),
                                                   static_cast<cmsUInt32Number>(buffer.length())),tag);
    }
    return QString();
}

const QString CyanCommon::getProfileTag(const QString filename,
                                    CyanCommon::ICCTag tag)
{
    if (!filename.isEmpty()) {
        return getProfileTag(cmsOpenProfileFromFile(filename.toStdString().c_str(), "r"), tag);
    }
    return QString();
}

const QString CyanCommon::getProfileTag(cmsHPROFILE profile,
                                    CyanCommon::ICCTag tag)
{
    std::string result;
    if (profile) {
        cmsUInt32Number size = 0;
        cmsInfoType cmsSelectedType;
        switch(tag) {
        case CyanCommon::ICCManufacturer:
            cmsSelectedType = cmsInfoManufacturer;
            break;
        case CyanCommon::ICCModel:
            cmsSelectedType = cmsInfoModel;
            break;
        case CyanCommon::ICCCopyright:
            cmsSelectedType = cmsInfoCopyright;
            break;
        default:
            cmsSelectedType = cmsInfoDescription;
        }
        size = cmsGetProfileInfoASCII(profile,
                                      cmsSelectedType,
                                      "en",
                                      "US",
                                      nullptr,
                                      0);
        if (size > 0) {
            std::vector<char> buffer(size);
            cmsUInt32Number newsize = cmsGetProfileInfoASCII(profile,
                                                             cmsSelectedType,
                                                             "en",
                                                             "US",
                                                             &buffer[0], size);
            if (size == newsize) {
                result = buffer.data();
            }
        }
    }
    cmsCloseProfile(profile);
    return QString::fromStdString(result);
}


const QString CyanCommon::humanFileSize(float num, bool mp, bool are)
{
    float byte = 1024.0;
    QStringList list;
    list << "KB";

    if (are) {
        byte = 1000.0;
        list << "MP" << "GP";
    } else if (mp) {
        byte = 1000.0;
        list << "MP";
    } else { list << "MB" << "GB" << "TB"; }

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= byte && i.hasNext()) {
        unit = i.next();
        num /= byte;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

const QString CyanCommon::colorspaceToString(Magick::ColorspaceType colorspace)
{
    QString result;
    switch(colorspace) {
    case Magick::RGBColorspace:
    case Magick::sRGBColorspace:
    case Magick::scRGBColorspace:
    case Magick::TransparentColorspace:
        result = "RGB";
        break;
    case Magick::CMYKColorspace:
        result = "CMYK";
        break;
    case Magick::CMYColorspace:
        result = "CMY";
        break;
    case Magick::GRAYColorspace:
        result = "GRAY";
        break;
    case Magick::LinearGRAYColorspace:
        result = "LinearGRAY";
        break;
    case Magick::LabColorspace:
        result = "LAB";
        break;
    case Magick::HSBColorspace:
        result = "HSB";
        break;
    case Magick::HSLColorspace:
        result = "HSL";
        break;
    case Magick::Rec601YCbCrColorspace:
        result = "Rec601YCbCr";
        break;
    case Magick::Rec709YCbCrColorspace:
        result = "Rec709YCbCr";
        break;
    case Magick::YCbCrColorspace:
        result = "YCbCr";
        break;
    default:
        result = QObject::tr("Other");
    }
    return result;
}

const QString CyanCommon::supportedImageMagickVersion()
{
    QVector<QString> versions;
    versions.push_back("7.0.10-26");
    versions.push_back("7.0.11-4");

    size_t magickV;
    QString version = QString(MagickCore::GetMagickVersion(&magickV)).split("Q").takeFirst().split("ImageMagick").takeLast().trimmed();

    if (versions.contains(version)) { return  QString(); }
    return version;
}

