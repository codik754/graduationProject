//Файл main.cpp
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(55000, "dbchatik");
    w.show();
    return a.exec();
}