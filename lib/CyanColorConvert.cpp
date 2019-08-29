/*
#
# Cyan Library <https://cyan.fxarena.net>
#
# Copyright Ole-André Rodlie, FxArena DA.
#
# ole.andre.rodlie@gmail.com / support@fxarena.net
#
# This software is governed either by the CeCILL or the CeCILL-C license
# under French law and abiding by the rules of distribution of free software.
# You can  use, modify and or redistribute the software under the terms of
# the CeCILL or CeCILL-C licenses as circulated by CEA, CNRS and INRIA
# at the following URL: "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL and CeCILL-C licenses and that you accept its terms.
#
*/

#include "CyanColorConvert.h"

#include <QDebug>

ColorConvert::ColorConvert(QObject *parent) : QObject (parent)
{

}

Magick::Image ColorConvert::convertColorspace(Magick::Image image,
                                              Magick::Blob input,
                                              Magick::Blob output,
                                              Magick::RenderingIntent intent,
                                              bool blackpoint)
{
    if (output.length()>0) {
        try {
            image.quiet(true);
            image.renderingIntent(intent);
            image.blackPointCompensation(blackpoint);
            if (input.length()>0) { image.profile("ICC", input); }
            image.profile("ICC", output);
        }
        catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
        catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    }
    return image;
}

Magick::Image ColorConvert::convertColorspace(Magick::Image image,
                                              const QString &input,
                                              const QString &output,
                                              Magick::RenderingIntent intent,
                                              bool blackpoint)
{
    Magick::Blob blob1,blob2;
    Magick::Image readBlob1,readBlob2;
    try {
        readBlob1.read(input.toStdString());
        readBlob1.write(&blob1);
        readBlob2.read(output.toStdString());
        readBlob2.write(&blob2);
        if (blob1.length()>0 && blob2.length()>0) {
            return convertColorspace(image,
                                     blob1,
                                     blob2,
                                     intent,
                                     blackpoint);
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return Magick::Image();
}

Magick::Image ColorConvert::convertColorspace(Magick::Image image,
                                              Magick::Blob input,
                                              const QString &output,
                                              Magick::RenderingIntent intent,
                                              bool blackpoint)
{
    Magick::Blob blob;
    Magick::Image readBlob;
    try {
        readBlob.read(output.toStdString());
        readBlob.write(&blob);
        if (blob.length()>0) {
            return convertColorspace(image,
                                     input,
                                     blob,
                                     intent,
                                     blackpoint);
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return Magick::Image();
}

Magick::Image ColorConvert::convertColorspace(Magick::Image image,
                                              const QString &input,
                                              Magick::Blob output,
                                              Magick::RenderingIntent intent,
                                              bool blackpoint)
{
    Magick::Blob blob;
    Magick::Image readBlob;
    try {
        readBlob.read(input.toStdString());
        readBlob.write(&blob);
        if (blob.length()>0) {
            return convertColorspace(image,
                                     blob,
                                     output,
                                     intent,
                                     blackpoint);
        }
    }
    catch(Magick::Error &error_ ) { qWarning() << error_.what(); }
    catch(Magick::Warning &warn_ ) { qWarning() << warn_.what(); }
    return Magick::Image();
}