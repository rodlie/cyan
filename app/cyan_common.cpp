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

#include "cyan_common.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QAction>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QApplication>

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
    // filename @ colorspace widthxheight dpixdpi bit
    QString label = QObject::tr("New image");
    if (!QString::fromStdString(image.label()).isEmpty()) {
        label = QString::fromStdString(image.label());
    }
    QString profile = CyanCommon::getProfileTag(image.iccColorProfile());
    if (profile.isEmpty()) { profile = "N/A"; }
    QString colorspace;
    switch(image.colorSpace()) {
    case Magick::RGBColorspace:
    case Magick::sRGBColorspace:
    case Magick::scRGBColorspace:
    case Magick::TransparentColorspace:
        colorspace = "RGB";
        break;
    case Magick::CMYKColorspace:
        colorspace = "CMYK";
        break;
    case Magick::CMYColorspace:
        colorspace = "CMY";
        break;
    case Magick::GRAYColorspace:
        colorspace = "GRAY";
        break;
    case Magick::LinearGRAYColorspace:
        colorspace = "LinearGRAY";
        break;
    case Magick::LabColorspace:
        colorspace = "LAB";
        break;
    case Magick::HSBColorspace:
        colorspace = "HSB";
        break;
    case Magick::HSLColorspace:
        colorspace = "HSL";
        break;
    case Magick::Rec601YCbCrColorspace:
        colorspace = "Rec601YCbCr";
        break;
    case Magick::Rec709YCbCrColorspace:
        colorspace = "Rec709YCbCr";
        break;
    default:
        colorspace = QObject::tr("Other");
    }
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
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

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
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
}

int CyanCommon::getMemoryResource()
{
    return qRound(static_cast<double>(Magick::ResourceLimits::memory()/RESOURCE_BYTE));
}

