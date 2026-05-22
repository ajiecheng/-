#include "myqtcpserver.h"
#include "server.h"

#include <QFile>
#include <QDebug>
//服务器实现
Server::Server(QWidget *parent): QWidget(parent)
{
    loadConfig();
    MyQTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

Server::~Server()
{

}

void Server::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = QString(baData);
        QStringList strList = strData.split("\r\n");

        m_strIP=strList.at(0);
        m_usPort=strList.at(1).toUShort();
        m_strRootPath = strList.at(2);
        // 解析数据库配置（跳过注释行）
        m_strDbHost = strList.at(4);
        m_usDbPort = strList.at(5).toUShort();
        m_strDbName = strList.at(6);
        m_strDbUser = strList.at(7);
        m_strDbPwd = strList.at(8);
        qDebug()<<"ip:"<< m_strIP <<"port:"<<m_usPort<<"RootPath"<<m_strRootPath;

        file.close();

    }
}

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}


