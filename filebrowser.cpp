#include "filebrowser.h"
#include <QDir>
#include <QCoreApplication>
#include <QGridLayout>
#include <QScrollBar>
#include <QDebug>

const QSize FileBrowser::thumbnailSize_ = QSize(50, 50);

FileBrowser::FileBrowser(const QString &filter, QWidget *parent)
    : QDialog(parent)
{
    nameFilter_ = filter;
    appDirPath_ = QCoreApplication::applicationDirPath();
    listWidget_ = new QListWidget(this);
    listWidget_->setIconSize(thumbnailSize_);
    listWidget_->verticalScrollBar()->setStyleSheet("width: 40px");
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(listWidget_, 0, 0, 1, 1);
    setLayout(gridLayout);
    setDir(appDirPath_);
    connect(listWidget_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openFile(QListWidgetItem*)));
    connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(openDir(QListWidgetItem*)));
}

FileBrowser::~FileBrowser()
{
    delete image_;
    image_ = NULL;
}

void FileBrowser::setDir(const QString &path)
{
    QDir dir(path, nameFilter_, QDir::DirsFirst | QDir::Name, QDir::AllDirs | QDir::Files);
    if (!dir.isReadable()) {
        return;
    }    

    basePath_ = dir.canonicalPath();

    //load icons for future reuse    
    image_ = new QImage(thumbnailSize_, QImage::Format_RGB32);
    if (false == image_->load(appDirPath_+"/icons/Adobe-PDF-Document-icon.png")) {
        qDebug() << "Cannot load image";
    }
    imageTable_.insert(PDF, *image_);
    if (false == image_->load(appDirPath_+"/icons/My-Ebooks-icon.png")) {
        qDebug() << "Cannot load image";
    }
    imageTable_.insert(FOLDER, *image_);
    if (false == image_->load(appDirPath_+"/icons/Button-Upload-icon.png")) {
        qDebug() << "Cannot load image";
    }
    imageTable_.insert(UP, *image_);

    QStringList entries = dir.entryList();
    QStringList::ConstIterator it = entries.constBegin();
    QListWidgetItem* listItem = NULL;
    listWidget_->clear();
    bool flipFlop = true;
    while (it != entries.constEnd()) {
        if (*it != ".") {
            listItem = new QListWidgetItem(*it);
            if (".." == *it) {
                listItem->setIcon(QPixmap::fromImage(imageTable_[UP]));
            } else {
                if (QFileInfo(basePath_ + "/" + *it).isFile()) {
                    listItem->setIcon(QPixmap::fromImage(imageTable_[PDF]));
                } else {
                    listItem->setIcon(QPixmap::fromImage(imageTable_[FOLDER]));
                }
            }
            listItem->setBackgroundColor(flipFlop?Qt::white:Qt::lightGray);
            flipFlop ^= true;
            listWidget_->addItem(listItem);
        }
        ++it;
    }    
}

void FileBrowser::openFile(QListWidgetItem *item)
{
    QString fileName = basePath_ + "/" + item->text();
    if (QFileInfo(fileName).isFile())
    {
        emit picked(fileName);
    }
}

void FileBrowser::openDir(QListWidgetItem *item)
{
    QString path = basePath_ + "/" + item->text();
    if (QFileInfo(path).isDir()) {
        setDir(path);
    }
}
