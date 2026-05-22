#ifndef MYQTCPSERVER_H
#define MYQTCPSERVER_H

//接收客户端连接
#include <QObject>
#include <QTcpServer>
#include <QThreadPool>
#include "mytcpsocket.h"
class MyQTcpServer : public QTcpServer
{
public:
    void incomingConnection(qintptr handle) override;
    static MyQTcpServer& getInstance();
    void deleteSocket(MyTcpSocket* mysocket);
    void resend(char* caTarName,PDU* pdu);
    QThreadPool threadPool;
    //QTcpServer m_socket;
private:
    MyQTcpServer();
    MyQTcpServer(const MyQTcpServer& instance)=delete;
    MyQTcpServer& operator=(const MyQTcpServer&)=delete;
    QList <MyTcpSocket*>m_socketList;
};

#endif // MYQTCPSERVER_H
