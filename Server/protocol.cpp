#include"protocol.h"
#include"stdlib.h"
#include<QDebug>
//消息协议
PDU* mkPDU(unsigned int uiMsgLen)
{
    unsigned int uiPDULen= sizeof(PDU) + uiMsgLen;
    PDU* pdu = (PDU*)malloc(uiPDULen); // PDU* pdu = (PDU*)malloc(uiMsgLen)错误！应分配uiPDULen
    if(pdu==NULL){
        qDebug()<<"申请PDU内存失败";
        exit(1);
    }
    memset(pdu,0,uiPDULen);
    pdu->uiMsgLen = uiMsgLen;
    pdu->uiPDULen = uiPDULen;
    return pdu;
}
