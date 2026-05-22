#include "clienttask.h"

#include <QThread>

ClientTask::ClientTask()
{

}

ClientTask::ClientTask(MyTcpSocket *socket)
{
    m_socket = socket;
}

void ClientTask::run()
{
    //关联接受到消息的信号的处理接收的函数
    connect(m_socket,&QTcpSocket::readyRead,m_socket,&MyTcpSocket::recvMsg);
    connect(m_socket,&QTcpSocket::disconnected,m_socket,&MyTcpSocket::clientoffline);
    m_socket->moveToThread(QThread::currentThread());
}
