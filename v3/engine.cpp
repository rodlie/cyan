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

#include "engine.h"

#include <QMimeDatabase>
#include <QMimeType>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QUuid>
#include <QStandardPaths>
#include <QElapsedTimer>

#include "Magick++.h"

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef __MINGW32__
extern "C" WINBASEAPI BOOL WINAPI GetPhysicallyInstalledSystemMemory (PULONGLONG);
#endif
#elif defined(Q_OS_LINUX)
#include <unistd.h>
#elif defined(Q_OS_MAC)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif
#define RESOURCE_BYTE 1050000000

using namespace Cyan;

Engine::Engine(QObject *parent)
    : QObject(parent)
{
    Magick::InitializeMagick(nullptr);
}

const QByteArray
Engine::fileToByteArray(const QString &filename)
{
    if ( !QFile::exists(filename) ) { return QByteArray(); }
    QFile file(filename);
    if ( file.open(QIODevice::ReadOnly) ) {
        QByteArray fileData =  file.readAll();
        file.close();
        return fileData;
    }
    return QByteArray();
}

bool
Engine::isValidImage(const QString &filename)
{
    // TODO: should probably just use IM for this
    QStringList supported;
    supported << "image/jpeg" << "image/png" << "image/tiff";
    if ( QFile::exists(filename) ) {
        QMimeDatabase db;
        if ( supported.contains( db.mimeTypeForFile(filename).name() ) ) { return true; }
    }
    return false;
}

bool Engine::isValidImage(const QByteArray &buffer)
{
    try {
        Magick::Blob blob( buffer.data(),
                           buffer.length() );
        Magick::Image image;
        image.read(blob);
        return image.isValid();
    }
    catch(Magick::Error &error) {
        qWarning() << error.what();
        return false;
    }
    catch(Magick::Warning &warn) {
        qWarning() << warn.what();
    }
    return false;
}

bool
Engine::isValidProfile(QByteArray buffer)
{
    if (getFileColorspace(buffer) != ColorSpaceUnknown) { return true; }
    return false;
}

Engine::ColorSpace
Engine::getFileColorspace(cmsHPROFILE profile)
{
    ColorSpace colorspace = ColorSpaceUnknown;
    if (profile) {
        if (cmsGetColorSpace(profile) == cmsSigRgbData) {
            colorspace = ColorSpaceRGB;
        } else if (cmsGetColorSpace(profile) == cmsSigCmykData) {
            colorspace = ColorSpaceCMYK;
        } else if (cmsGetColorSpace(profile) == cmsSigGrayData) {
            colorspace = ColorSpaceGRAY;
        }
    }
    cmsCloseProfile(profile);
    return colorspace;
}

Engine::ColorSpace
Engine::getFileColorspace(const QString &filename)
{
    if ( QFile::exists(filename) ) {
        return getFileColorspace( cmsOpenProfileFromFile(filename.toStdString().c_str(),
                                                         "r") );
    }
    return ColorSpaceUnknown;
}

Engine::ColorSpace
Engine::getFileColorspace(QByteArray buffer)
{
    if (buffer.size() > 0) {
        return getFileColorspace( cmsOpenProfileFromMem( buffer.data(),
                                                         static_cast<cmsUInt32Number>(buffer.size() ) ) );
    }
    return ColorSpaceUnknown;
}

const QString
Engine::getProfileTag(cmsHPROFILE profile,
                      ICCTag tag)
{
    QString result;
    if (profile) {
        cmsUInt32Number size = 0;
        cmsInfoType cmsSelectedType;
        switch(tag) {
        case ICCManufacturer:
            cmsSelectedType = cmsInfoManufacturer;
            break;
        case ICCModel:
            cmsSelectedType = cmsInfoModel;
            break;
        case ICCCopyright:
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
                                                             &buffer[0],
                                                             size);
            if (size == newsize) { result = buffer.data(); }
        }
    }
    cmsCloseProfile(profile);
    return result;
}

const QString
Engine::getProfileTag(const QString &filename,
                      ICCTag tag)
{
    if ( QFile::exists(filename) ) {
        return getProfileTag(cmsOpenProfileFromFile(filename.toStdString().c_str(),
                                                    "r"),
                             tag);
    }
    return QString();
}

