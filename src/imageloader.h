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

public slots:
    void requestImage(const QString &file);

private slots:
    void readImage(const QString &file);
};

#endif // IMAGELOADER_H
