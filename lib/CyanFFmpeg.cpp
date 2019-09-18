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

#include "CyanFFmpeg.h"
#include <QDebug>

#ifdef WITH_FFMPEG
QByteArray CyanFFmpeg::getEmbeddedCoverArt(const QString &filename)
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

int CyanFFmpeg::getVideoMaxFrames(const QString &filename)
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

Magick::Image CyanFFmpeg::getVideoFrame(const QString &filename,
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
                catch(Magick::Warning &warn_ ) {
                    qDebug() << warn_.what();
                }

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