const QString Engine::getProfileTag(QByteArray buffer, ICCTag tag)
{
    if (buffer.size() > 0) {
        return getProfileTag(cmsOpenProfileFromMem( buffer.data(),
                                                    static_cast<cmsUInt32Number>( buffer.size() ) ),
                             tag);
    }
    return QString();
}

const QMap<QString, QString>
Engine::getProfiles(ColorSpace colorspace,
                    bool returnPaths,
                    const QString &fallback,
                    bool forceFallback)
{
    QMap<QString,QString> output;
    QStringList folders;

    folders <<  QString("%1/WINDOWS/System32/spool/drivers/color").arg( QDir::rootPath() );
    folders << "/Library/ColorSync/Profiles";
    folders << QString("%1/Library/ColorSync/Profiles").arg( QDir::homePath() );
    folders << QString("%1/.icc").arg( QDir::homePath() );
    folders << QString("%1/.color/icc").arg( QDir::homePath() );

    QStringList locations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    for (auto &dir : locations) { folders << QString("%1/color/icc").arg(dir); }

    if (returnPaths) {
        for (auto &dir : folders) { output[dir] = dir; }
        return output;
    }

    for (int i = 0; i < folders.size(); ++i) {
        QStringList filter;
        filter << "*.icc" << "*.icm";
        QDirIterator it(folders.at(i),
                        filter,
                        QDir::Files,
                        QDirIterator::Subdirectories);
        while ( it.hasNext() ) {
            QString iccFile = it.next();
            QString profile = getProfileTag(iccFile, ICCDescription);
            if ( iccFile.isEmpty() || profile.isEmpty() ) { continue; }
            if ( output.contains(profile) ) { continue; }
            if (getFileColorspace(iccFile) != colorspace) { continue; }
            output[profile] = iccFile;
        }
    }
    if ( (output.size() < 1 || forceFallback) && !fallback.isEmpty() ) {
        QString profile = getProfileTag(fileToByteArray(fallback), ICCDescription);
        output[profile] = fallback;
    }
    return output;
}

const Engine::Image
Engine::readImage(const QString &filename,
                  const QString &fallbackProfileRGB,
                  const QString &fallbackProfileCMYK,
                  const QString &fallbackProfileGRAY,
                  const Engine::RenderingIntent intent,
                  bool blackPoint,
                  bool identify)
{
    // TODO: we might need to add support for other "backends"
    return readImageIM(filename,
                       fallbackProfileRGB,
                       fallbackProfileCMYK,
                       fallbackProfileGRAY,
                       intent,
                       blackPoint,
                       identify);
}

