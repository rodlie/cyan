/*
#
# Cyan API <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
*/

#ifndef CYANWIDGETAPI_H
#define CYANWIDGETAPI_H

#define CYAN_WIDGET_API "net.fxarena.cyan.plugin.widget.api/2"

#include <QObject>

QT_BEGIN_NAMESPACE
class QString;
class QWidget;
class QColor;
QT_END_NAMESPACE

#include "CyanGlobal.h"

class CYANSHARED_EXPORT CyanWidgetPlugin
{
public:
    enum CyanWidgetPluginPosition
    {
        CyanWidgetPluginLeftPosition,
        CyanWidgetPluginRightPosition,
        CyanWidgetPluginTopPosition,
        CyanWidgetPluginBottomPosition
    };
    virtual ~CyanWidgetPlugin() {}
    virtual const QString uuid() const = 0;
    virtual double version() const = 0;
    virtual int position() const = 0;
    virtual const QString title() const = 0;
    virtual const QString desc() const = 0;
    virtual bool isText() const = 0;

public slots:
    virtual void setCurrentColor(const QColor &color) = 0;
    virtual void setCurrentText(const QString &html,
                                int align,
                                int rotate,
                                bool enabled) = 0;
    virtual QWidget* getWidget(QWidget *parent) = 0;

signals:
    void statusMessage(const QString &text, int timeout);
    void currentColorChanged(const QColor &color);
    void currentTextChanged(const QString &html,
                            int align,
                            int rotate,
                            bool enabled);
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(CyanWidgetPlugin, CYAN_WIDGET_API)
QT_END_NAMESPACE

#endif // CYANWIDGETAPI_H
