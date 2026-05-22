#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"
#include "reshandler.h"

#include <QTcpSocket>
#include <QWidget>
#include <QHostAddress>
//结构体头文件

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:
    ~Client();
    void loadConfig();//创建加载配置函数
    QTcpSocket m_socket;//创建套接字管道对象
    static Client& getInstance();
    QString m_strUserName;
    void sendMsg(PDU*pdu);//包装发送函数
    PDU* readMsg();//读取消息，用pdu
    void handleMsg(PDU* pdu);
    ResHandler* m_prh;//响应处理
    QString m_strRootPath;
    QByteArray buffer;//消息缓存池（数组）
    void startUpload();//上传函数
    void showError(QString msg);

public slots:
    void showConnect();//查看链接
    //    void on_send_PB_clicked();
    void on_regist_PB_clicked();
    void recvMsg();//接收消息的函数
    void on_login_PB_clicked();

private:
    Client(QWidget *parent = nullptr);//创建主窗口指针对象初始化为空
    Client(const Client& instance)=delete;//单例实现，删除拷贝构造
    Client& operator=(const Client&)=delete;//单例实现，删除拷贝赋值运算符

    Ui::Client *ui;//创建窗口ui指针对象
    QString m_strIP;//创建的私有成员变量IP地址
    quint16 m_usPort;//端口号

};
#endif // CLIENT_H
