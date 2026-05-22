#include "client.h"
#include "index.h"
#include "reshandler.h"
#include <QMessageBox>
#include <string>
ResHandler::ResHandler()
{

}

void ResHandler::regist(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof (bool));
    if(ret){
        QMessageBox::information(&Client::getInstance(),"注册","注册成功");
    }else{
        QMessageBox::information(&Client::getInstance(),"注册","注册失败");
    }
}

void ResHandler::login(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().show();//登录成功调用index
        Client::getInstance().hide();//隐藏登录窗口
    }else{
        QMessageBox::information(&Client::getInstance(),"登录","登录失败");
    }
}

void ResHandler::findUser(PDU *pdu)
{
    int ret;
    memcpy(&ret,pdu->caData,sizeof(int));
    if(ret==-1){
        QMessageBox::information(Index::getInstance().getFriend(),"查找用户","查找用户失败");
    }else if(ret == 0){
        QMessageBox::information(Index::getInstance().getFriend(),"查找用户","该用户不在线");
    }else if(ret == 1){
        QMessageBox::information(Index::getInstance().getFriend(),"查找用户","该用户在线");
    }else if(ret == 2){
        QMessageBox::information(Index::getInstance().getFriend(),"查找用户","该用户不存在");
    }
}

void ResHandler::onlineUser(PDU *pdu)
{
    QStringList userList;
     char caTmp[32] = {'\0'};
    for(int i = 0;i < int(pdu->uiMsgLen/32);i++){
        memcpy(caTmp, pdu->caMsg+i*32, 32);
        userList.append(caTmp);
    }
    Index::getInstance().getFriend()->m_pOnlineUser->updateOnlineUser(userList);

}

void ResHandler::addFriend(PDU *pdu)
{
    int ret;
    memcpy(&ret,pdu->caData,sizeof(int));
    if(ret==-1){
        QMessageBox::information(Index::getInstance().getFriend(),"添加好友","添加好友失败");
    }else if(ret == 0){
        QMessageBox::information(Index::getInstance().getFriend(),"添加好友","该用户不在线");
    }else if(ret == 2){
        QMessageBox::information(Index::getInstance().getFriend(),"添加好友","已经是好友关系");
    }
}

void ResHandler::addFriendResend(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    memcpy(caCurName,pdu->caData,32);
    int ret = QMessageBox::question(Index::getInstance().getFriend(),"添加好友",QString("是否同意 %1 的好友申请").arg(caCurName));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU*respdu = mkPDU();
    memcpy(respdu->caData,pdu->caData,64);
    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_ARGEE_REQUEST;
    Client::getInstance().sendMsg(respdu);
}

void ResHandler::addFriendAgree(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFriend()->flushFriend();
        //QMessageBox::information(Index::getInstance().getFriend(),"添加好友","添加好友成功");
        //弹窗可能阻塞
    }else{
        QMessageBox::information(Index::getInstance().getFriend(),"添加好友","添加好友失败");
    }
}

void ResHandler::flushFriend(PDU *pdu)
{
    QStringList friendList;
    char caTmp[32] = {'\0'};
    for(int i = 0;i < int(pdu->uiMsgLen/32);i++){
        memcpy(caTmp, pdu->caMsg+i*32, 32);
        friendList.append(caTmp);
    }
    Index::getInstance().getFriend()->updateFriend(friendList);

}

void ResHandler::delFriend(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFriend()->flushFriend();
    }else{
        QMessageBox::information(Index::getInstance().getFriend(),"删除好友","删除好友失败");
    }
}

void ResHandler::chat(PDU *pdu)
{
    Chat* c = Index::getInstance().getFriend()->m_pChat;
    if(c->isHidden()){
        c->show();
    }
    char caChatName[32] = {'\0'};
    memcpy(caChatName,pdu->caData,32);
    c->m_strChatName = caChatName;
    c->updageShow_TE(QString("%1: %2").arg(caChatName).arg(pdu->caMsg));

}

void ResHandler::mkdir(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();//创建成功直接刷新
    }else{
        QMessageBox::information(Index::getInstance().getFile(),"创建文件夹","创建文件夹失败");
    }
}

void ResHandler::flushFile(PDU *pdu)
{
    QList<FileInfo*> pFileList;
    int iFileCount = pdu->uiMsgLen / sizeof(FileInfo);
    for(int i = 0; i < iFileCount; i++){
        FileInfo* pFileInfo = new FileInfo;
        memcpy(pFileInfo,pdu->caMsg+i*sizeof(FileInfo),sizeof(FileInfo));
        pFileList.append(pFileInfo);
    }
    Index::getInstance().getFile()->updateFileList(pFileList);
}

void ResHandler::deldir(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().getFile()->flushFile();//创建成功直接刷新
    }else{
        QMessageBox::information(Index::getInstance().getFile(),"删除文件夹","删除文件夹失败");
    }
}

void ResHandler::uploadFileInit(PDU *pdu)
{
    bool ret;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Client::getInstance().startUpload();
    }else{
        QMessageBox::information(Index::getInstance().getFile(),"上传文件","上传文件失败");
    }
}

void ResHandler::uploadFileData(PDU *pdu)
{
    QMessageBox::information(Index::getInstance().getFile(),"上传文件","上传文件成功");
    Index::getInstance().getFile()->flushFile();//刷新
}

void ResHandler::shareFile(PDU *pdu)
{
    QMessageBox::information(Index::getInstance().getFile(),"分享文件","文件已分享");
}

void ResHandler::shareFileResend(PDU *pdu)
{
    QString strSharePath = QString(pdu->caMsg);
    int index = strSharePath.lastIndexOf('/');
    QString strFileName = strSharePath.right(strSharePath.size()-index-1);
    int ret = QMessageBox::question(Index::getInstance().getFile(),"分享文件",QString("%1 分享文件: %2\n是否接收").arg(pdu->caData).arg(strFileName));
    if(ret != QMessageBox::Yes){
        return;
    }
    PDU* respdu = mkPDU(pdu->uiMsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_ARGEE_REQUEST;
    memcpy(respdu->caData,Client::getInstance().m_strUserName.toStdString().c_str(),32);
    memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
    Client::getInstance().sendMsg(respdu);

}