void CyanCommon::setMemoryResource(int gib)
{
    try {
        Magick::ResourceLimits::memory(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
        Magick::ResourceLimits::map(static_cast<qulonglong>(gib)*static_cast<qulonglong>(RESOURCE_BYTE));
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
}

void CyanCommon::setThreadResources(int thread)
{
    if (thread == 0) { return; }
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
    folders << qApp->applicationDirPath() + "/../share/Cyan/profiles";
    return folders;
}

QMap<QString, QString> CyanCommon::getColorProfiles(Magick::ColorspaceType colorspace)
{
    QMap<QString, QString> output;
    QStringList folders = getColorProfilesPath();
    for (int i = 0; i < folders.size(); ++i) {
        QStringList filter;
        filter << "*.icc" << "*.icm";
        QDirIterator it(folders.at(i), filter, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString iccFile = it.next();
            QString profile = getProfileTag(iccFile);
            if (iccFile.isEmpty() || profile.isEmpty()) { continue; }
            if (getProfileColorspace(iccFile)!= colorspace) { continue; }
            output[profile] = iccFile;
        }
    }
    return output;
}

Magick::ColorspaceType CyanCommon::getProfileColorspace(const QString &filename)
{
    if (!filename.isEmpty()) {
        return getProfileColorspace(cmsOpenProfileFromFile(filename.toStdString().c_str(), "r"));
    }
    return Magick::UndefinedColorspace;
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


#ifdef WITH_FFMPEG
QByteArray CyanCommon::getEmbeddedCoverArt(const QString &filename)
{
    QByteArray result;
    if (!filename.isEmpty()) {
        AVCodecContext  *pCodecCtx;
        AVFormatContext *pFormatCtx = avformat_alloc_context();
        AVCodec * pCodec;

        if (avformat_open_input(&pFormatCtx,filename.toUtf8().data(),
                                Q_NULLPTR,
                                Q_NULLPTR) != 0) { return result; }
        if (avformat_find_stream_info(pFormatCtx,
                                      Q_NULLPTR) < 0) { return result; }

        av_dump_format(pFormatCtx, 0, filename.toUtf8().data(), 0);
        int videoStream = -1;

        for (int i=0; i < static_cast<int>(pFormatCtx->nb_streams); i++) {
            if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStream = i;
                break;
            }
        }
        if (videoStream == -1) { return result; }

        pCodec =avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
        pCodecCtx = avcodec_alloc_context3(Q_NULLPTR);
        if (pCodec == Q_NULLPTR || pCodecCtx == Q_NULLPTR) { return result; }
        if (avcodec_parameters_to_context(pCodecCtx,
                                          pFormatCtx->streams[videoStream]->codecpar)<0)
        { return result; }
        if (avcodec_open2(pCodecCtx,
                         pCodec,
                         Q_NULLPTR) < 0) { return result; }

        if (pFormatCtx->streams[videoStream]->disposition == AV_DISPOSITION_ATTACHED_PIC) {
            AVPacket pkt = pFormatCtx->streams[videoStream]->attached_pic;
            if (pkt.size>0) {
                QByteArray attachedPix = QByteArray(reinterpret_cast<const char*>(pkt.data),
                                                    pkt.size);
                if (attachedPix.length()>0) {
                    avcodec_close(pCodecCtx);
                    avformat_close_input(&pFormatCtx);
                    return attachedPix;
                }
            }
        }
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
    }
    return result;
}

int CyanCommon::getVideoMaxFrames(const QString &filename)
{
    int result = -1;
    if (filename.isEmpty()) { return result; }

    AVCodecContext  *pCodecCtx;
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVCodec * pCodec;

    if (avformat_open_input(&pFormatCtx,filename.toUtf8().data(),
                            Q_NULLPTR,
                            Q_NULLPTR) != 0) { return result; }
    if (avformat_find_stream_info(pFormatCtx,
                                  Q_NULLPTR) < 0) { return result; }

    av_dump_format(pFormatCtx, 0, filename.toUtf8().data(), 0);
    int videoStream = -1;

    for (int i=0; i < static_cast<int>(pFormatCtx->nb_streams); i++) {
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) { return result; }

    pCodec =avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
    pCodecCtx = avcodec_alloc_context3(Q_NULLPTR);
    if (pCodec == Q_NULLPTR || pCodecCtx == Q_NULLPTR) { return result; }
    if (avcodec_parameters_to_context(pCodecCtx,
                                      pFormatCtx->streams[videoStream]->codecpar)<0)
    { return result; }
    if (avcodec_open2(pCodecCtx,
                     pCodec,
                     Q_NULLPTR) < 0) { return result; }

    double fps = av_q2d(pFormatCtx->streams[videoStream]->r_frame_rate);
    double dur = static_cast<double>(pFormatCtx->duration)/AV_TIME_BASE;
    result = qRound((dur*fps)/2);

    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    return  result;
}

Magick::Image CyanCommon::getVideoFrame(const QString &filename,
                                    int frame)
{
    Magick::Image result;

    if (filename.isEmpty()) { return result; }

    AVCodecContext  *pCodecCtx;
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    AVCodec * pCodec;
    AVFrame *pFrame, *pFrameRGB;

    if (avformat_open_input(&pFormatCtx,filename.toUtf8().data(),
                            Q_NULLPTR,
                            Q_NULLPTR) != 0) { return result; }
    if (avformat_find_stream_info(pFormatCtx,
                                  Q_NULLPTR) < 0) { return result; }

    av_dump_format(pFormatCtx, 0, filename.toUtf8().data(), 0);
    int videoStream = -1;

    for (int i=0; i < static_cast<int>(pFormatCtx->nb_streams); i++) {
        if(pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) { return result; }

    pCodec =avcodec_find_decoder(pFormatCtx->streams[videoStream]->codecpar->codec_id);
    pCodecCtx = avcodec_alloc_context3(Q_NULLPTR);
    if (pCodec == Q_NULLPTR || pCodecCtx == Q_NULLPTR) { return result; }
    if (avcodec_parameters_to_context(pCodecCtx,
                                      pFormatCtx->streams[videoStream]->codecpar)<0)
    { return result; }
    if (avcodec_open2(pCodecCtx,
                     pCodec,
                     Q_NULLPTR) < 0) { return result; }

    pFrame    = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    uint8_t *buffer;
    int numBytes;

    AVPixelFormat  pFormat = AV_PIX_FMT_BGR24;
    numBytes = av_image_get_buffer_size(pFormat,
                                        pCodecCtx->width,
                                        pCodecCtx->height,
                                        16);
    buffer = static_cast<uint8_t*>(av_malloc(static_cast<unsigned long>(numBytes)*sizeof(uint8_t)));
    av_image_fill_arrays(pFrameRGB->data,
                         pFrameRGB->linesize,
                         buffer,
                         pFormat,
                         pCodecCtx->width,
                         pCodecCtx->height,
                         1);

    int res;
    int frameFinished;
    AVPacket packet;
    int currentFrame = 0;
    bool fetchFrame = false;
    double fps = av_q2d(pFormatCtx->streams[videoStream]->r_frame_rate);
    double dur = static_cast<double>(pFormatCtx->duration)/AV_TIME_BASE;
    int maxFrame = qRound((dur*fps)/2);
    if (frame>maxFrame) { frame = maxFrame; }

    int64_t seekT = (int64_t(frame) *
                     pFormatCtx->streams[videoStream]->r_frame_rate.den *
                     pFormatCtx->streams[videoStream]->time_base.den) /
                     (int64_t(pFormatCtx->streams[videoStream]->r_frame_rate.num) *
                     pFormatCtx->streams[videoStream]->time_base.num);
    if (av_seek_frame(pFormatCtx, videoStream, seekT, AVSEEK_FLAG_FRAME) >= 0) {
              av_init_packet(&packet);
              currentFrame = frame;
    }
    while((res = av_read_frame(pFormatCtx,&packet)>=0)) {
        if (currentFrame>=frame) { fetchFrame = true; }
        if (packet.stream_index == videoStream){
            if (!fetchFrame) {
                currentFrame++;
                continue;
            }

            int ret = avcodec_send_packet(pCodecCtx, &packet);
            if (ret<0) { continue; }
            ret = avcodec_receive_frame(pCodecCtx, pFrame);
            if (ret>=0) { frameFinished = true; }
            else { continue; }

            if (frameFinished) {
                struct SwsContext * img_convert_ctx;
                img_convert_ctx = sws_getCachedContext(Q_NULLPTR,
                                                       pCodecCtx->width,
                                                       pCodecCtx->height,
                                                       pCodecCtx->pix_fmt,
                                                       pCodecCtx->width,
                                                       pCodecCtx->height,
                                                       AV_PIX_FMT_BGR24,
                                                       SWS_BICUBIC,
                                                       Q_NULLPTR,
                                                       Q_NULLPTR,
                                                       Q_NULLPTR);
                sws_scale(img_convert_ctx,
                          (dynamic_cast<AVFrame*>(pFrame)->data),
                          (dynamic_cast<AVFrame*>(pFrame)->linesize),
                          0,
                          pCodecCtx->height,
                          (dynamic_cast<AVFrame*>(pFrameRGB)->data),
                          (dynamic_cast<AVFrame*>(pFrameRGB)->linesize));
                try {
                    Magick::Image image(static_cast<size_t>(pFrame->width),
                                        static_cast<size_t>(pFrame->height),
                                        "BGR",
                                        Magick::CharPixel,
                                        pFrameRGB->data[0]);
                    result = image;
                }
                catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
                catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }

                av_packet_unref(&packet);
                sws_freeContext(img_convert_ctx);
                break;
            }
            currentFrame++;
        }
    }

    av_packet_unref(&packet);
    avcodec_close(pCodecCtx);
    av_free(pFrame);
    av_free(pFrameRGB);
    avformat_close_input(&pFormatCtx);

    return result;
}
#endif

