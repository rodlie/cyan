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

#ifndef CYANEFFECTAPI_H
#define CYANEFFECTAPI_H

#define CYAN_EFFECT_API "net.fxarena.cyan.plugin.effect.api/2"

#include <QObject>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

#include "CyanGlobal.h"
#include "CyanImageFormat.h"

class CYANSHARED_EXPORT CyanEffectPlugin
{
public:
    enum CyanEffectKnob
    {
        CyanEffectKnobSliderInt,
        CyanEffectKnobSliderDouble,
        CyanEffectKnobInt,
        CyanEffectKnobDouble,
        CyanEffectKnobComboBox,
        CyanEffectKnobText
    };
    struct CyanEffectOption
    {
        CyanEffectKnob valueKnob;
        QString valueName;
        QVariant valueMin;
        QVariant valueMax;
        QVariant valueDefault;
        QVector<QVariant> valueList;
    };
    virtual ~CyanEffectPlugin() {}
    virtual const QString uuid() const = 0;
    virtual double version() const = 0;
    virtual const QString title() const = 0;
    virtual const QString desc() const = 0;
    virtual QVector<CyanEffectOption> options() const = 0;

public slots:
    // just testing
    virtual Magick::Image render(Magick::Image image,
                                 CyanImageFormat::CyanEffect options) = 0;

signals:
    void statusMessage(const QString &text, int timeout);
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(CyanEffectPlugin, CYAN_EFFECT_API)
QT_END_NAMESPACE

#endif // CYANEFFECTAPI_H
