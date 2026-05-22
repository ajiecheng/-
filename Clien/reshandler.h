#ifndef RESHANDLER_H
#define RESHANDLER_H

#include "protocol.h"



class ResHandler
{
public:
    ResHandler();
    void regist(PDU* pdu);
    void login(PDU* pdu);
    void findUser(PDU* pdu);
    void onlineUser(PDU* pdu);
    void addFriend(PDU* pdu);
    void addFriendResend(PDU* pdu);
    void addFriendAgree(PDU* pdu);
    void flushFriend(PDU* pdu);
    void delFriend(PDU* pdu);
    void chat(PDU* pdu);
    void mkdir(PDU* pdu);
    void flushFile(PDU* pdu);
    void deldir(PDU* pdu);
    void uploadFileInit(PDU* pdu);
    void uploadFileData(PDU* pdu);
    void shareFile(PDU* pdu);
    void shareFileResend(PDU* pdu);



};

#endif // RESHANDLER_H
