#include "friend.h"
#include "ui_friend.h"
#include "QDebug"
#include "protocol.h"
#include "client.h"
#include <QInputDialog>
#include <QMessageBox>

Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    m_pOnlineUser = new OnlineUser;
    m_pChat = new Chat;
    flushFriend();//后续需要修改，粘包问题
}

Friend::~Friend()
{
    delete m_pOnlineUser;
    delete m_pChat;
    delete ui;
}

void Friend::flushFriend()
{
    QString strCurName = Client::getInstance().m_strUserName;//当前用户
    PDU* pdu = mkPDU();
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);//单例发送
}

void Friend::updateFriend(QStringList userList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(userList);
}

QListWidget *Friend::getFirend_LW()
{
    return ui->listWidget;
}


void Friend::on_findUser_PB_clicked()
{
    QString strName =  QInputDialog::getText(this,"查找用户","用户名");//输入对话框
    if(strName.isEmpty()){
        return;
    }
    qDebug()<<"查找用户 strName" << strName;
    //将用户名发送给服务器
    PDU*pdu = mkPDU();
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    pdu->uiMsgType = ENUM_MSG_TYPE_FIND_USER_REQUEST;
    Client::getInstance(). sendMsg(pdu);/*m_socket.write((char*)pdu,pdu->uiPDULen);
    qDebug()<< "send Msg(发送查找请求) uiPDULen" << pdu->uiPDULen
            << "uiMsgLen" << pdu->uiMsgLen
            << "uiMsgType" << pdu->uiMsgType
            << "caData" << pdu->caData
            << "caData+32" << pdu->caData+32
            << "caMsg" <<pdu->caMsg;
    free(pdu);
    pdu=NULL;*/
}


void Friend::on_onlineUser_PB_clicked()
{
    if(m_pOnlineUser->isHidden()){
        m_pOnlineUser->show();
    }
    PDU*pdu = mkPDU();
    pdu->uiMsgType =ENUM_MSG_TYPE_ONLINE_USER_REQUEST;
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_flushFriend_PB_clicked()
{
    flushFriend();
}

void Friend::on_delFriend_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"删除好友","请选择要删除的好友");
        return;
    }
    QString strTarName = pItem->text();//目标用户名
    QString strCurName = Client::getInstance().m_strUserName;//当前用户
    PDU* pdu = mkPDU();
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FRIEND_REQUEST;
    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strTarName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);
}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(!pItem){
        QMessageBox::information(this,"开始聊天","请选择聊天好友");
        return;
    }
    if(m_pChat->isHidden()){
        m_pChat->show();
    }
    m_pChat->m_strChatName = pItem->text();
}
