/*
# Copyright Ole-André Rodlie, INRIA.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
*/

#include <QtTest>
#include <QFile>
#include <QDebug>

#include "FXX.h"
#include "Magick++.h"

class Cyan : public QObject
{
    Q_OBJECT

public:
    Cyan();
    ~Cyan();

private:
    FXX fx;
    FXX::Image image;
    std::vector<unsigned char> sampleCMYK;
    std::vector<unsigned char> sampleGRAY;
    bool compareImages(std::vector<unsigned char> image1,
                       std::vector<unsigned char> image2);

private slots:
    void test_case1();
    void test_case2();
    void test_case3();
    void test_case4();
};

Cyan::Cyan()
{

}

Cyan::~Cyan()
{

}

bool Cyan::compareImages(std::vector<unsigned char> image1,
                         std::vector<unsigned char>image2)
{
    Magick::Blob blob1(image1.data(), image1.size());
    Magick::Blob blob2(image2.data(), image2.size());
    Magick::Image src,dst;
    src.quiet(true);
    dst.quiet(true);
    try {
        src.read(blob1);
        dst.read(blob2);
    }
    catch(Magick::Error &error_ ) {
        qDebug() << error_.what();
        return false;
    }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
    try {
        if (!src.compare(dst)) {
            double distortion = 0.0;
            Magick::Image errorImage = src.compare(dst,
                                                  MagickCore::AbsoluteErrorMetric,
                                                  &distortion);
            errorImage.quiet(true);
            errorImage.write("compare.jpg");
            src.write("src.tif");
            dst.write("dst.tif");
        } else { // images are identical
            return true;
        }
    }
    catch(Magick::Error &error_ ) {
        qDebug() << error_.what();
    }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
    return false;
}

void Cyan::test_case1()
{
    qDebug() << "Loading original sample image ...";
    QFile testImage(":/Test_Out-of-Gamut_colors-en.tif");
    QVERIFY(testImage.open(QIODevice::ReadOnly));
    QByteArray result = testImage.readAll();
    testImage.close();
    QVERIFY(result.size()>0);
    image.imageBuffer = std::vector<unsigned char>(result.begin(),
                                                   result.end());

    qDebug() << "Extract embedded color profile from sample image ...";
    Magick::Blob magickBlob(result.data(), static_cast<size_t>(result.length()));
    QVERIFY(magickBlob.length()>0);
    Magick::Image magickImage;
    magickImage.quiet(true);
    try {
        magickImage.read(magickBlob);
    }
    catch(Magick::Error &error_ ) {
        qDebug() << error_.what();
        QVERIFY(false);
    }
    catch(Magick::Warning &warn_ ) {
        qDebug() << warn_.what();
    }
    QVERIFY(magickImage.iccColorProfile().length()>0);
    unsigned char *iccBuffer = reinterpret_cast<unsigned char*>(const_cast<void*>(magickImage.iccColorProfile().data()));
    std::vector<unsigned char> iccData(iccBuffer,
                                       iccBuffer + magickImage.iccColorProfile().length());
    image.iccInputBuffer = iccData;
    QVERIFY(image.iccInputBuffer.size()>0);
    QVERIFY(fx.getProfileTag(image.iccInputBuffer) == "Adobe RGB (1998)");

    qDebug() << "Loading CMYK sample image ...";
    QFile sampleCMYKFile(":/sample-CMYK.tif");
    QVERIFY(sampleCMYKFile.open(QIODevice::ReadOnly));
    QByteArray sampleCMYKResult = sampleCMYKFile.readAll();
    sampleCMYKFile.close();
    QVERIFY(sampleCMYKResult.size()>0);
    sampleCMYK = std::vector<unsigned char>(sampleCMYKResult.begin(),
                                            sampleCMYKResult.end());

    qDebug() << "Loading GRAY sample image ...";
    QFile sampleGRAYFile(":/sample-GRAY.tif");
    QVERIFY(sampleGRAYFile.open(QIODevice::ReadOnly));
    QByteArray sampleGRAYResult = sampleGRAYFile.readAll();
    sampleGRAYFile.close();
    QVERIFY(sampleGRAYResult.size()>0);
    sampleGRAY = std::vector<unsigned char>(sampleGRAYResult.begin(),
                                            sampleGRAYResult.end());

    qDebug() << "Loading RGB sample profile ...";
    QFile testICC1(":/icc/rgb.icc");
    QVERIFY(testICC1.open(QIODevice::ReadOnly));
    QByteArray result2 = testICC1.readAll();
    testICC1.close();
    QVERIFY(result2.size()>0);
    image.iccRGB = std::vector<unsigned char>(result2.begin(),
                                              result2.end());

    qDebug() << "Loading CMYK sample profile ...";
    QFile testICC2(":/icc/cmyk.icc");
    QVERIFY(testICC2.open(QIODevice::ReadOnly));
    QByteArray result3 = testICC2.readAll();
    testICC2.close();
    QVERIFY(result3.size()>0);
    image.iccCMYK = std::vector<unsigned char>(result3.begin(),
                                               result3.end());

    qDebug() << "Loading GRAY sample profile ...";
    QFile testICC3(":/icc/gray.icc");
    QVERIFY(testICC3.open(QIODevice::ReadOnly));
    QByteArray result4 = testICC3.readAll();
    testICC3.close();
    QVERIFY(result4.size()>0);
    image.iccGRAY = std::vector<unsigned char>(result4.begin(),
                                               result4.end());
}

