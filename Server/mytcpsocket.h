#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
//网络通信的起点和终点
#include "msghandler.h"
#include "protocol.h"

#include <QObject>
#include <QTcpSocket>

class MyTcpSocket : public QTcpSocket
{
public:
    MyTcpSocket();
    QString m_strUserName;
    void sendMsg(PDU*pdu);
    PDU* readMsg();
    PDU* handleMsg(PDU* pdu);
    MsgHandler* m_pmh;
    QByteArray buffer;

    ~MyTcpSocket();

public slots:
    void recvMsg();
    void clientoffline();
};

#endif // MYTCPSOCKET_H
