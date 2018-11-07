#include "imageloader.h"

ImageLoader::ImageLoader(QObject *parent) : QObject(parent)
{
    moveToThread(&t);
    t.start();
}

ImageLoader::~ImageLoader()
{
    t.quit();
    t.wait();
}

void ImageLoader::requestImage(const QString &file, const FXX::Image &failsafe)
{
    QMetaObject::invokeMethod(this, "readImage",
                              Q_ARG(QString, file),
                              Q_ARG(FXX::Image, failsafe));
}

void ImageLoader::requestConvert(const FXX::Image &image)
{
    QMetaObject::invokeMethod(this, "convertImage", Q_ARG(FXX::Image, image));
}

void ImageLoader::readImage(const QString &file, const FXX::Image &failsafe)
{
    FXX::Image imageData;
    if (!file.isEmpty()) {
        //std::vector<unsigned char> profile(failsafe.begin(), failsafe.end());
        imageData = fx.readImage(file.toStdString(), failsafe);
    }
    emit loadedImage(imageData);
}

void ImageLoader::convertImage(const FXX::Image &image)
{
    emit convertedImage(fx.convertImage(image));
}
