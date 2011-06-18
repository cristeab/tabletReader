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

DocumentWidget::DocumentWidget(QWidget */*parent*/)
    : doc_(NULL),
      currentPage_(-1),
      maxNumPages_(0),
      scaleFactor_(1.0),
      physicalDpiX_(0), physicalDpiY_(0)
{
    for (int n = 0; n < BUFFER_LEN; ++n)
    {
        images_.append(new QImage);
    }
}

DocumentWidget::~DocumentWidget()
{
    for (int n = 0; n < images_.size(); ++n)
    {
        delete images_[n];
    }
    delete doc_;
}

qreal DocumentWidget::scale() const
{
    return scaleFactor_;
}

void DocumentWidget::loadImage(unsigned int page)
{
    if (page >= maxNumPages_)
    {
        return;
    }

    (*images_[page%BUFFER_LEN]) = doc_->page(page)
                      ->renderToImage(scaleFactor_ * physicalDpiX_,
                                      scaleFactor_ * physicalDpiY_);
}

void DocumentWidget::showPage(int page)
{
    if (page != -1 && page != currentPage_ + 1) {
        currentPage_ = page - 1;
        emit pageChanged(page);
    }

    loadImage(currentPage_);

    qDebug() << "current page " << currentPage_;
    QLabel *label = (QLabel*)scrollAreas_[currentPage_%BUFFER_LEN]->widget();
    qDebug() << "after current page";
    label->setPixmap(QPixmap::fromImage(*images_[currentPage_%BUFFER_LEN]));
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

void DocumentWidget::setScale(qreal scale)
{
    if (scaleFactor_ != scale) {
        scaleFactor_ = scale;
        showPage();
    }
}
