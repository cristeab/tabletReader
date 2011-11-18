#include "filebrowser.h"
#include <QDir>
#include <QCoreApplication>
#include <QGridLayout>
#include <QScrollBar>
#include <QListWidgetItem>
#include <flickable.h>
#include <QDebug>

const QSize FileBrowser::thumbnailSize_ = QSize(50, 50);

FileBrowser::FileBrowser(const QString &filter, QWidget *parent)
    : QDialog(parent)
{
    nameFilter_ = filter;
    listWidget_ = new QListWidget(this);
    listWidget_->setIconSize(thumbnailSize_);
    listWidget_->verticalScrollBar()->setStyleSheet("width: 40px");
    flickable_ = new Flickable(this);
    flickable_->activateOn(listWidget_);
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->addWidget(listWidget_, 0, 0, 1, 1);
    setLayout(gridLayout);    
    connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this,
            SLOT(openFile(QListWidgetItem*)));
    connect(listWidget_, SIGNAL(itemClicked(QListWidgetItem*)), this,
            SLOT(openDir(QListWidgetItem*)));

    //load icons
    QImage image = QImage(thumbnailSize_, QImage::Format_RGB32);
    if (false == image.load(":/filebrowser/icons/Adobe-PDF-Document-icon.png")) {
        qDebug() << "FileBrowser::setDir: Cannot load document image";
    }
    pixmapTable_[PDF] = QPixmap::fromImage(image);

    image = QImage(thumbnailSize_, QImage::Format_RGB32);
    if (false == image.load(":/filebrowser/icons/My-Ebooks-icon.png")) {
        qDebug() << "FileBrowser::setDir: Cannot load folder image";
    }
    pixmapTable_[FOLDER] = QPixmap::fromImage(image);

    image = QImage(thumbnailSize_, QImage::Format_RGB32);
    if (false == image.load(":/filebrowser/icons/Button-Upload-icon.png")) {
        qDebug() << "FileBrowser::setDir: Cannot load up one level image";
    }
    pixmapTable_[UP] = QPixmap::fromImage(image);

    setDir(QDir::homePath());
}

FileBrowser::~FileBrowser()
{
}

void FileBrowser::setDir(const QString &path)
{
    QDir dir(path, nameFilter_, QDir::DirsFirst | QDir::Name | QDir::IgnoreCase |
             QDir::LocaleAware, QDir::AllDirs | QDir::Files);
    if (!dir.isReadable()) {
        //TODO: display message box
        return;
    }    

    basePath_ = dir.canonicalPath();
    setWindowTitle("Current folder: " + basePath_);

    //show folder list
    QStringList entries = dir.entryList();
    QStringList::ConstIterator it = entries.constBegin();
    QListWidgetItem* listItem = NULL;
    listWidget_->clear();
    bool flipFlop = true;
    while (it != entries.constEnd()) {
        if (*it != ".") {
            listItem = new QListWidgetItem(*it);
            if (".." == *it) {
                listItem->setIcon(pixmapTable_[UP]);
            } else {
                if (QFileInfo(basePath_ + QDir::toNativeSeparators("/") + *it).isFile()) {
                    listItem->setIcon(pixmapTable_[PDF]);
                } else {
                    listItem->setIcon(pixmapTable_[FOLDER]);
                }
            }
            listItem->setBackgroundColor(flipFlop?Qt::white:Qt::lightGray);
            flipFlop ^= true;
            listWidget_->addItem(listItem);
        }
        ++it;
    }
    listWidget_->verticalScrollBar()->setValue(0);//upper view
}

void FileBrowser::openFile(QListWidgetItem *item)
{
    QString fileName = basePath_ + QDir::toNativeSeparators("/") + item->text();
    if (QFileInfo(fileName).isFile())
    {
        emit picked(fileName);
    }
}

void FileBrowser::openDir(QListWidgetItem *item)
{
    QString path = basePath_ + QDir::toNativeSeparators("/") + item->text();
    if (QFileInfo(path).isDir()) {
        setDir(path);
    }
}
