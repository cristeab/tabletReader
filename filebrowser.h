#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QListWidget>
#include <QDialog>

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
    QString appDirPath_;
    QHash<int, QImage> imageTable_;
    enum {PDF, FOLDER, UP};
    static const QSize thumbnailSize_;
    QImage *image_;
    QListWidget *listWidget_;
};

#endif // FILEBROWSER_H
