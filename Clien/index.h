#ifndef INDEX_H
#define INDEX_H

#include "file.h"
#include "friend.h"

#include <QWidget>

namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:
    static Index& getInstance();
    ~Index();
    Friend * getFriend();
    File* getFile();

private slots:
    void on_firend_PB_clicked();
    void on_file_PB_clicked();

private:
    explicit Index(QWidget *parent = nullptr);
    Index(const Index& instance)=delete;//拷贝构造
    Index& operator=(const Index&)=delete;//赋值
    Ui::Index *ui;
};

#endif // INDEX_H
