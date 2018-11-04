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

void ImageLoader::readImage(const QString &file)
{
    FXX::Image imageData;
    if (!file.isEmpty()) {
        imageData = fx.readImage(file.toStdString());
    }
    emit loadedImage(imageData);
}
