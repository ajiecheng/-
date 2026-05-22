#include "client.h"
#include "index.h"
#include "protocol.h"
#include "ui_client.h"
#include "uploader.h"

#include <QFile>
#include <QMessageBox>
#include <QDebug>
//客户端实现
Client::Client(QWidget *parent) : QWidget(parent), ui(new Ui::Client)
{
    ui->setupUi(this);//初始化界面
    loadConfig();
    //连接服务器
    m_socket.connectToHost(QHostAddress(m_strIP),m_usPort);//最好先连接信号槽，再连接服务器，防止信号延迟
    //谁发，发的什么，谁接收，槽函数
    connect(&m_socket,&QTcpSocket::connected,this,&Client::showConnect);//connected:连接的
    connect(&m_socket,&QTcpSocket::readyRead,this,&Client::recvMsg);
    m_prh = new ResHandler;//创建消息处理器指向一块新内存，需要记得释放防止内存泄露
}

Client::~Client()
{
    delete ui;//销毁界面管理对象（释放控件内存）
    delete m_prh;
    m_prh = NULL;//重置消息处理器
}

void Client::loadConfig()//加载配置
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly)){
        //读取文件
        QByteArray baData =file.readAll();
        QString strData = QString(baData);
        QStringList strList = strData.split("\r\n");

        m_strIP=strList.at(0);
        m_usPort=strList.at(1).toUShort();
        m_strRootPath = strList.at(2);
        qDebug()<<"ip:"<< m_strIP <<"port:"<<m_usPort<<"RootPath"<<m_strRootPath;

        file.close();//关闭文件
    }else{
        QMessageBox::information(this,"打开配置文件","打开失败");//消息弹窗弹出消息
    }
}

Client &Client::getInstance()
{
    static Client instance;
    return instance;
}

void Client::sendMsg(PDU*pdu)//包装发送函数
{
    if(pdu==NULL){
        return;
    }

    m_socket.write((char*)pdu,pdu->uiPDULen);
    qDebug()<< "send uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" <<pdu->caMsg;
    free(pdu);
    pdu=NULL;
}

PDU *Client::readMsg()
{
    qDebug()<<"readMsg 读消息的长度"<<m_socket.bytesAvailable();
    unsigned int uiPDULen = 0;
    m_socket.read((char* ) &uiPDULen,sizeof(unsigned int));

    PDU* pdu = mkPDU(uiPDULen-sizeof(PDU));
    m_socket.read((char*)pdu+sizeof(unsigned int),uiPDULen-sizeof(unsigned int));
    qDebug()<< "readMsg(接收回应) uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" <<pdu->caMsg;
    return pdu;
}

void Client::handleMsg(PDU *pdu)
{
    qDebug()<< "handleMsg(接收回应) uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" <<pdu->caMsg;
    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGIST_RESPOND:{
        m_prh ->regist(pdu);
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:{
        m_prh ->login(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FIND_USER_RESPOND:{
        m_prh ->findUser(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_RESPOND:{
        m_prh ->onlineUser(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
        m_prh ->addFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
        m_prh ->addFriendResend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_ARGEE_RESPOND:{
        m_prh ->addFriendAgree(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{
        m_prh ->flushFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_FRIEND_RESPOND:{
        m_prh ->delFriend(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUEST:{
        m_prh ->chat(pdu);
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_RESPOND:{
        m_prh ->mkdir(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:{
        m_prh ->flushFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_RESPOND:{
        m_prh ->deldir(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_INIT_RESPOND:{
        m_prh ->uploadFileInit(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND:{
        m_prh ->uploadFileData(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
        m_prh ->shareFile(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_ARGEE_RESPOND:{
        m_prh ->shareFileResend(pdu);
        break;
    }



    default:
        break;

    }
//    free(pdu);
    //    pdu=NULL;
}

void Client::startUpload()//开始上传
{
    File* f = Index::getInstance().getFile();
    Uploader* uploader = new Uploader(f->m_strUploadFilePath);
    connect(uploader,&Uploader::errorMsg,this,&Client::showError);
    connect(uploader,&Uploader::sendPDU,this,&Client::sendMsg);
    uploader->start();
}

void Client::showError(QString msg)
{
    QMessageBox::information(this,"提示",msg);
}

void Client::showConnect()
{
    qDebug()<<"服务器链接成功";
}


/*void Client::on_send_PB_clicked()
//{
//    QString strMsg = ui->input_LE->text();
//    if(strMsg.isEmpty()){
//        return;
//    }
//    PDU*pdu =mkPDU(strMsg.toStdString().size()+1);
//    pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;

//    memcpy(pdu->caMsg, strMsg.toStdString().c_str(),strMsg.toStdString().size());
//    m_socket.write((char*)pdu,pdu->uiPDULen);
//    qDebug()<< "send Msg uiPDULen" << pdu->uiPDULen
//            << "uiMsgLen" << pdu->uiMsgLen
//            << "caData" << pdu->caData
//            << "caMsg" <<pdu->caMsg;
//    free(pdu);
//    pdu=NULL;
//}*/

void Client::on_regist_PB_clicked()//注册
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    if(strName.isEmpty()){
        return;
    }
    PDU*pdu = mkPDU();
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
    pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
    sendMsg(pdu);
}

void Client::on_login_PB_clicked()//登录
{
    QString strName = ui->name_LE->text();
    QString strPwd = ui->pwd_LE->text();
    m_strUserName = strName;
    PDU*pdu = mkPDU();
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
    pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
    sendMsg(pdu);//消息发送函数
}

void Client::recvMsg()//接收消息
{
    qDebug()<<"recvMsg 读消息的长度"<<m_socket.bytesAvailable();
    //处理粘包问题
    QByteArray data = m_socket.readAll();
    buffer.append(data);
    while(buffer.size() >= int(sizeof(PDU))){
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size() < int(pdu->uiPDULen)){
            break;
        }
        handleMsg(pdu);
        buffer.remove(0,pdu->uiPDULen);
    }

}


