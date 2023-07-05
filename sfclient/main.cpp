//Файл main.cpp
#include "mainwidget.h"

#include <QApplication>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w("localhost", 55000);

    w.show();
    w.showStartMenu();
    return a.exec();
}
