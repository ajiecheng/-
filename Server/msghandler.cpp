#include "msghandler.h"
#include "myqtcpserver.h"
#include "operatedb.h"
#include "server.h"
//#include <string>
#include <QDebug>
#include <QDir>

MsgHandler::MsgHandler()
{

}

PDU* MsgHandler::regist(PDU *pdu)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    memcpy(caPwd,pdu->caData+32,32);
    bool ret = OperateDb::getInstance().handleRegist(caName,caPwd);
    qDebug()<<"注册返回 ret"<<ret;
    if(ret){
        QDir dir;
        dir.mkdir(QString("%1/%2").arg(Server::getInstance().m_strRootPath).arg(caName));
    }
    PDU*respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(bool));
    respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
    return respdu;
}

PDU *MsgHandler::login(PDU *pdu,QString &loginName)//引用
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    memcpy(caPwd,pdu->caData+32,32);
    bool ret = OperateDb::getInstance().handleLogin(caName,caPwd);
    qDebug()<<"登录返回"<<ret;
    if(ret){
        loginName = caName;
    }
    PDU* respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(bool));
    respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
    return respdu;
}

PDU *MsgHandler::findUser(PDU *pdu)
{
    char caName[32] = {'\0'};
    memcpy(caName,pdu->caData,32);
    int ret = OperateDb::getInstance().handleFindUser(caName);
    qDebug()<<"查找返回"<<ret;
    PDU* respdu = mkPDU();
    qDebug()<<"我靠了";
    memcpy(respdu->caData,&ret,sizeof(int));
    respdu->uiMsgType = ENUM_MSG_TYPE_FIND_USER_RESPOND;
    return respdu;
}

PDU *MsgHandler::onlineUser()
{
    QStringList ret = OperateDb::getInstance().handleOnlineUser();
    qDebug()<<"在线用户"<<ret;
    PDU* respdu = mkPDU(ret.size()*32);
    respdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_USER_RESPOND;
    for(int i = 0 ; i<ret.size();i++){
        memcpy(respdu->caMsg+32*i,ret.at(i).toStdString().c_str(),32);
        qDebug()<<"第"<<i+1<<"个在线用户"<<respdu->caMsg+32*i;
    }
    return respdu;
}

PDU *MsgHandler::addFriend(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};//目标用户
    memcpy(caCurName,pdu->caData,32);
    memcpy(caTarName,pdu->caData+32,32);
    int ret = OperateDb::getInstance().handleAddFirend(caCurName,caTarName);
    qDebug()<<"添加好友 ret"<<ret;
    if(ret == 1){
        MyQTcpServer::getInstance().resend(caTarName,pdu);//发送给目标客户端
    }
    PDU*respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(int));
    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
    return respdu;
}

PDU *MsgHandler::addFriendArgee(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};//目标用户
    memcpy(caCurName,pdu->caData,32);
    memcpy(caTarName,pdu->caData+32,32);
    bool ret = OperateDb::getInstance().handleAddFirendArgee(caCurName,caTarName);
    qDebug()<<"同意添加好友 ret"<<ret;
    PDU*respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(bool));
    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_ARGEE_RESPOND;
    return respdu;
}

PDU *MsgHandler::flushFriend(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    memcpy(caCurName,pdu->caData,32);
    QStringList res = OperateDb::getInstance().handleFlushFriend(caCurName);
    //    qDebug()<<"刷新好友 ret"<<ret;
    PDU*respdu = mkPDU(res.size()*32);
    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
    for(int i = 0;i<res.size();i++)
    {
        memcpy(respdu->caMsg+i*32,res.at(i).toStdString().c_str(),32);//直接通过Msg发送
    }
    return respdu;
}

PDU *MsgHandler::delFriend(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    char caTarName[32] = {'\0'};//目标用户
    memcpy(caCurName,pdu->caData,32);
    memcpy(caTarName,pdu->caData+32,32);
    bool ret = OperateDb::getInstance().handleDelFirend(caCurName,caTarName);
    qDebug()<<"删除好友 ret"<<ret;
    PDU*respdu = mkPDU();
    memcpy(respdu->caData,&ret,sizeof(bool));
    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_FRIEND_RESPOND;
    return respdu;
}

PDU *MsgHandler::chat(PDU *pdu)
{
    char caTarName[32] = {'\0'};//目标用户
    memcpy(caTarName,pdu->caData+32,32);
    MyQTcpServer::getInstance().resend(caTarName,pdu);
    return NULL;
}

