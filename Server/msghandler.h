#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"

#include <QFile>
#include <QString>



class MsgHandler
{
public:
    MsgHandler();
    qint64 m_iUploadFileSize;
    qint64 m_iUploadFileReceived;
    QFile m_fUploadFile;
    PDU* regist(PDU* pdu);
    PDU* login(PDU* pdu,QString& loginName);
    PDU* findUser(PDU* pdu);
    PDU* onlineUser();
    PDU* addFriend(PDU* pdu);
    PDU* addFriendArgee(PDU* pdu);
    PDU* flushFriend(PDU* pdu);
    PDU* delFriend(PDU* pdu);
    PDU* chat(PDU* pdu);
    PDU* mkdir(PDU* pdu);
    PDU* flushFile(PDU* pdu);
    PDU* deldir(PDU* pdu);
    PDU* uploadFileInit(PDU* pdu);
    PDU* uploadFileData(PDU* pdu);
    PDU* shareFile(PDU* pdu);
    PDU* shareFileAgree(PDU* pdu);

};

#endif // MSGHANDLER_H
