#include "operatedb.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
//连接数据库
OperateDb &OperateDb::getInstance()
{
    static OperateDb instance;
    return instance;
}

void OperateDb::connectDb(const QString& host, quint16 port, const QString& dbName,
                          const QString& user, const QString& pwd)
{
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbName);
    m_db.setUserName(user);
    m_db.setPassword(pwd);
    if(m_db.open()){
        qDebug() << "数据库连接成功";
    }else{
        qDebug() << "数据库连接失败" << m_db.lastError().text();
    }
}

bool OperateDb::handleRegist(const char * caName,const char* caPwd)
{
    if(caName == NULL || caPwd == NULL){
        return false;
    }
    //判断要添加用户是否已经存在，构建sql,用QSqlQery对象调用
    QString sql = QString("select * from user_info where name = '%1'").arg(caName);
    qDebug()<<"handleRegist 判断用户是否存在"<<sql;
    QSqlQuery q;
    //执行sql语句，返回代表执行成功或失败
    if(!q.exec(sql)){
        return false;
    }
    //next取结果集中下一个结果，第一次就调用第一个，返回是否存在
    if(q.next()){
        return false;
    }
    //插入新用户
    sql=QString("insert into user_info(name,pwd) values('%1','%2')").arg(caName).arg(caPwd);
    qDebug()<<"handleRegist 添加用户"<<sql;
    return q.exec(sql);
}

bool OperateDb::handleLogin(const char *caName, const char *caPwd)
{
    if(caName == NULL || caPwd == NULL){
        return false;
    }
    //判断要添加用户是否已经存在，构建sql,用QSqlQery对象调用
    QString sql = QString("select * from user_info where name = '%1' and pwd ='%2'").arg(caName).arg(caPwd);
    qDebug()<<"handleLogin 判断登录的用户名及密码是否正确"<<sql;
    QSqlQuery q;
    //执行sql语句，返回代表执行成功或失败
    if(!q.exec(sql)||!q.next()){
        return false;
    }
    sql=QString("update user_info set online=1 where name = '%1' and pwd ='%2'").arg(caName).arg(caPwd);
    qDebug()<<"handleLogin 用户online置为1："<<sql;
    return q.exec(sql);
}

void OperateDb::handleOffline(const char *caName)
{
    if(caName == NULL){
        return;
    }
    QSqlQuery q;
    QString sql=QString("update user_info set online=0 where name = '%1'").arg(caName);
    qDebug()<<"handleoffline 用户online置为0："<<sql;
    q.exec(sql);
}

int OperateDb::handleFindUser(const char *caName)
{
    if(caName == NULL){
        return -1;
    }
    //判断用户是否存在，构建sql,用QSqlQery对象调用
    QString sql = QString("select online from user_info where name = '%1'").arg(caName);
    qDebug()<<"handleFindUser 判断用户是否存在且在线"<<sql;
    QSqlQuery q;
    //执行sql语句，返回代表执行成功或失败
    if(!q.exec(sql)){
        return false;
    }
    //next取结果集中下一个结果，第一次就调用第一个，返回是否存在
    if(q.next()){
        return q.value(0).toInt();
    }
    return 2;
}

QStringList OperateDb::handleOnlineUser()
{
    QString sql = QString("select name from user_info where online = 1");
    qDebug()<<"handleRegist 获取在线用户"<<sql;
    QSqlQuery q;
    QStringList result;
    //执行sql语句，返回代表执行成功或失败
    if(!q.exec(sql)){
        return result;
    }
    while(q.next()){
        result.append(q.value(0).toString());
    }
    return result;
}


OperateDb::~OperateDb()
{
    m_db.close();
}

int OperateDb::handleAddFirend(const char *caCurName, const char *caTarName)
{
    if(caCurName == NULL || caTarName == NULL){
        return -1;
    }
    QString sql = QString(R"(
                          select * from friend
                          where(
                          user_id = ( select id from user_info where name = '%1')
                          and
                          friend_id = ( select id from user_info where name = '%2')
                          )
                          or(
                          friend_id = ( select id from user_info where name = '%1')
                          and
                          user_id = ( select id from user_info where name = '%2')
                          )
                          )").arg(caCurName).arg(caTarName);
    qDebug()<<"handleAddFirend 是否为好友"<<sql;
    QSqlQuery q;
    q.exec(sql);
    if(q.next()){
        return 2;
    }
    sql = QString("select online from user_info where name = '%1'").arg(caTarName);
    qDebug()<<"handleAddFirend 目标用户是否在线"<<sql;
    q.exec(sql);
    if(q.next()){
        return q.value(0).toInt();//返回1是为正常
    }
    return -1;
}

bool OperateDb::handleAddFirendArgee(const char *caCurName, const char *caTarName)
{
    if(caCurName == NULL || caTarName == NULL){
        return false;
    }
    QString sql = QString(R"(
                          insert into friend(user_id,friend_id)
                          select u1.id, u2.id
                          from user_info u1,user_info u2
                          where u1.name = '%1' and u2.name = '%2'
                          )").arg(caCurName).arg(caTarName);
     qDebug()<<"handleAddFirendArgee 添加好友记录"<<sql;
     QSqlQuery q;
     return q.exec(sql);
}

QStringList OperateDb::handleFlushFriend(const char *caCurName)
{
    QStringList res;
    if(caCurName == NULL){
        return res;
    }
    QString sql = QString(R"(select name from user_info
                          where id in
                          (
                          select friend_id from friend
                          where user_id = (select id from user_info where name='%1')
                          union
                          select user_id from friend
                          where friend_id = (select id from user_info where name='%1')
                          ))").arg(caCurName);
    QSqlQuery q;
    q.exec(sql);
    while(q.next()){
        res.append(q.value(0).toString());
    }
    qDebug()<<"handleFlushFriend 刷新好友";
    return res;
}

bool OperateDb::handleDelFirend(const char *caCurName, const char *caTarName)
{
    if(caCurName == NULL || caTarName == NULL){
        return false;
    }
    QString sql = QString(R"(
                          select * from friend
                          where(
                          user_id = ( select id from user_info where name = '%1')
                          and
                          friend_id = ( select id from user_info where name = '%2')
                          )
                          or(
                          friend_id = ( select id from user_info where name = '%1')
                          and
                          user_id = ( select id from user_info where name = '%2')
                          )
                          )").arg(caCurName).arg(caTarName);
    qDebug()<<"handleDelFirend 是否为好友"<<sql;
    QSqlQuery q;
    q.exec(sql);
    if(!q.next()){
        return false;
    }
    sql = QString(R"(
                  delete from friend
                  where(
                  user_id = ( select id from user_info where name = '%1')
                  and
                  friend_id = ( select id from user_info where name = '%2')
                  )
                  or(
                  friend_id = ( select id from user_info where name = '%1')
                  and
                  user_id = ( select id from user_info where name = '%2')
                  )
                  )").arg(caCurName).arg(caTarName);
    qDebug()<<"handleDelFirend 删除好友关系"<<sql;
    return q.exec(sql);
}

OperateDb::OperateDb(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
}
