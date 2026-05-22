#include "client.h"
#include "index.h"
#include "sharefile.h"
#include "ui_sharefile.h"

#include <QMessageBox>

ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateFirend_LW()
{
    ui->listWidget->clear();
    QListWidget* friend_LW = Index::getInstance().getFriend()->getFirend_LW();
    for (int i = 0; i < friend_LW->count() ; i++) {
        QListWidgetItem* friendItem = friend_LW->item(i);
        QListWidgetItem* newItem = new QListWidgetItem(*friendItem);
        ui->listWidget->addItem(newItem);
    }
}

void ShareFile::on_allSelect_PB_clicked()
{
    for(int i = 0; i < ui->listWidget->count(); i++){
        ui->listWidget->item(i)->setSelected(true);
    }
}

void ShareFile::on_cancelSelect_PB_clicked()
{
    for(int i = 0; i < ui->listWidget->count(); i++){
        ui->listWidget->item(i)->setSelected(false);
    }
}

void ShareFile::on_ok_PB_clicked()
{
    //当前用户名、分享文件的完整路径、要分享的好友名
    //路径和好友名放caMsg，要存好友数量为了取时算出偏移量
    QString strCurName = Client::getInstance().m_strUserName;
    QString strCurPath = Index::getInstance().getFile()->m_strCurPath;
    QString strShaerFileName = Index::getInstance().getFile()->m_strShareFileName;
    QString strPath = QString("%1/%2").arg(strCurPath).arg(strShaerFileName);
    QList<QListWidgetItem*> pItems = ui->listWidget->selectedItems();

    int iFriendNum = pItems.size();
    PDU* pdu = mkPDU(iFriendNum*32+strPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    memcpy(pdu->caData, strCurName.toStdString().c_str(),32);
    memcpy(pdu->caData+32, &iFriendNum,sizeof(int));

    for (int i = 0;i < iFriendNum; i++) {
        memcpy(pdu->caMsg+i*32,pItems.at(i)->text().toStdString().c_str(),32);
    }
    memcpy(pdu->caMsg+iFriendNum*32,strPath.toStdString().c_str(),strPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}