void Cyan::test_case2()
{
    qDebug() << "Checking for JPEG support ...";
    QVERIFY(fx.hasJPEG());
    qDebug() << "Checking for PNG support ...";
    QVERIFY(fx.hasPNG());
    qDebug() << "Checking for TIFF support ...";
    QVERIFY(fx.hasTIFF());
    qDebug() << "Checking for LCMS support ...";
    QVERIFY(fx.hasLCMS());
    qDebug() << "Checking for HDRI support ...";
    QVERIFY(fx.hasHDRI());
    qDebug() << "Checking for OpenMP support ...";
    QVERIFY(fx.hasMP());
    qDebug() << "Checking for Quantum depth ...";
    QVERIFY(fx.supportedQuantumDepth() == "Q16" || fx.supportedQuantumDepth() == "Q32");
}

void Cyan::test_case3()
{
    qDebug() << "Checking profiles color space ...";
    QVERIFY(fx.getProfileColorspace(image.iccRGB) == FXX::RGBColorSpace);
    QVERIFY(fx.getProfileColorspace(image.iccCMYK) == FXX::CMYKColorSpace);
    QVERIFY(fx.getProfileColorspace(image.iccGRAY) == FXX::GRAYColorSpace);

    qDebug() << "Checking profiles tags ...";
    QVERIFY(fx.getProfileTag(image.iccRGB) == "sRGB (built-in)");
    QVERIFY(fx.getProfileTag(image.iccRGB,
                             FXX::ICCCopyright) == "This profile is free of known copyright restrictions");
    QVERIFY(fx.getProfileTag(image.iccRGB,
                             FXX::ICCModel) == "This general purpose profile was designed by Hewlett-Packard and Microsoft and lives on as the default profile on the Internet for untagged RGB colors and used in HDTV. Most uncalibrated displays are able to display most of the colors available in sRGB, although this profile is sometimes a poor choice for printing.");
    QVERIFY(fx.getProfileTag(image.iccRGB,
                             FXX::ICCManufacturer).empty());

    QVERIFY(fx.getProfileTag(image.iccCMYK) == "ISO Coated v2 (built-in)");
    QVERIFY(fx.getProfileTag(image.iccCMYK,
                             FXX::ICCCopyright) == "basICColor CMYKick v1.2 - Copyright (c) 2006-2007 Color Solutions, All Rights Reserved.");

    QVERIFY(fx.getProfileTag(image.iccCMYK,
                             FXX::ICCManufacturer).empty());
    QVERIFY(fx.getProfileTag(image.iccCMYK,
                             FXX::ICCModel).empty());

    QVERIFY(fx.getProfileTag(image.iccGRAY) == "ISO Coated v2 - GREY 1c - (built-in)");
    QVERIFY(fx.getProfileTag(image.iccGRAY,
                             FXX::ICCCopyright) == "basICColor - Copyright (c) 2007 Color Solutions, All Rights Reserved.");

    QVERIFY(fx.getProfileTag(image.iccGRAY,
                             FXX::ICCManufacturer).empty());
    QVERIFY(fx.getProfileTag(image.iccGRAY,
                             FXX::ICCModel).empty());
}

void Cyan::test_case4()
{
    qDebug() << "Converting RGB to CMYK ...";
    FXX::Image convertCMYK;
    convertCMYK.imageBuffer = image.imageBuffer;
    convertCMYK.iccInputBuffer = image.iccInputBuffer;
    convertCMYK.iccOutputBuffer = image.iccCMYK;
    convertCMYK.blackpoint = true;
    convertCMYK.intent = FXX::PerceptualRenderingIntent;
    FXX::Image resultCMYK = fx.convertImage(convertCMYK, false);
    QVERIFY(resultCMYK.imageBuffer.size()>0);
    QVERIFY(compareImages(sampleCMYK, resultCMYK.imageBuffer));

    qDebug() << "Converting CMYK to RGB ...";
    FXX::Image sampleCMYK2RGB;
    sampleCMYK2RGB.imageBuffer = sampleCMYK;
    sampleCMYK2RGB.iccInputBuffer = image.iccCMYK;
    sampleCMYK2RGB.iccOutputBuffer = image.iccRGB;
    sampleCMYK2RGB.blackpoint = true;
    sampleCMYK2RGB.intent = FXX::PerceptualRenderingIntent;
    FXX::Image sampleCMYK2RGBResult = fx.convertImage(sampleCMYK2RGB, false);

    FXX::Image resultCMYK2RGB;
    resultCMYK2RGB.imageBuffer = resultCMYK.imageBuffer;
    resultCMYK2RGB.iccInputBuffer = image.iccCMYK;
    resultCMYK2RGB.iccOutputBuffer = image.iccRGB;
    resultCMYK2RGB.blackpoint = true;
    resultCMYK2RGB.intent = FXX::PerceptualRenderingIntent;
    FXX::Image resultCMYK2RGBResult = fx.convertImage(resultCMYK2RGB, false);

    QVERIFY(compareImages(sampleCMYK2RGBResult.imageBuffer, resultCMYK2RGBResult.imageBuffer));

    qDebug() << "Converting RGB to GRAY ...";
    FXX::Image convertGRAY;
    convertGRAY.imageBuffer = image.imageBuffer;
    convertGRAY.iccInputBuffer = image.iccInputBuffer;
    convertGRAY.iccOutputBuffer = image.iccGRAY;
    convertGRAY.blackpoint = true;
    convertGRAY.intent = FXX::PerceptualRenderingIntent;
    FXX::Image resultGRAY = fx.convertImage(convertGRAY, false);
    QVERIFY(resultGRAY.imageBuffer.size()>0);
    QVERIFY(compareImages(sampleGRAY, resultGRAY.imageBuffer));
}

QTEST_APPLESS_MAIN(Cyan)

#include "tst_cyan.moc"
