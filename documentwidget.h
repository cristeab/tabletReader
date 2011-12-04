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

#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

#include <QLabel>
#include <QRectF>
#include <QScrollArea>
#include <QScrollBar>
#include <QMutex>
#include <QDebug>
#include <poppler-qt4.h>

class SlidingStackedWidget;
class ProgressDlg;

class DocumentWidget : public QObject
{
    Q_OBJECT

    friend class Worker;

public:
    DocumentWidget(QWidget *parent = 0);
    ~DocumentWidget();
    qreal scale() const
    {
        return scaleFactor_;
    }
    int currentPage() const
    {
        return currentPage_;
    }
    int currentIndex() const
    {
        return currentPage_%CACHE_SIZE;
    }
    int numPages() const
    {
        return doc_->numPages();
    }
    bool isLoaded()
    {
        return (doc_ != NULL);
    }
    void setPhysicalDpi(int physicalDpiX, int physicalDpiY)
    {
        physicalDpiX_ = physicalDpiX;
        physicalDpiY_ = physicalDpiY;
    }
    void setStackedWidget(SlidingStackedWidget *stackedWidget)
    {
        stackedWidget_ = stackedWidget;
    }

    void showCurrentPageUpper()
    {
        if (NULL != currentScrollArea_)
        {
            currentScrollArea_->verticalScrollBar()->setValue(0);
        }
    }
    void showCurrentPageLower()
    {
        if (NULL != currentScrollArea_)
        {
            QScrollBar *scroll = currentScrollArea_->verticalScrollBar();
            scroll->setValue(scroll->maximum());
        }
    }
    void invalidateCache(void)
    {
        qDebug() << "DocumentWidget::invalidateCache";
        cacheMutex_.lock();
        for (int n = 0; n < CACHE_SIZE; ++n) {
            pageCache_[n]->valid = false;
        }
        cacheMutex_.unlock();
    }
    bool invalidatePageCache(int page)
    {
        qDebug() << "DocumentWidget::invalidatePageCache" << page;
        if (0 > page || maxNumPages_ <= page) {
            qDebug() << "DocumentWidget::invalidatePageCache: nothing to do";
            return false;//operation failed
        }
        cacheMutex_.lock();
        pageCache_[page%CACHE_SIZE]->valid = false;
        cacheMutex_.unlock();
        return true;//operation successful
    }

    void loadImage(int page);

    enum {CACHE_SIZE = 3};

    bool loadFromData(const QByteArray &fileContents);

public slots:
    bool setDocument(const QString &filePath);
    void setPage(int page = -1);
    void setScale(qreal scale)
    {
        scaleFactor_ = scale;
    }

signals:
    void pageLoaded(int currentPage);

public:
    void showPage(int page = -1);    
private:
    Poppler::Document *doc_;
    int currentPage_;
    int currentIndex_;
    int maxNumPages_;
    qreal scaleFactor_;
    struct ImageCache {
        QImage image;
        bool valid;
    };
    QList<ImageCache*> pageCache_;
    QMutex cacheMutex_;//used to protect the access to image list
    SlidingStackedWidget *stackedWidget_;
    QScrollArea *currentScrollArea_;
    int physicalDpiX_;
    int physicalDpiY_;
};

#endif
