#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class Flickable;

class FileBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit FileBrowser(const QString &filter, QWidget *parent = NULL);
    virtual ~FileBrowser();
    void setDir(const QString &path);

signals:
    void picked(const QString &fileName);

private slots:
    void openFile(QListWidgetItem *item);
    void openDir(QListWidgetItem *item);

private:
    QString nameFilter_;
    QString basePath_;
    enum {PDF = 0, FOLDER, UP, IMG_TABLE_LEN};
    QPixmap pixmapTable_[IMG_TABLE_LEN];
    static const QSize thumbnailSize_;
    QListWidget *listWidget_;
    Flickable *flickable_;
};

#endif // FILEBROWSER_H
