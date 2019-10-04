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

#ifndef CYANWIDGETEXAMPLE_H
#define CYANWIDGETEXAMPLE_H

#include "CyanWidgetApi.h"

class CyanWidgetExample : public QObject, public CyanWidgetPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID CYAN_WIDGET_API)
    Q_INTERFACES(CyanWidgetPlugin)

public:
    CyanWidgetExample();
    ~CyanWidgetExample();
    const QString uuid() const;
    const QString title() const;
    const QString desc() const;
    double version() const;
    int position() const;
    bool isText() const;

public slots:
    void setCurrentColor(const QColor &color);
    void setCurrentText(const QString &html,
                        int align,
                        int rotate,
                        bool enabled)
    {
        Q_UNUSED(html)
        Q_UNUSED(align)
        Q_UNUSED(rotate)
        Q_UNUSED(enabled)
    }
    QWidget* getWidget(QWidget* parent);

private slots:

private:
    QWidget* _widget;

signals:
    void statusMessage(QString text, int timeout);
    void currentColorChanged(const QColor &color);
};

#endif // CYANWIDGETEXAMPLE_H
