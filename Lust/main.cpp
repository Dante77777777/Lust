#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Lust Vision");
    a.setOrganizationName("OnePiece");

    QFile styleFile(":/resource/style.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        a.setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    }

    MainWindow w;
    w.show();
    return a.exec();
}
