#include "chat.h"
#include "client.h"
#include "ui_chat.h"

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

void Chat::updageShow_TE(QString strMsg)
{
    ui->show_TE->append(strMsg);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_send_PB_clicked()
{
    QString strCurName = Client::getInstance().m_strUserName;//当前用户
    QString strMsg = ui->msg_LE->text();//目标用户
    ui->msg_LE->clear();
    PDU* pdu = mkPDU(strMsg.toStdString().size()+1);//定义PDU
    pdu->uiMsgType = ENUM_MSG_TYPE_CHAT_REQUEST;//定义协议类型
    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,m_strChatName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.toStdString().size());
    Client::getInstance().sendMsg(pdu);//发送给服务器

}
