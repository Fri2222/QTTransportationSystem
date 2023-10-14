#include "login_window.h"
//#include "main_interface.h"
#include <QApplication>
#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