PDU *MsgHandler::mkdir(PDU *pdu)
{
    QString strNewPath = QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData);
    QDir dir;
    bool ret = dir.mkdir(strNewPath);
    PDU* respdu = mkPDU();
    respdu->uiMsgType = ENUM_MSG_TYPE_MKDIR_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::flushFile(PDU *pdu)
{
    //使用QDidr对象获取路径下的所有文件信息
    QDir dir(pdu->caMsg);
    QFileInfoList fileInfoList =  dir.entryInfoList();
    //获取文件信息，根据文件数量和文件信息结构体的大小计算出需要caMsg的和长度，构建respdu
    int iFileCount =  fileInfoList.size();
    PDU* respdu = mkPDU(iFileCount * sizeof (FileInfo));
    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
    //遍历文件，将respsu转成FileInfo*进行偏移，把文件名和文件类型放入结构体
    QString strFileName;
    for(int i = 0 ;i < iFileCount; i++ ){
        strFileName = fileInfoList[i].fileName();
        FileInfo* pFileInfo = (FileInfo*)respdu->caMsg+i;//偏移指针大小的位置
        memcpy(pFileInfo->caName,strFileName.toStdString().c_str(),32);
        if(fileInfoList[i].isDir()){
            pFileInfo->iFileType = 0;//目录
        }else{
            pFileInfo->iFileType = 1;//文件
        }
        qDebug()<<"strFileName 刷新文件"<<strFileName;
    }
    return respdu;
}

PDU *MsgHandler::deldir(PDU *pdu)
{
    QString strNewPath = QString("%1/%2").arg(pdu->caMsg).arg(pdu->caData);
    QDir dir(strNewPath);
    bool ret = dir.removeRecursively();
    PDU* respdu = mkPDU();
    respdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::uploadFileInit(PDU *pdu)
{
    //取出文件名和大小
    char caFileName[32] = {'\0'};
    memcpy(caFileName,pdu->caData,32);
    memcpy(&m_iUploadFileSize, pdu->caData+32,sizeof (qint64));
    //拼接完整路径并设置成员变量m_fUploadFile的路径
    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(caFileName);
    m_fUploadFile.setFileName(strPath);
    m_iUploadFileReceived = 0;
    //创建并打开文件，将结果返回给客户端
    bool ret = m_fUploadFile.open(QIODevice::WriteOnly);

    PDU* respdu = mkPDU();
    respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_INIT_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}

PDU *MsgHandler::uploadFileData(PDU *pdu)
{
    m_fUploadFile.write(pdu->caMsg,pdu->uiMsgLen);
    m_iUploadFileReceived += pdu->uiMsgLen;
    if(m_iUploadFileReceived < m_iUploadFileSize){
        return NULL;
    }
    m_fUploadFile.close();
    PDU* respdu = mkPDU();
    respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPOND;
    return respdu;
}

PDU *MsgHandler::shareFile(PDU *pdu)
{
    char strCurName[32] = {'\0'};
    memcpy(strCurName,pdu->caData,32);
    int iFriendNum = 0;
    memcpy(&iFriendNum,pdu->caData+32,sizeof(int));
    PDU* resendpdu = mkPDU(pdu->uiMsgLen-iFriendNum*32);
    resendpdu->uiMsgType = pdu->uiMsgType;
    memcpy(resendpdu->caData,strCurName,32);
    memcpy(resendpdu->caMsg,pdu->caMsg+iFriendNum*32,pdu->uiMsgLen-iFriendNum*32);

    char caRecvName[32] = {'\0'};
    for(int i = 0;i < iFriendNum; i++){
        memcpy(caRecvName, pdu->caMsg+i*32,32);
        MyQTcpServer::getInstance().resend(caRecvName, resendpdu);
    }
    PDU* respdu = mkPDU();
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_ARGEE_RESPOND;
    return respdu;
}

PDU *MsgHandler::shareFileAgree(PDU *pdu)
{
    QString strShareFriendPath = pdu->caMsg;
    int index = strShareFriendPath.lastIndexOf('/');
    QString strFileName = strShareFriendPath.right(strShareFriendPath.size() - index -1);

    QString strNewPath = QString("%1/%2/%3").arg(Server::getInstance().m_strRootPath)
            .arg(pdu->caData).arg(strFileName);
    bool ret = QFile::copy(strShareFriendPath, strNewPath);
    PDU* respdu = mkPDU();
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_ARGEE_RESPOND;
    memcpy(respdu->caData,&ret,sizeof(bool));
    return respdu;
}
