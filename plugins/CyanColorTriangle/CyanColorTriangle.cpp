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

#include "CyanColorTriangle.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QDebug>

CyanColorTriangle::CyanColorTriangle(): QObject ()
, _widget(nullptr)
, _triangle(nullptr)
{
}

CyanColorTriangle::~CyanColorTriangle()
{
}

const QString CyanColorTriangle::uuid() const
{
    return CYAN_PLUGIN_UUID;
}

const QString CyanColorTriangle::title() const
{
    return CYAN_PLUGIN_TITLE;
}

const QString CyanColorTriangle::desc() const
{
    return CYAN_PLUGIN_DESC;
}

double CyanColorTriangle::version() const
{
    return QString(CYAN_PLUGIN_VERSION).toDouble();
}

int CyanColorTriangle::position() const
{
    return CyanWidgetPluginRightPosition;
}

void CyanColorTriangle::setCurrentColor(const QColor &color)
{
    if (_triangle->color() != color) {
        _triangle->setColor(color);
    }
}

QWidget *CyanColorTriangle::getWidget(QWidget *parent)
{
    _widget = new QWidget(parent);
    QVBoxLayout *widgetLayout = new QVBoxLayout(_widget);

    _triangle = new QtColorTriangle(_widget);
    connect(_triangle,
            SIGNAL(colorChanged(QColor)),
            this,
            SLOT(handleColorChanged(QColor)));
    widgetLayout->addWidget(_triangle);

    return _widget;
}

void CyanColorTriangle::handleColorChanged(const QColor &color)
{
    emit currentColorChanged(color);
}
