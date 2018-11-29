# GetICC
Extract embedded ICC color profiles from files.

# Usage
If you have the GUI version just launch the application, select input and output. The commandline version takes input filename and optional output filename.

# Build

The commandline version has no dependencies and is easy to build, see ``Makefile.unix``, or just ``make -f Makefile.unix``. The GUI (primarily made for use in Windows) version can be built using CMake, note that FLTK is required (you can also build using FLTK directly ``fltk-config --compile geticc.cxx``).
