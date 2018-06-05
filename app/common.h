/*
#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
#
*/

#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QMap>
#include <Magick++.h>
#include <QDateTime>

typedef QMap<int, Magick::Image> layersMap;
typedef QMap<int, Magick::CompositeOperator> compMap;
typedef QMap<int, QSize> posMap;
typedef QMap<int, bool> visibilityMap;

class Common
{
public:
    static QString timestamp()
    {
        QDateTime date;
        return date.currentDateTime().toString("yyyyMMddHHmmsszzz");
    }
    static QMap<Magick::CompositeOperator, QString> compositeModes()
    {
        QMap<Magick::CompositeOperator, QString> result;

        result[MagickCore::OverCompositeOp] = QObject::tr("Over");
        result[MagickCore::OverlayCompositeOp] = QObject::tr("Overlay");
        result[MagickCore::ScreenCompositeOp] = QObject::tr("Screen");
        result[MagickCore::AtopCompositeOp] = QObject::tr("Atop");
        result[MagickCore::DissolveCompositeOp] = QObject::tr("Dissolve");
        result[MagickCore::DistortCompositeOp] = QObject::tr("Distort");
        result[MagickCore::MultiplyCompositeOp] = QObject::tr("Multiply");
        result[MagickCore::AddCompositeOp] = QObject::tr("Add");
        result[MagickCore::PlusCompositeOp] = QObject::tr("Plus");
        result[MagickCore::MinusCompositeOp] = QObject::tr("Minus");
        result[MagickCore::DivideCompositeOp] = QObject::tr("Divide");
        result[MagickCore::SubtractCompositeOp] = QObject::tr("Subtract");
        result[MagickCore::DifferenceCompositeOp] = QObject::tr("Difference");
        result[MagickCore::XorCompositeOp] = QObject::tr("Xor");
        result[MagickCore::BlendCompositeOp] = QObject::tr("Blend");
        result[MagickCore::ColorBurnCompositeOp] = QObject::tr("Color Burn");
        result[MagickCore::ColorDodgeCompositeOp] = QObject::tr("Color Dodge");
        result[MagickCore::HardLightCompositeOp] = QObject::tr("Hard Light");
        result[MagickCore::LightenCompositeOp] = QObject::tr("Lighten");
        result[MagickCore::LightenIntensityCompositeOp] = QObject::tr("Lighten Intensity");
        result[MagickCore::DarkenCompositeOp] = QObject::tr("Darken");
        result[MagickCore::DarkenIntensityCompositeOp] = QObject::tr("Darken Intensity");
        result[MagickCore::VividLightCompositeOp] = QObject::tr("Vivid Light");
        result[MagickCore::PinLightCompositeOp] = QObject::tr("Pin Light");
        result[MagickCore::SoftLightCompositeOp] = QObject::tr("Soft Light");
        result[MagickCore::PegtopLightCompositeOp] = QObject::tr("Pegtop Light");

        /*
        MagickCore::ModulusAddCompositeOp;
        MagickCore::HueCompositeOp;
        MagickCore::MinusDstCompositeOp;
        MagickCore::SaturateCompositeOp;
        MagickCore::MinusSrcCompositeOp;
        MagickCore::DivideSrcCompositeOp;
        MagickCore::MathematicsCompositeOp;
        MagickCore::LinearBurnCompositeOp;
        MagickCore::LinearDodgeCompositeOp;
        MagickCore::BlurCompositeOp;
        MagickCore::DivideDstCompositeOp;
        MagickCore::ThresholdCompositeOp;
        MagickCore::ModulusSubtractCompositeOp;
        MagickCore::SrcOverCompositeOp;
        MagickCore::SrcOutCompositeOp;
        MagickCore::SrcInCompositeOp;
        MagickCore::SrcCompositeOp;
        MagickCore::SrcAtopCompositeOp;
        MagickCore::ReplaceCompositeOp;
        MagickCore::OutCompositeOp;
        MagickCore::ModulateCompositeOp;
        MagickCore::LuminizeCompositeOp;
        MagickCore::LinearLightCompositeOp;
        MagickCore::InCompositeOp;
        MagickCore::HardMixCompositeOp;
        MagickCore::ExclusionCompositeOp;
        MagickCore::DisplaceCompositeOp;
        MagickCore::DstCompositeOp
        MagickCore::DstOverCompositeOp;
        MagickCore::DstOutCompositeOp;
        MagickCore::DstInCompositeOp;
        MagickCore::DstAtopCompositeOp;
        */

        return result;
    }
};

#endif // COMMON_H