const Engine::Image
Engine::readImageIM(const QString &filename,
                    const QString &fallbackProfileRGB,
                    const QString &fallbackProfileCMYK,
                    const QString &fallbackProfileGRAY,
                    const RenderingIntent intent,
                    bool blackPoint,
                    bool identify)
{
    QElapsedTimer timer;
    timer.start();

    Image result;
    result.success = false;
    result.filename = filename;
    Magick::Image image;
    try {
        image.read( filename.toStdString() );
    }
    catch(Magick::Error &error) {
        qWarning() << error.what();
        result.errors.append( error.what() );
        return result;
    }
    catch(Magick::Warning &warn) {
        qWarning() << warn.what();
        result.warnings.append( warn.what() );
    }
    if (identify) {
        auto wand = NewMagickWandFromImage( image.constImage() );
        result.information = MagickIdentifyImage(wand);
        wand = DestroyMagickWand(wand);
    }
    try {
        bool hasProfile = (image.iccColorProfile().length() > 0);
        result.profile = QByteArray( (char*)image.iccColorProfile().data(),
                                     image.iccColorProfile().length() );
        QString fallbackProfile = fallbackProfileRGB;
        result.colorspace = ColorSpaceRGB;
        switch ( image.colorSpace() ) {
        case Magick::CMYColorspace:
        case Magick::CMYKColorspace:
            result.colorspace = ColorSpaceCMYK;
            fallbackProfile = fallbackProfileCMYK;
            break;
        case Magick::GRAYColorspace:
            result.colorspace = ColorSpaceGRAY;
            fallbackProfile = fallbackProfileGRAY;
            break;
        default:;
        }
        if (!hasProfile) {
            switch (intent) {
            case SaturationRenderingIntent:
                image.renderingIntent(Magick::SaturationIntent);
                break;
            case PerceptualRenderingIntent:
                image.renderingIntent(Magick::PerceptualIntent);
                break;
            case AbsoluteRenderingIntent:
                image.renderingIntent(Magick::AbsoluteIntent);
                break;
            case RelativeRenderingIntent:
                image.renderingIntent(Magick::RelativeIntent);
                break;
            default:;
            }
            image.blackPointCompensation(blackPoint);
            QByteArray profileArray = fileToByteArray(fallbackProfile);
            Magick::Blob profileBlob( profileArray.data(),
                                      profileArray.size() );
            image.profile("ICC", profileBlob);
        }
    }
    catch(Magick::Error &error) {
        qWarning() << error.what();
        result.errors.append( error.what() );
        return result;
    }
    catch(Magick::Warning &warn) {
        qWarning() << warn.what();
        result.warnings.append( warn.what() );
    }
    try {
        image.magick("RGBA");
        if (image.depth() != 8) { image.depth(8); }
        Magick::Blob blob;
        image.write(&blob);
        if ( blob.length() > 0) {
            result.success = true;
            result.width = image.columns();
            result.height = image.rows();
            result.buffer = QByteArray( (char*)blob.data(),
                                        blob.length() );
        }
    }
    catch(Magick::Error &error) {
        qWarning() << error.what();
        result.errors.append( error.what() );
        return result;
    }
    catch(Magick::Warning &warn) {
        qWarning() << warn.what();
        result.warnings.append( warn.what() );
    }

    qWarning() << "elapsed" << timer.elapsed();
    return result;
}

bool
Engine::compareImages(const QByteArray image1,
                      const QByteArray image2,
                      const QString &tmpPath)
{
    Magick::Blob blob1( image1.data(),
                        image1.size() );
    Magick::Blob blob2( image2.data(),
                        image2.size() );
    Magick::Image src;
    Magick::Image dst;
    src.quiet(true);
    dst.quiet(true);
    try {
        src.read(blob1);
        dst.read(blob2);
    }
    catch(Magick::Error &error) {
        qWarning() << error.what();
        return false;
    }
    catch(Magick::Warning &warn) {
        qWarning() << warn.what();
    }
    try {
        if ( !src.compare(dst) ) { // fail, generate image diff
            double distortion = 0.0;
            Magick::Image errorImage = src.compare(dst,
                                                   MagickCore::AbsoluteErrorMetric,
                                                   &distortion);
            errorImage.quiet(true);
            if ( !tmpPath.isEmpty() ) {
                errorImage.write(QString("%1/compare-%2.jpg").arg( tmpPath,
                                                                   QUuid::createUuid ().toString() ).toStdString() );
            }
        } else {
            return true; // success, images are identical
        }
    }
    catch(Magick::Error &error) {
        qWarning() << error.what();
    }
    catch(Magick::Warning &warn) {
        qWarning() << warn.what();
    }
    return false;
}

