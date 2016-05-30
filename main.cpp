#include "mainwindow.h"
#include <QApplication>
#include <QtDBus/QDBusMetaType>
#include "nowplaying.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setFixedSize(1024, 600);
    w.show();

    return a.exec();
}
