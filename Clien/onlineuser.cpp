#include "client.h"
#include "onlineuser.h"
#include "ui_onlineuser.h"

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::updateOnlineUser(QStringList userList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(userList);
}

void OnlineUser::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strTarName = item->text();//目标用户
    QString strCurName = Client::getInstance().m_strUserName;//当前用户
    PDU* pdu = mkPDU();//定义PDU
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;//定义协议类型
    memcpy(pdu->caData,strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32,strTarName.toStdString().c_str(),32);
    Client::getInstance().sendMsg(pdu);//发送给服务器

}