Engine::Image
Engine::convertImage(const QByteArray &inputFileData,
                     const QByteArray &inputProfileData,
                     const QByteArray &outputProfileData,
                     RenderingIntent intent,
                     bool blackPoint,
                     bool forceInputProfile,
                     bool assignInputProfile,
                     bool applyInputProfile,
                     bool applyOutputProfile,
                     bool checkifValidResult,
                     bool display,
                     bool identify,
                     const QSize &scale)
{
    QElapsedTimer timer;
    timer.start();

    Image result;
    result.success = false;
    if (inputFileData.size() <= 0) {
        result.errors.append( tr("Missing input data") );
        return result;
    }

    if ( applyInputProfile && !isValidProfile(inputProfileData) ) {
        result.errors.append( tr("Missing valid input color profile") );
        return result;
    }
    if ( applyOutputProfile && !isValidProfile(outputProfileData) ) {
        result.errors.append( tr("Missing valid output color profile") );
        return result;
    }

    Magick::Image image;
    image.quiet(true);
    try {
        Magick::Blob inputBlob( inputFileData.data(),
                                inputFileData.length() );
        image.read(inputBlob);
    }
    catch(Magick::Error &error) {
        result.errors.append(QString::fromStdString(error.what()));
        return result;
    }
    catch(Magick::Warning &warn) {
        result.warnings.append( QString::fromStdString( warn.what() ) );
    }
    try {
        if ( scale.isValid() ) {
            image.scale( Magick::Geometry( scale.width(),
                                           scale.height() ) );
        }
        switch (intent) {
        case SaturationRenderingIntent:
            image.renderingIntent(Magick::SaturationIntent);
            break;
        case PerceptualRenderingIntent:
            image.renderingIntent(Magick::PerceptualIntent);
            break;
        case AbsoluteRenderingIntent:
            image.renderingIntent(Magick::AbsoluteIntent);
            break;
        case RelativeRenderingIntent:
            image.renderingIntent(Magick::RelativeIntent);
            break;
        default:;
        }
        image.blackPointCompensation(blackPoint);
    }
    catch(Magick::Error &error) {
        result.errors.append( QString::fromStdString( error.what() ) );
        return result;
    }
    catch(Magick::Warning &warn) {
        result.warnings.append( QString::fromStdString(warn.what() ) );
    }
    try {
        Magick::Blob inputProfile( inputProfileData.data(),
                                   inputProfileData.size() );
        Magick::Blob outputProfile( outputProfileData.data(),
                                    outputProfileData.size() );
        if (image.iccColorProfile().length() == 0 || forceInputProfile) {
            qWarning() << "image missing color profile or override wanted";
            if (forceInputProfile && assignInputProfile) {
                qWarning() << "strip color profile(s) requested";
                image.profile( "ICC", Magick::Blob() );
                image.profile( "ICM", Magick::Blob() );
            }
            if(applyInputProfile) { image.profile("ICC", inputProfile); }
        }
        if (applyOutputProfile) { image.profile("ICC", outputProfile); }
    }
    catch(Magick::Error &error) {
        result.errors.append( QString::fromStdString( error.what() ) );
        return result;
    }
    catch(Magick::Warning &warn) {
        result.warnings.append( QString::fromStdString( warn.what() ) );
    }
    try {
        if (identify) {
            auto wand = NewMagickWandFromImage( image.constImage() );
            result.information = MagickIdentifyImage(wand);
            wand = DestroyMagickWand(wand);
        }
        result.width = image.columns();
        result.height = image.rows();
        if (display) {
            image.magick("RGBA");
            if (image.depth() != 8) { image.depth(8); }
        }
        Magick::Blob outputBlob;
        image.write(&outputBlob);
        result.buffer = QByteArray( (char*)outputBlob.data(),
                                    outputBlob.length() );
    }
    catch(Magick::Error &error) {
        qWarning() << error.what();
        return result;
    }
    catch(Magick::Warning &warn) {
        qWarning() << warn.what();
    }
    if (display) { result.success = result.buffer.length() > 0; }
    else {
        if ( checkifValidResult &&
             isValidImage(result.buffer) ) { result.success = true; }
    }

    qWarning() << "elapsed" << timer.elapsed();
    return result;
}

bool
Engine::hasDelegate(const QString &delegate)
{
    if ( delegate.isEmpty() ) { return false; }
    std::string result = MagickCore::GetMagickDelegates();
    if ( QString::fromStdString(result).contains(delegate) ) { return true; }
    return false;
}

bool
Engine::hasFeature(const QString &feature)
{
    if ( feature.isEmpty() ) { return false;}
    std::string result = MagickCore::GetMagickFeatures();
    if ( QString::fromStdString(result).contains(feature) ) { return true; }
    return false;
}

const QString
Engine::supportedQuantumDepth()
{
    return QString(MagickQuantumDepth);
}

bool
Engine::hasJPEG()
{
    return hasDelegate("jpeg");
}

bool
Engine::hasPNG()
{
    return hasDelegate("png");
}

