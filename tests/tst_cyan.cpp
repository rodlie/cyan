/*
# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie<ole.andre.rodlie@gmail.com>
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
*/

#include <QtTest>
#include <QFile>
#include <QDebug>

#include "FXX.h"

class Cyan : public QObject
{
    Q_OBJECT

public:
    Cyan();
    ~Cyan();

private:
    FXX fx;
    FXX::Image image;

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

void Cyan::test_case1()
{
    qDebug() << "Loading sample image ...";
    QFile testImage(":/Test_Out-of-Gamut_colors-en.tif");
    QVERIFY(testImage.open(QIODevice::ReadOnly));
    QByteArray result = testImage.readAll();
    testImage.close();
    QVERIFY(result.size()>0);
    image.imageBuffer = std::vector<unsigned char>(result.begin(), result.end());

    qDebug() << "Loading RGB sample profile ...";
    QFile testICC1(":/icc/rgb.icc");
    QVERIFY(testICC1.open(QIODevice::ReadOnly));
    QByteArray result2 = testICC1.readAll();
    testICC1.close();
    QVERIFY(result2.size()>0);
    image.iccRGB = std::vector<unsigned char>(result2.begin(), result2.end());

    qDebug() << "Loading CMYK sample profile ...";
    QFile testICC2(":/icc/cmyk.icc");
    QVERIFY(testICC2.open(QIODevice::ReadOnly));
    QByteArray result3 = testICC2.readAll();
    testICC2.close();
    QVERIFY(result3.size()>0);
    image.iccCMYK = std::vector<unsigned char>(result3.begin(), result3.end());

    qDebug() << "Loading GRAY sample profile ...";
    QFile testICC3(":/icc/gray.icc");
    QVERIFY(testICC3.open(QIODevice::ReadOnly));
    QByteArray result4 = testICC3.readAll();
    testICC3.close();
    QVERIFY(result4.size()>0);
    image.iccGRAY = std::vector<unsigned char>(result4.begin(), result4.end());

    qDebug() << "Checking loaded assets ...";
    QVERIFY(image.imageBuffer.size()>0);
    QVERIFY(image.iccRGB.size()>0);
    QVERIFY(image.iccCMYK.size()>0);
    QVERIFY(image.iccGRAY.size()>0);
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
    qDebug() << "Converting RGB to GRAY ...";
    qDebug() << "Converting RGB to RGB ...";
}

QTEST_APPLESS_MAIN(Cyan)

#include "tst_cyan.moc"
