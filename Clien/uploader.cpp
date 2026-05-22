#include "client.h"
#include "uploader.h"

#include <QFile>
#include <QThread>


Uploader::Uploader()
{

}

Uploader::Uploader(const QString strFilePath)
{
    m_strUploadFilePath = strFilePath;
}

void Uploader::uploadFile()
{
    //使用QFile对象打开文件
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        emit errorMsg("上传文件失败：打开文件失败");
        emit finished();
        return;
    }
    //循环读取文件并发送
    while(true){
        PDU* datapdu = mkPDU(4096);
        datapdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST;
        qint64 ret = file.read(datapdu->caMsg,4096);
        if(ret < 0){
            emit errorMsg("上传文件失败：读取文件失败");
            emit finished();
            return;
        }if(ret == 0){
            break;
        }
        //根据实际读取的长度更新消息长度及总长度
        datapdu->uiMsgLen = ret;
        datapdu->uiPDULen = ret+sizeof(PDU);
        emit sendPDU(datapdu);
    }
    //释放空间、关闭文件
    file.close();
    emit finished();
}

void Uploader::start()
{
    QThread* thread = new QThread;
    this->moveToThread(thread);
    connect(thread,&QThread::started,this,&Uploader::uploadFile);

    connect(this,&Uploader::finished,thread,&QThread::quit);

    connect(thread,&QThread::finished,thread,&QThread::deleteLater);

    thread->start();
}
