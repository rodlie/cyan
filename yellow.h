/*
* Cyan <https://github.com/olear/cyan>,
* Copyright (C) 2016 Ole-André Rodlie
*
* Cyan is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as published
* by the Free Software Foundation.
*
* Cyan is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
*/

#ifndef YELLOW_H
#define YELLOW_H

#include <QObject>
#include <lcms2.h>
#include <QStringList>
#include <QByteArray>

class Yellow : public QObject
{
    Q_OBJECT
public:
    explicit Yellow(QObject *parent = 0);
    ~Yellow();

public slots:
    QByteArray profileDefault(int colorspace);
    QString profileDescFromFile(QString file);
    QString profileDescFromData(QByteArray data);
    int profileColorSpaceFromFile(QString file);
    int profileColorSpaceFromData(QByteArray data);
    QStringList genProfiles(int colorspace);
};

#endif // YELLOW_H
