#include "viewer.h"
#include <QApplication>
#include <QtPlugin>
#include <Magick++.h>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
Q_IMPORT_PLUGIN(ImageFilters)
#else
Q_IMPORT_PLUGIN(imageFilters)
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName(QString("viewer"));
    QApplication::setOrganizationDomain("org.lumina-desktop");
    Magick::InitializeMagick(NULL);
    Viewer w;
    w.show();

    return a.exec();
}
