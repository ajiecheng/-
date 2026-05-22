#ifndef SERVER_H
#define SERVER_H
//服务器头文件
#include <QWidget>

class Server : public QWidget
{
    Q_OBJECT

public:
    ~Server();
    void loadConfig();//加载配置
    QString m_strIP;
    quint16 m_usPort;
    QString m_strRootPath;
    // 数据库配置
    QString m_strDbHost;
    quint16 m_usDbPort;
    QString m_strDbName;
    QString m_strDbUser;
    QString m_strDbPwd;
    static Server& getInstance();
private:
    Server(QWidget *parent = nullptr);
    Server(const Server& instance)=delete;
    Server& operator=(const Server&)=delete;

};
#endif // SERVER_H
