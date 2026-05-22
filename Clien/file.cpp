#include "client.h"
#include "file.h"
#include "ui_file.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    ui->setupUi(this);
    m_strUserPath = QString("%1/%2").arg(Client::getInstance().m_strRootPath).arg(Client::getInstance().m_strUserName);
    m_strCurPath = m_strUserPath;
    m_pShareFile = new ShareFile;
    flushFile();
}

File::~File()
{
    delete ui;
}

void File::flushFile()
{
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::updateFileList(QList<FileInfo *> pFileList)
{
    foreach(FileInfo* pFileInfo,m_pFileInfoList){
        delete pFileInfo;
    }
    //File类定义QList<FileInfo *>类型的成员变量m_pFileInfoList存当前路径的所有文件按信息
    m_pFileInfoList.clear();
    m_pFileInfoList = pFileList;
    ui->listWidget->clear();
    for(int i = 0;i < pFileList.size(); i++){
       QListWidgetItem* pItem = new QListWidgetItem;
       if(pFileList[i]->iFileType == 0){
           pItem->setIcon(QIcon(QPixmap(":/dir.png")));
       }else if(pFileList[i]->iFileType == 1){
           pItem->setIcon(QIcon(QPixmap(":/file.png")));
       }
       pItem->setText(pFileList[i]->caName);
       ui->listWidget->addItem(pItem);
    }
}

void File::uploadFile()
{
    //使用QFile对象打开文件
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(this,"上传文件","打开文件失败");
        return;
    }
    //循环读取文件并发送
    PDU* datapdu = mkPDU(4096);
    datapdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST;
    while(true){
        qint64 ret = file.read(datapdu->caMsg,4096);
        if(ret < 0){
            QMessageBox::information(this,"上传文件","读取文件失败");
            return;
        }if(ret == 0)
        {
            break;
        }
        //根据实际读取的长度更新消息长度及总长度
        datapdu->uiMsgLen = ret;
        datapdu->uiPDULen = ret+sizeof(PDU);
        Client::getInstance().m_socket.write((char*)datapdu,datapdu->uiPDULen);
    }
    //释放空间、关闭文件
    free(datapdu);
    datapdu = NULL;
    file.close();
}

void File::on_mkdir_PB_clicked()
{
    QString strName =  QInputDialog::getText(this,"新建文件夹","文件夹名");//输入对话框
    if(strName.isEmpty()||strName.toStdString().size()>32){
        QMessageBox::information(this,"新建文件夹","文件夹长度非法");
        return;
    }
    qDebug()<<"查找用户 strName" << strName;
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_MKDIR_REQUEST;
    memcpy(pdu->caData,strName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_flushFile_PB_clicked()
{
    flushFile();
}

void File::on_deldir_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(pItem == NULL){
        QMessageBox::information(this,"删除文件夹","请选择要删除的文件夹");
        return;
    }
    QString strDelName = pItem->text();
    foreach(FileInfo* pFileInfo, m_pFileInfoList){
        if(strDelName == pFileInfo->caName && pFileInfo->iFileType != 0){
            QMessageBox::information(this,"删除文件夹","请选择正确的文件夹");
        }
    }
    int ret = QMessageBox::question(this,"删除文件夹",QString("是否确认删除 %1").arg(strDelName));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
    memcpy(pdu->caData,strDelName.toStdString().c_str(),32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strDirName = item->text();

    foreach(FileInfo* pFileInfo, m_pFileInfoList){
        if(strDirName == pFileInfo->caName && pFileInfo->iFileType != 0){
            return;
        }
    }
    m_strCurPath = QString("%1/%2").arg(m_strCurPath).arg(strDirName);
    flushFile();
}

void File::on_return_PB_clicked()
{
    if(m_strCurPath == m_strUserPath){
        QMessageBox::information(this,"返回","返回失败，已经到顶了~");
        return;
    }
    int index = m_strCurPath.lastIndexOf('/');
    m_strCurPath.remove(index,m_strCurPath.size() - index);
    flushFile();
}

void File::on_uploadFile_PB_clicked()//上传文件
{
    //发送当前路径（给服务器用）、文件名、文件大小

    m_strUploadFilePath = QFileDialog::getOpenFileName();//获取用户选择的文件的路径（本地路径）\成员变量
    //qDebug()<<"m_strUploadFilePath"<<m_strUploadFilePath;//用于获取文件名和文件大小,问题点?
    int index = m_strUploadFilePath.lastIndexOf('/');
    QString strFileName =  m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);
    QFile file(m_strUploadFilePath);
    qint64 iFileSize = file.size();
    //构建pdu，文件名和文件大小放入caData,当前路径放入caMsg
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size()+1);
    pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_INIT_REQUEST;
    memcpy(pdu->caData,strFileName.toStdString().c_str(),32);
    memcpy(pdu->caData+32, &iFileSize,32);
    memcpy(pdu->caMsg,m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendMsg(pdu);
}

void File::on_shareFile_PB_clicked()
{
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if(pItem == NULL){
        QMessageBox::information(this,"分享文件","请选择要分享的文件");
        return;
    }
    m_strShareFileName = pItem->text();
    m_pShareFile->updateFirend_LW();
    if(m_pShareFile->isHidden()){
       m_pShareFile->show();
    }
}
