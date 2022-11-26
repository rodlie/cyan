/*
#
# Cyan - https://github.com/rodlie/cyan
#
# Copyright (c) Ole-André Rodlie <ole.andre.rodlie@gmail.com>. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

#include <QtTest>
#include <QDebug>
#include <QByteArray>
#include <iostream>

#include "engine.h"
#include "Magick++.h"

class Test : public QObject
{
    Q_OBJECT

public:
    Test();
    ~Test();

private:
    struct TestFiles {
        QByteArray sampleRGB;
        QByteArray sampleProfile;
        QByteArray sampleCMYK;
        QByteArray sampleGRAY;
        QByteArray profileRGB;
        QByteArray profileCMYK;
        QByteArray profileGRAY;
    };
    TestFiles _files;

private slots:
    void test_case1();
    void test_case2();
    void test_case3();
    void test_case4();
    void test_case5();
};

Test::Test()
{
    Magick::InitializeMagick(nullptr);
}

Test::~Test()
{

}

void Test::test_case1()
{
    std::cout << "Loading original sample image ..." << std::endl;
    _files.sampleRGB = Cyan::Engine::fileToByteArray(":/Test_Out-of-Gamut_colors-en.tif");
    QVERIFY(_files.sampleRGB.size()>0);

    std::cout << "Extract embedded color profile from sample image ..." << std::endl;
    Magick::Blob magickBlob(_files.sampleRGB.data(), static_cast<size_t>(_files.sampleRGB.length()));
    QVERIFY(magickBlob.length()>0);
    Magick::Image magickImage;
    magickImage.quiet(true);
    try {
        magickImage.read(magickBlob);
    }
    catch(Magick::Error &error) {
        std::cout << error.what() << std::endl;
        QVERIFY(false);
    }
    catch(Magick::Warning &warn) {
        std::cout << warn.what() << std::endl;
    }
    QVERIFY(magickImage.iccColorProfile().length()>0);
    _files.sampleProfile = QByteArray((char*)magickImage.iccColorProfile().data(),
                                      magickImage.iccColorProfile().length());
    QVERIFY(_files.sampleProfile.size()>0);
    QVERIFY(Cyan::Engine::getProfileTag(_files.sampleProfile) == "Adobe RGB (1998)");

    std::cout << "Loading CMYK sample image ..." << std::endl;
    _files.sampleCMYK = Cyan::Engine::fileToByteArray(":/sample-CMYK.tif");
    QVERIFY(_files.sampleCMYK.size()>0);

    std::cout << "Loading GRAY sample image ..." << std::endl;
    _files.sampleGRAY = Cyan::Engine::fileToByteArray(":/sample-GRAY.tif");
    QVERIFY(_files.sampleGRAY.size()>0);

    std::cout << "Loading RGB sample profile ..." << std::endl;
    _files.profileRGB = Cyan::Engine::fileToByteArray(":/icc/rgb.icc");
    QVERIFY(_files.profileRGB.size()>0);

    std::cout << "Loading CMYK sample profile ..." << std::endl;
    _files.profileCMYK = Cyan::Engine::fileToByteArray(":/icc/cmyk.icc");
    QVERIFY(_files.profileCMYK.size()>0);

    std::cout << "Loading GRAY sample profile ..." << std::endl;
    _files.profileGRAY = Cyan::Engine::fileToByteArray(":/icc/gray.icc");
    QVERIFY(_files.profileGRAY.size()>0);
}

void Test::test_case2()
{
    std::cout << "Checking for JPEG support ..." << std::endl;
    QVERIFY(Cyan::Engine::hasJPEG());
    std::cout << "Checking for PNG support ..." << std::endl;
    QVERIFY(Cyan::Engine::hasPNG());
    std::cout << "Checking for TIFF support ..." << std::endl;
    QVERIFY(Cyan::Engine::hasTIFF());
    std::cout << "Checking for LCMS support ..." << std::endl;
    QVERIFY(Cyan::Engine::hasLCMS());
    //std::cout << "Checking for HDRI support ..." << std::endl;
    //QVERIFY(Cyan::Engine::hasHDRI());
    //std::cout << "Checking for OpenMP support ..." << std::endl;
    //QVERIFY(Cyan::Engine::hasMP());
    std::cout << "Checking for Quantum depth ..." << std::endl;
    QVERIFY(Cyan::Engine::supportedQuantumDepth() == "Q16" || Cyan::Engine::supportedQuantumDepth() == "Q32");
}

void Test::test_case3()
{
    std::cout << "Checking profiles color space ..." << std::endl;
    QVERIFY(Cyan::Engine::getFileColorspace(_files.profileRGB) == Cyan::Engine::colorSpaceRGB);
    QVERIFY(Cyan::Engine::getFileColorspace(_files.profileCMYK) == Cyan::Engine::colorSpaceCMYK);
    QVERIFY(Cyan::Engine::getFileColorspace(_files.profileGRAY) == Cyan::Engine::colorSpaceGRAY);

    std::cout << "Checking profiles tags ..." << std::endl;
    QVERIFY(Cyan::Engine::getProfileTag(_files.profileRGB) == "sRGB (built-in)");
    QVERIFY(Cyan::Engine::getProfileTag(_files.profileRGB,
                                        Cyan::Engine::ICCCopyright) == "This profile is free of known copyright restrictions");
    QVERIFY(Cyan::Engine::getProfileTag(_files.profileRGB,
                                        Cyan::Engine::ICCModel) == "This general purpose profile was designed by Hewlett-Packard and Microsoft and lives on as the default profile on the Internet for untagged RGB colors and used in HDTV. Most uncalibrated displays are able to display most of the colors available in sRGB, although this profile is sometimes a poor choice for printing.");
    QVERIFY(Cyan::Engine::getProfileTag(_files.profileRGB,
                                        Cyan::Engine::ICCManufacturer).isEmpty());

    QVERIFY(Cyan::Engine::getProfileTag(_files.profileCMYK) == "ISO Coated v2 (built-in)");
    QVERIFY(Cyan::Engine::getProfileTag(_files.profileCMYK,
                                        Cyan::Engine::ICCCopyright) == "basICColor CMYKick v1.2 - Copyright (c) 2006-2007 Color Solutions, All Rights Reserved.");

    QVERIFY(Cyan::Engine::getProfileTag(_files.profileCMYK,
                                        Cyan::Engine::ICCManufacturer).isEmpty());
    QVERIFY(Cyan::Engine::getProfileTag(_files.profileCMYK,
                                        Cyan::Engine::ICCModel).isEmpty());

    QVERIFY(Cyan::Engine::getProfileTag(_files.profileGRAY) == "ISO Coated v2 - GREY 1c - (built-in)");
    QVERIFY(Cyan::Engine::getProfileTag(_files.profileGRAY,
                                        Cyan::Engine::ICCCopyright) == "basICColor - Copyright (c) 2007 Color Solutions, All Rights Reserved.");

    QVERIFY(Cyan::Engine::getProfileTag(_files.profileGRAY,
                                        Cyan::Engine::ICCManufacturer).isEmpty());
    QVERIFY(Cyan::Engine::getProfileTag(_files.profileGRAY,
                                        Cyan::Engine::ICCModel).isEmpty());
}

void Test::test_case4()
{
    std::cout << "Converting RGB to CMYK ..." << std::endl;
    auto convertedCMYK = Cyan::Engine::convertImage(_files.sampleRGB,
                                                    _files.profileRGB,
                                                    _files.profileCMYK);
    QVERIFY(Cyan::Engine::compareImages(convertedCMYK.buffer,
                                        _files.sampleCMYK));
    std::cout << Cyan::Engine::identify(convertedCMYK.buffer).toStdString() << std::endl;

    std::cout << "Converting CMYK to RGB ..." << std::endl;
    auto convertedCMYK2RGB = Cyan::Engine::convertImage(_files.sampleCMYK,
                                                        _files.profileCMYK,
                                                        _files.profileRGB);
    auto resultCMYK2RGB = Cyan::Engine::convertImage(convertedCMYK.buffer,
                                                     _files.profileCMYK,
                                                     _files.profileRGB);
    QVERIFY(Cyan::Engine::compareImages(convertedCMYK2RGB.buffer,
                                        resultCMYK2RGB.buffer));
    std::cout << Cyan::Engine::identify(convertedCMYK2RGB.buffer).toStdString() << std::endl;

    std::cout << "Converting RGB to GRAY ..." << std::endl;
    auto convertedGRAY = Cyan::Engine::convertImage(_files.sampleRGB,
                                                    _files.profileRGB,
                                                    _files.profileGRAY);
    QVERIFY(Cyan::Engine::compareImages(convertedGRAY.buffer,
                                        _files.sampleGRAY));
    std::cout << Cyan::Engine::identify(convertedGRAY.buffer).toStdString() << std::endl;
}

void Test::test_case5()
{
    std::cout << "Color profiles search paths:" << std::endl;
    for (auto &path : Cyan::Engine::getProfiles(Cyan::Engine::colorSpaceRGB, true)) {
        std::cout << "   * " << path.toStdString() << std::endl;
    }
    std::cout << "Color profiles RGB:" << std::endl;
    for (auto &profile : Cyan::Engine::getProfiles(Cyan::Engine::colorSpaceRGB).toStdMap()) {
        std::cout << "   * " << profile.first.toStdString() << " : " << profile.second.toStdString() << std::endl;
    }
    std::cout << "Color profiles CMYK:" << std::endl;
    for (auto &profile : Cyan::Engine::getProfiles(Cyan::Engine::colorSpaceCMYK).toStdMap()) {
        std::cout << "   * " << profile.first.toStdString() << " : " << profile.second.toStdString() << std::endl;
    }
    std::cout << "Color profiles GRAY:" << std::endl;
    for (auto &profile : Cyan::Engine::getProfiles(Cyan::Engine::colorSpaceGRAY).toStdMap()) {
        std::cout << "   * " << profile.first.toStdString() << " : " << profile.second.toStdString() << std::endl;
    }
}

QTEST_APPLESS_MAIN(Test)

#include "tests.moc"
