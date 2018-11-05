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

void ImageLoader::requestImage(const QString &file)
{
    QMetaObject::invokeMethod(this, "readImage", Q_ARG(QString, file));
}

void ImageLoader::requestConvert(const FXX::Image &image)
{
    QMetaObject::invokeMethod(this, "convertImage", Q_ARG(FXX::Image, image));
}

void ImageLoader::readImage(const QString &file)
{
    FXX::Image imageData;
    if (!file.isEmpty()) {
        imageData = fx.readImage(file.toStdString());
    }
    emit loadedImage(imageData);
}

void ImageLoader::convertImage(const FXX::Image &image)
{
    emit convertedImage(fx.convertImage(image));
}