bool
Engine::hasTIFF()
{
    return hasDelegate("tiff");
}

bool
Engine::hasLCMS()
{
    return hasDelegate("lcms");
}

bool
Engine::hasHDRI()
{
    return hasFeature("HDRI");
}

bool
Engine::hasMP()
{
    return hasFeature("OpenMP");
}

const QString
Engine::identify(const QByteArray &buffer)
{
    if (buffer.size() > 0) {
        MagickCore::MagickWand *wand = MagickCore::NewMagickWand();
        QString result;
        if ( MagickReadImageBlob( wand,
                                  buffer.data(),
                                  buffer.size() ) )
        {
            result = MagickIdentifyImage(wand);
        }
        wand = DestroyMagickWand(wand);
        return result;
    }
    return QString();
}

const QString
Engine::identify(const QString &filename)
{
    return identify( fileToByteArray(filename) );
}

int
Engine::getDiskResource()
{
    return qRound( static_cast<double>(Magick::ResourceLimits::disk() / RESOURCE_BYTE) );
}

void
Engine::setDiskResource(int gib)
{
    try {
        Magick::ResourceLimits::disk( static_cast<qulonglong>(gib) * static_cast<qulonglong>(RESOURCE_BYTE) );
    }
    catch(Magick::Error &error ) { qWarning() << error.what(); }
    catch(Magick::Warning &warn ) { qWarning() << warn.what(); }
}

int
Engine::getMemoryResource()
{
    return qRound( static_cast<double>(Magick::ResourceLimits::memory() / RESOURCE_BYTE) );
}

void
Engine::setMemoryResource(int gib)
{
    try {
        Magick::ResourceLimits::memory( static_cast<qulonglong>(gib) * static_cast<qulonglong>(RESOURCE_BYTE) );
        Magick::ResourceLimits::map( static_cast<qulonglong>(gib) * static_cast<qulonglong>(RESOURCE_BYTE) );
    }
    catch(Magick::Error &error) { qWarning() << error.what(); }
    catch(Magick::Warning &warn) { qWarning() << warn.what(); }
}

int
Engine::getTotalRam(int percent)
{
    int ram = 0;
#ifdef Q_OS_WIN
    unsigned long long physicalMemory = 0;
    GetPhysicallyInstalledSystemMemory(&physicalMemory);
    int gib = qRound(static_cast<double>((physicalMemory*1024)/RESOURCE_BYTE));
    ram = qRound(static_cast<double>((gib*percent)/100));
#elif defined(Q_OS_LINUX)
#if defined _SC_PHYS_PAGES && defined _SC_PAGESIZE
    unsigned long long physicalMemory = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE);
    int gib = qRound( static_cast<double>(physicalMemory / 1024000000) );
    ram = qRound( static_cast<double>( (gib*percent) / 100 ) );
#endif
#elif defined(Q_OS_MAC)
    int mib [] = { CTL_HW, HW_MEMSIZE };
    int64_t physicalMemory = 0;
    size_t length = sizeof(physicalMemory);
    sysctl(mib, 2, &physicalMemory, &length, NULL, 0);
    int gib = qRound( static_cast<double>(physicalMemory / 1024000000) );
    ram = qRound( static_cast<double>( (gib*percent) / 100) );
#endif
    Q_UNUSED(percent)

    if (ram < 4) { ram = 4; }
    return ram;
}

const QStringList
Engine::supportedReadFormats()
{
    QStringList formats;
    formats << "*.psd" << "*.xcf" <<  "*.miff" << "*.bmp";
    if ( hasJPEG() ) { formats << "*.jpg" << "*.jpeg"; }
    if ( hasPNG() ) { formats << "*.png"; }
    if ( hasTIFF() ) { formats << "*.tif" << "*.tiff"; }
    return formats;
}

const QStringList Engine::supportedWriteFormats()
{
    QStringList formats;
    formats << "*.psd";
    if ( hasJPEG() ) { formats << "*.jpg" << "*.jpeg"; }
    //if ( hasPNG() ) { formats << "*.png"; } // can we store an ICC with IM?
    if ( hasTIFF() ) { formats << "*.tif" << "*.tiff"; }
    return formats;
}
