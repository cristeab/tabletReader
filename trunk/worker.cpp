/****************************************************************************
**
** Copyright (C) 2011, Bogdan Cristea. All rights reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2, or (at your option)  any later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
** Street - Fifth Floor, Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include <QProgressDialog>
#include <QDebug>
#include "worker.h"
#include "documentwidget.h"

Worker::Worker(QWidget *parent, DocumentWidget *doc) :
    QThread(parent),
    doc_(doc)
{
    setTerminationEnabled(true);
    setPriority(IdlePriority);
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
