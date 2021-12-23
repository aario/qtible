#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir dir(Settings::getLibraryPath());
    if (!dir.exists(dir.path()))
        dir.mkpath(dir.path());

    MainWindow w;
    w.show();
    return a.exec();
}
