/*
# GetICC <https://github.com/rodlie/geticc>
# Copyright (c) 2018 Ole-André Rodlie. All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include <iostream>
#include <fstream>

#ifndef NOGUI
#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#endif

#define ICC_HEADER_LENGTH 128

#ifndef ORD8
#define ORD8 unsigned char
#endif

static unsigned int readUInt32Number(char *p) {
    unsigned int rv;
    rv = 16777216 * (unsigned int)((ORD8 *)p)[0]
       +    65536 * (unsigned int)((ORD8 *)p)[1]
       +      256 * (unsigned int)((ORD8 *)p)[2]
       +            (unsigned int)((ORD8 *)p)[3];
    return rv;
}

inline bool fileExists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

inline bool extractICC(const std::string& filename, std::string newfilename)
{
    if (filename.empty()) { return false; }
    std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
    if (!file.is_open()) {
        std::cout << "unable to open file " << filename << std::endl;
        return false;
    }
    unsigned int profileSize = 0;
    int profileOffset = 0;
    int offset = 0;
    int found;
    do {
        found = 0;
        int fc = 0;
        char c;
        file.seekg(offset,std::ios::beg);
        if (file.tellg()!=offset) {
            break;
        }
        while(found == 0) {
            if (!file.read(&c,1)) { break; }
            offset++;
            switch (fc) {
            case 0:
                if (c == 'a') { fc++; } else { fc = 0; }
                break;
            case 1:
                if (c == 'c') { fc++; } else { fc = 0; }
                break;
            case 2:
                if (c == 's') { fc++; } else { fc = 0; }
                break;
            case 3:
                if (c == 'p') {
                    found = 1;
                    offset -= 40;
                } else { fc = 0; }
                break;
            }
        }
        if (found) {
            profileOffset = offset;
            file.seekg(offset, std::ios::beg);
            char *sizeBuffer = new char[4];
            if (file.read(sizeBuffer, 4)) {
                profileSize = readUInt32Number(sizeBuffer);
                std::cout << "found a profile at offset " << offset << ", size is " << profileSize << std::endl;
            } else {
                std::cout << "found a profile header, but unable to get the profile size" << std::endl;
            }
            delete[] sizeBuffer;
            offset += ICC_HEADER_LENGTH;
        }
    } while (found != 0);
    bool wroteFile = false;
    if (profileOffset>0 && profileSize>ICC_HEADER_LENGTH) {
        if (newfilename.empty()) {
#ifndef NOGUI
            Fl_Native_File_Chooser fc;
            fc.title("Save ICC profile to ...");
            fc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
            std::string fallback = filename;
            fallback.append(".icc");
            fc.preset_file(fallback.c_str());
            switch (fc.show()) {
            case -1:
                fl_alert("%s",fc.errmsg());
                return false;
            default:
                newfilename = fc.filename();
            }
            if (newfilename.empty()) { newfilename = fallback; }
#else
            newfilename = filename;
            newfilename.append(".icc");
            int checks = 0;
            while (fileExists(newfilename)) {
                if (checks>10) { return false; }
                newfilename.append(".icc");
                ++checks;
            }
#endif
        }

        file.clear();
        file.seekg(profileOffset,std::ios::beg);
        char *profileBuffer = new char[profileSize];
        if (file.read(profileBuffer, profileSize)) {
            std::ofstream output(newfilename.c_str(), std::ios::out|std::ios::binary|std::ios::ate);
            if (output.is_open()) {
                output.write(profileBuffer, profileSize);
                output.close();
                wroteFile = fileExists(newfilename);
                std::cout << "wrote profile to " << newfilename << std::endl;
            } else {
                std::cout << "unable to write to file, probably permission issue" << std::endl;
            }
        } else {
            std::cout << "unable to read profile buffer, this should not happen ;)" << std::endl;
        }
        delete[] profileBuffer;
    } else {
        std::cout << "this file does not have an embedded profile" << std::endl;
    }
    file.close();
    return wroteFile;
}

int main(int argc, const char* argv[])
{
    std::cout << "GetICC (c)2018 Ole-André Rodlie <http://prepress.sf.net>" << std::endl;
#ifndef NOGUI
    std::cout << "Uses FLTK for widgets <http://www.fltk.org>" << std::endl;
#endif
    std::cout << "" << std::endl;

    std::string filename,icc;
    if (argc>=2) { filename = argv[1]; }
    if (argc>=3) { icc = argv[2]; }
    if (filename.empty()) {
#ifndef NOGUI
        Fl_Native_File_Chooser fc;
        fc.title("Pick a file");
        fc.type(Fl_Native_File_Chooser::BROWSE_FILE);
        switch (fc.show()) {
        case -1:
            fl_alert("%s", fc.errmsg());
            return 1;
        case  1:
            return 0;
        default:
            filename = fc.filename();
        }
#else
        std::cout << "missing filename" << std::endl;
        return 1;
#endif
    }
    if (extractICC(filename, icc)) {
#ifndef NOGUI
        std::string message = "Extracted ICC color profile from\n";
        message.append(filename);
        fl_alert("%s", message.c_str());
#endif
    } else {
#ifndef NOGUI
        std::string message = "Failed to find or extract an ICC color profile from\n";
        message.append(filename);
        fl_alert("%s", message.c_str());
#endif
    }
    return 0;
}
