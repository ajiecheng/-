#ifndef FILE_H
#define FILE_H

#include "protocol.h"
#include "sharefile.h"

#include <QListWidget>
#include <QWidget>

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    QString m_strUserPath;
    QString m_strCurPath;
    QString m_strUploadFilePath;
    void flushFile();
    void updateFileList(QList<FileInfo*> pFileList);
    QList<FileInfo*> m_pFileInfoList;
    void uploadFile();
    QString m_strShareFileName;
    ShareFile* m_pShareFile;


private slots:
    void on_mkdir_PB_clicked();

    void on_flushFile_PB_clicked();

    void on_deldir_PB_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_return_PB_clicked();

    void on_uploadFile_PB_clicked();

    void on_shareFile_PB_clicked();

private:
    Ui::File *ui;
};

#endif // FILE_H
