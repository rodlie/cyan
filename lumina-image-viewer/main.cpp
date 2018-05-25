#include "viewer.h"
#include <QApplication>
#include <Magick++.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Magick::InitializeMagick(NULL);
    Viewer w;
    w.show();

    return a.exec();
}
