#include "demomainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DemoMainWindow w;
    w.show();

    return a.exec();
}
