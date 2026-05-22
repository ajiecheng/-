#include "mytcpsocket.h"
#include "protocol.h"
#include "operatedb.h"
#include "myqtcpserver.h"
//网络通信的起点和终点
MyTcpSocket::MyTcpSocket()
{
    //关联接收到消息的信号和处理接收的函数
    //谁发，发的什么，谁接收，槽函数
    connect(this,&MyTcpSocket::readyRead,this,&MyTcpSocket::recvMsg);
    connect(this,&MyTcpSocket::disconnected,this,&MyTcpSocket::clientoffline);
    m_pmh = new MsgHandler;
}

void MyTcpSocket::sendMsg(PDU*pdu)//包装发送函数
{
    if(pdu==NULL){
        return;
    }

    this -> write((char*)pdu,pdu->uiPDULen);
    qDebug()<< "send(接收处理请求) uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" <<pdu->caMsg;
    free(pdu);
    pdu=NULL;
}

PDU* MyTcpSocket::readMsg()
{
    qDebug()<<"readMsg 读取消息的长度"<<this->bytesAvailable();

    unsigned int uiPDULen = 0;
    this->read((char* ) &uiPDULen,sizeof(unsigned int));

    PDU* pdu = mkPDU(uiPDULen-sizeof(PDU));
    this->read((char*)pdu+sizeof(unsigned int),uiPDULen-sizeof(unsigned int));
    qDebug()<< "readMsg(读取消息) uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" <<pdu->caMsg;
    return pdu;
}

PDU *MyTcpSocket::handleMsg(PDU *pdu)
{
    qDebug()<< "handleMsg(读取消息) uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" <<pdu->caMsg;
    PDU* respdu = NULL;
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_REQUEST:{
        respdu = m_pmh -> regist(pdu);
        break;
    }

    case ENUM_MSG_TYPE_LOGIN_REQUEST:{
        respdu = m_pmh -> login(pdu,m_strUserName);
        break;
    }
    case ENUM_MSG_TYPE_FIND_USER_REQUEST:{
        respdu = m_pmh -> findUser(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_REQUEST:{
        respdu = m_pmh -> onlineUser();
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        respdu = m_pmh -> addFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_ARGEE_REQUEST:{
        respdu = m_pmh -> addFriendArgee(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:{
        respdu = m_pmh -> flushFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_FRIEND_REQUEST:{
        respdu = m_pmh -> delFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUEST:{
        respdu = m_pmh -> chat(pdu);
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_REQUEST:{
        respdu = m_pmh -> mkdir(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:{
        respdu = m_pmh -> flushFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_REQUEST:{
        respdu = m_pmh -> deldir(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_INIT_REQUEST:{
        respdu = m_pmh -> uploadFileInit(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST:{
        respdu = m_pmh -> uploadFileData(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
        respdu = m_pmh -> shareFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_ARGEE_REQUEST:{
        respdu = m_pmh -> shareFileAgree(pdu);
        break;
    }



    default:
        break;
    }
//    free(pdu);//更改后不是申请的空间了
//    pdu=NULL;
    return respdu;
}

MyTcpSocket::~MyTcpSocket()
{
    delete m_pmh;
    m_pmh = NULL;
}

void MyTcpSocket::recvMsg()//消息接受与回馈
{
    qDebug()<<"recvMsg 读取消息的长度"<<this->bytesAvailable();
    //处理粘包问题
    QByteArray data = this->readAll();
    buffer.append(data);
    while(buffer.size() >= int(sizeof(PDU))){
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size() < int(pdu->uiPDULen)){
            break;
        }
        PDU* respdu = handleMsg(pdu);
        sendMsg(respdu);//消息发送函数
        buffer.remove(0,pdu->uiPDULen);
    }
}

void MyTcpSocket::clientoffline()//退出登录的状态设置
{
    OperateDb::getInstance().handleOffline(m_strUserName.toStdString().c_str());
    MyQTcpServer::getInstance().deleteSocket(this);
}
