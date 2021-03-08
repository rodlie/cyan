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

#ifndef CYANPLUGINS_H
#define CYANPLUGINS_H

#include <QObject>
#include <QString>
#include <QPluginLoader>
#include <QList>

#include "CyanWidgetApi.h"
#include "CyanEffectApi.h"

#include "CyanGlobal.h"

class CYANSHARED_EXPORT CyanPlugins : public QObject
{
    Q_OBJECT

public:
    explicit CyanPlugins(QObject *parent = nullptr);
    ~CyanPlugins();

private:
    QList<QPluginLoader*> plugins;

private slots:
    void scanPluginsFolder(const QString &folder,
                           bool recursive = false);
    void loadPlugin(QPluginLoader *loader);
    void unloadPlugin(QPluginLoader *loader);
    void addPlugin(QPluginLoader *loader);
    void removePlugin(const QString &uuid);
    void removePlugin(QPluginLoader *loader);
    QPluginLoader *getLoader(const QString &uuid);
    QPluginLoader *getLoader(int index);
    CyanWidgetPlugin *getWidgetPlugin(int index);
    CyanWidgetPlugin *getWidgetPlugin(QPluginLoader *loader);
    CyanEffectPlugin *getEffectPlugin(int index);
    CyanEffectPlugin *getEffectPlugin(QPluginLoader *loader);

public slots:
    void removePlugins();
    CyanWidgetPlugin *getWidgetPlugin(const QString &uuid);
    CyanEffectPlugin *getEffectPlugin(const QString &uuid);
    QList<QPluginLoader*> scan();
};

#endif // CYANPLUGINS_H
