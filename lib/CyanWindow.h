#ifndef CYANWINDOW_H
#define CYANWINDOW_H

#include <QObject>

class CyanWindow : public QObject
{
    Q_OBJECT
public:
    explicit CyanWindow(QObject *parent = nullptr);

signals:

};

#endif // CYANWINDOW_H
