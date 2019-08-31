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

#include "CyanWidgetExample.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

CyanWidgetExample::CyanWidgetExample(): QObject ()
, _widget(nullptr)
{
    qDebug() << "==> Example Plugin load" << uuid() << version();
}

CyanWidgetExample::~CyanWidgetExample()
{
    qDebug() << "==> Example Plugin unload" << uuid() << version();
}

const QString CyanWidgetExample::uuid() const
{
    return CYAN_PLUGIN_UUID;
}

const QString CyanWidgetExample::title() const
{
    return CYAN_PLUGIN_TITLE;
}

const QString CyanWidgetExample::desc() const
{
    return CYAN_PLUGIN_DESC;
}

double CyanWidgetExample::version() const
{
    return QString(CYAN_PLUGIN_VERSION).toDouble();
}

int CyanWidgetExample::position() const
{
    return CyanWidgetPluginLeftPosition;
}

void CyanWidgetExample::setCurrentColor(const QColor &color)
{
    _widget->setStyleSheet(QString("background: rgb(%1, %2, %3);")
                           .arg(color.red())
                           .arg(color.green())
                           .arg(color.blue()));
}

QWidget *CyanWidgetExample::getWidget(QWidget *parent)
{
    _widget = new QWidget(parent);
    _widget->setMinimumSize(QSize(200,200));
    QLabel *label = new QLabel(_widget);
    label->setText(tr("Cyan Widget Example Plugin"));
    QVBoxLayout *widgetLayout = new QVBoxLayout(_widget);
    widgetLayout->addWidget(label);
    return _widget;
}
