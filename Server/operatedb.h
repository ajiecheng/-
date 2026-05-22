#ifndef OPERATEDB_H
#define OPERATEDB_H
//连接数据库，数据库操作类
#include <QObject>
#include <QSqlDatabase>
class OperateDb : public QObject
{
    Q_OBJECT
public:
    static OperateDb& getInstance();
    void connectDb(const QString& host, quint16 port, const QString& dbName,
                   const QString& user, const QString& pwd);
    bool handleRegist(const char * caName, const char * caPwd);
    bool handleLogin(const char * caName, const char * caPwd);
    void handleOffline(const char * caName);
    int handleFindUser(const char * caName);
    QStringList handleOnlineUser();
    ~OperateDb();
     int handleAddFirend(const char * caCurName, const char * caTarName);
     bool handleAddFirendArgee(const char * caCurName, const char * caTarName);
     QStringList handleFlushFriend(const char *caCurName);
     bool handleDelFirend(const char * caCurName, const char * caTarName);


private:
    explicit OperateDb(QObject *parent = nullptr);
    OperateDb(const OperateDb& instance) = delete;
    OperateDb& operator=(const OperateDb&) = delete;
    QSqlDatabase m_db;

signals:

};

#endif // OPERATEDB_H
