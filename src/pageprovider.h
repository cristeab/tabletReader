/****************************************************************************
**
** Copyright (C) 2013, Bogdan Cristea. All rights reserved.
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

#ifndef PAGEPROVIDER_H
#define PAGEPROVIDER_H

#include <QDebug>
#include <QEventLoop>
#ifdef QT5
#include <QQuickImageProvider>
#else
#include <QDeclarativeImageProvider>
#endif
#include "okulardocument.h"

#ifdef QT5
class PageProvider : public QObject, public QQuickImageProvider
#else
class PageProvider : public QObject, public QDeclarativeImageProvider
#endif
{
  Q_OBJECT

signals:
  void pageRequest(int page, qreal factor);

public slots:
  void onPageReady(int page, const OkularDocument::PageContentType *pix);

public:
  PageProvider();
  ~PageProvider();
#ifdef POPPLER_BACKEND
  QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
#else
  QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
#endif
  bool setDocument(const QString &filePath);
  void setScale(qreal scale, int index) {
    scaleFactor_ = scale;
    scaleIndex_ = index;
  }
  qreal scaleFactor() const {
    return scaleFactor_;
  }
  int scaleIndex() const {
    return scaleIndex_;
  }
  int currentPage() const {
    return currentPage_;
  }
  uint numPages() const {
    return (NULL != doc_) ? doc_->numPages() : 0;
  }
  const QString& filePath() const {
    return filePath_;
  }
  bool isLoaded() const {
    return (doc_ != NULL);
  }
  bool cacheReady() const {
    if (requestingPixmap_) {
      return false;
    }
    for (int n = 0; n < cacheSize_; ++n) {
      if (PageCache::VALID != pageCache_[n].status) {
        return false;
      }
    }
    return true;
  }
  void setWinWidth(int width) {
    if (NULL != doc_) {
      doc_->setWinWidth(width);
    }
  }
  const QStringList& supportedFilePatterns() const {
    return (NULL != doc_)?doc_->supportedFilePatterns():emptyStringList_;
  }
  void gotoPage(uint page);
private:
  bool invalidatePageCache(uint page) {
    if((NULL == doc_) || (numPages() <= page)) {
      return false;//operation failed
    }
    doc_->deletePage(pageCache_[page % CACHE_SIZE].page);
    pageCache_[page % CACHE_SIZE].page = NULL;
    pageCache_[page % CACHE_SIZE].status = PageCache::INVALID;
    return true;//operation successful
  }
  enum {CACHE_SIZE = 3};
  void sendPageRequest(uint page) {
    if (true == invalidatePageCache(page)) {
      pageCache_[page % CACHE_SIZE].status = PageCache::PENDING;
      emit pageRequest(page, scaleFactor_);
    }
  }
  void updateCache();
  OkularDocument *doc_;
  int currentPage_;
  int prevPage_;
  qreal scaleFactor_;
  int scaleIndex_;
  struct PageCache {
	PageCache() : page(NULL), status(INVALID) {}
	enum Status {INVALID, VALID, PENDING};
	const OkularDocument::PageContentType *page;
    Status status;
	~PageCache() {
	  delete page;
	}
  };
  QVector<PageCache> pageCache_;
  QString filePath_;
  bool requestingPixmap_;//needed to block pixmap requests when inside pixmapRequest method
  QEventLoop evt_;//used for synchronous page requests
  int cacheSize_;//depends on the default cache size and the number of pages of the document
  QStringList emptyStringList_;
};

#endif
