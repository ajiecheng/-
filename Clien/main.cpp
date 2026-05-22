#include "client.h"

#include <QApplication>
//程序入口
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client::getInstance().show();
//    Client w;
//    w.show();
    return a.exec();
}
