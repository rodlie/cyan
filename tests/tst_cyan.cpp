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
    QFile testImage(":/Test_Out-of-Gamut_colors-en.tif");
    QVERIFY(testImage.open(QIODevice::ReadOnly));
    QByteArray result = testImage.readAll();
    testImage.close();
    QVERIFY(result.size()>0);
    image.imageBuffer = std::vector<unsigned char>(result.begin(), result.end());

    QFile testICC1(":/icc/rgb.icc");
    QVERIFY(testICC1.open(QIODevice::ReadOnly));
    QByteArray result2 = testICC1.readAll();
    testICC1.close();
    QVERIFY(result2.size()>0);
    image.iccRGB = std::vector<unsigned char>(result2.begin(), result2.end());

    QFile testICC2(":/icc/cmyk.icc");
    QVERIFY(testICC2.open(QIODevice::ReadOnly));
    QByteArray result3 = testICC2.readAll();
    testICC2.close();
    QVERIFY(result3.size()>0);
    image.iccCMYK = std::vector<unsigned char>(result3.begin(), result3.end());

    QFile testICC3(":/icc/gray.icc");
    QVERIFY(testICC3.open(QIODevice::ReadOnly));
    QByteArray result4 = testICC3.readAll();
    testICC3.close();
    QVERIFY(result4.size()>0);
    image.iccGRAY = std::vector<unsigned char>(result4.begin(), result4.end());

    QVERIFY(image.imageBuffer.size()>0);
    QVERIFY(image.iccRGB.size()>0);
    QVERIFY(image.iccCMYK.size()>0);
    QVERIFY(image.iccGRAY.size()>0);
}

Cyan::~Cyan()
{

}

void Cyan::test_case1()
{
    /*QVERIFY(fx.hasHDRI());
    QVERIFY(fx.hasJPEG());
    QVERIFY(fx.hasLCMS());
    QVERIFY(fx.hasPNG());
    QVERIFY(fx.hasTIFF());
    QVERIFY(fx.hasMP());*/
}

void Cyan::test_case2()
{
    QVERIFY(fx.getProfileColorspace(image.iccRGB) == FXX::RGBColorSpace);
    QVERIFY(fx.getProfileColorspace(image.iccCMYK) == FXX::CMYKColorSpace);
    QVERIFY(fx.getProfileColorspace(image.iccGRAY) == FXX::GRAYColorSpace);
    QVERIFY(fx.getProfileTag(image.iccRGB) == "sRGB (built-in)");
    QVERIFY(fx.getProfileTag(image.iccCMYK) == "ISO Coated v2 (built-in)");
    QVERIFY(fx.getProfileTag(image.iccGRAY) == "ISO Coated v2 - GREY 1c - (built-in)");
}

void Cyan::test_case3()
{

}

void Cyan::test_case4()
{

}

QTEST_APPLESS_MAIN(Cyan)

#include "tst_cyan.moc"
