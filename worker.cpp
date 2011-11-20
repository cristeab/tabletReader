#include <QProgressDialog>
#include <QDebug>
#include "worker.h"
#include "documentwidget.h"

Worker::Worker(QWidget *parent, DocumentWidget *doc) :
    QThread(parent),
    doc_(doc)
{
    setTerminationEnabled(true);
}

void Worker::updateCache(int page)
{
    qDebug() << "Worker::updateCache begin" << page;
    doc_->cacheMutex_.lock();
    if (false == doc_->pageCache_[page%DocumentWidget::CACHE_SIZE]->valid) {
        doc_->loadImage(page);
    } else {
        qDebug() << "Worker::updateCache: nothing to do";
    }
    doc_->cacheMutex_.unlock();
    qDebug() << "Worker::updateCache end";
}

void Worker::run()
{
    exec();
}
