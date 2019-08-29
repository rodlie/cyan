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

#ifndef CYANWIDGETAPI_H
#define CYANWIDGETAPI_H

#define CYAN_WIDGET_API "net.fxarena.cyan.widget.plugin/2"

#include <QObject>

QT_BEGIN_NAMESPACE
class QString;
class QAction;
class QWidget;
QT_END_NAMESPACE

class CyanWidgetPlugin
{
public:
    virtual ~CyanWidgetPlugin() {}
    virtual const QString uuid() const = 0;
    virtual const QString title() const = 0;
    virtual const QString desc() const = 0;
    virtual double version() const = 0;
    virtual QList<QAction*> actions() const = 0;
    virtual QList<QAction*> options() const = 0;

public slots:
    void load();
    void unload();
    void save();
    void refresh();
    QWidget* widget();
    virtual void init() = 0;

signals:
    void statusMessage(const QString &text, int timeout);
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(CyanWidgetPlugin, CYAN_WIDGET_API)
QT_END_NAMESPACE

#endif // CYANWIDGETAPI_H
