#include "clienttask.h"
#include "myqtcpserver.h"

MyQTcpServer::MyQTcpServer()
{
    threadPool.setMaxThreadCount(8);
}

void MyQTcpServer::incomingConnection(qintptr handle)
{
    qDebug()<<"客户端链接成功";
    MyTcpSocket* pSocket =new MyTcpSocket;//连接成功一个则创建一个MyTcpSocket对象
    pSocket->setSocketDescriptor(handle);
    m_socketList.append(pSocket);//用列表来存所有与服务器连接的客户端
    ClientTask* task = new ClientTask(pSocket);
    threadPool.start(task);
  /*for(int i= 0;i< m_socketList.size();i++){
   *
        qDebug() << m_socketList.at(i);
        }*/
}

//接收客户端连接
MyQTcpServer &MyQTcpServer::getInstance()
{
    static MyQTcpServer instance;
    return instance;
}

void MyQTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    m_socketList.removeOne(mysocket);//删除列表中的客户端连接
    mysocket->deleteLater();//释放堆空间
    mysocket = NULL;
}

void MyQTcpServer::resend(char *caTarName, PDU *pdu)//转发函数
{
    if(caTarName == NULL || pdu == NULL){
        return;
    }
    for(int i = 0; i<m_socketList.size();i++){
        if(caTarName == m_socketList.at(i)->m_strUserName){
            m_socketList.at(i)->write((char*)pdu,pdu->uiPDULen);
        }
    }
}
