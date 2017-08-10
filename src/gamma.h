#ifndef GAMMA_H
#define GAMMA_H

#include <QObject>

class Gamma : public QObject
{
    Q_OBJECT

public:
    explicit Gamma(QObject *parent = 0);
    bool clear();
    bool apply(QString filename);

signals:

public slots:
};

#endif // GAMMA_H
