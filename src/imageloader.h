#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include <QThread>
#include <QString>
#include "FXX.h"

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject *parent = Q_NULLPTR);
    ~ImageLoader();

private:
    FXX fx;
    QThread t;

signals:
    void loadedImage(FXX::Image result);
    void convertedImage(FXX::Image result);

public slots:
    void requestImage(const QString &file);
    void requestConvert(const FXX::Image &image);

private slots:
    void readImage(const QString &file);
    void convertImage(const FXX::Image &image);
};

#endif // IMAGELOADER_H
