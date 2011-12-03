/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#include <QtGui>
#include <poppler-qt4.h>
#include "documentwidget.h"
#include "SlidingStackedWidget.h"

DocumentWidget::DocumentWidget(QWidget */*parent*/)
    : doc_(NULL),
      currentPage_(-1),
      currentIndex_(-1),
      maxNumPages_(0),
      scaleFactor_(1.0),
      stackedWidget_(NULL),
      currentScrollArea_(NULL),
      physicalDpiX_(0), physicalDpiY_(0)
{
    for (int n = 0; n < CACHE_SIZE; ++n)
    {
        pageCache_.append(new ImageCache);
        pageCache_[n]->valid = false;
    }
}

DocumentWidget::~DocumentWidget()
{
    for (int n = 0; n < pageCache_.size(); ++n)
    {
        delete pageCache_[n];
    }
    delete doc_;
}

void DocumentWidget::loadImage(int page)
{
    qDebug() << "DocumentWidget::loadImage begin";
    if ((0 > page) || (maxNumPages_ <= page))
    {
        qDebug() << "DocumentWidget::loadImage: nothing to do";
        return;
    }

    pageCache_[page%CACHE_SIZE]->image = doc_->page(page)
                      ->renderToImage(scaleFactor_ * physicalDpiX_,
                                      scaleFactor_ * physicalDpiY_);
    pageCache_[page%CACHE_SIZE]->valid = true;
    qDebug() << "DocumentWidget::loadImage end";
}

void DocumentWidget::showPage(int page)
{
    qDebug() << "DocumentWidget::showPage" << page;
    if (page != -1) {//if we show the same page nothing should be done, but this verification is done by the routines above
        currentIndex_ = stackedWidget_->currentIndex();
        if (NULL != currentScrollArea_) { //do nothing if no page has been loaded
            if (currentPage_ <= (page - 1)) {
                ++currentIndex_;
                if (CACHE_SIZE == currentIndex_) {
                    currentIndex_ = 0;
                }
            } else {
                --currentIndex_;
                if (currentIndex_ < 0) {
                    currentIndex_ = CACHE_SIZE-1;
                }
            }
        }
        currentPage_ = page - 1;
    }

    //set image on the scroll area
    currentScrollArea_ = (QScrollArea*)stackedWidget_->widget(currentIndex_);//get next/prev widget
    QLabel *label = (QLabel*)currentScrollArea_->widget();
    cacheMutex_.lock();
    if (false == pageCache_[currentPage_%CACHE_SIZE]->valid) {
        qDebug() << "DocumentWidget::showPage: invalid cache";
        loadImage(currentPage_);//load image into memory
        pageCache_[currentPage_%CACHE_SIZE]->valid = true;
    } else {
        qDebug() << "DocumentWidget::showPage: valid cache";
    }
    qDebug() << "DocumentWidget::showPage: begin setPixmap";
    label->setPixmap(QPixmap::fromImage(pageCache_[currentPage_%CACHE_SIZE]->image));
    qDebug() << "DocumentWidget::showPage: end setPixmap";
    cacheMutex_.unlock();
    //label->update();//TODO: use repaint

    emit pageLoaded(page);//change value in the spin box
}

bool DocumentWidget::setDocument(const QString &filePath)
{
    Poppler::Document *oldDocument = doc_;

    doc_ = Poppler::Document::load(filePath);
    if (doc_) {
        delete oldDocument;
        doc_->setRenderHint(Poppler::Document::Antialiasing);
        doc_->setRenderHint(Poppler::Document::TextAntialiasing);
        maxNumPages_ = doc_->numPages();
        currentPage_ = -1;
    }
    return doc_ != 0;
}

void DocumentWidget::setPage(int page)
{
    if (page != currentPage_ + 1)
    {
        showPage(page);
    }
}
