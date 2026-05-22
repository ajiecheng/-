#include "server.h"
#include "operatedb.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server::getInstance();
   // w.show();
    Server& server = Server::getInstance();
    OperateDb::getInstance().connectDb(server.m_strDbHost, server.m_usDbPort,
                                        server.m_strDbName, server.m_strDbUser, server.m_strDbPwd);
    return a.exec();
}
