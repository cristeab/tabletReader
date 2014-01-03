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

#include <QtGui>
#ifndef QT5
#include <kmimetype.h>
#endif
#include "pageprovider.h"
#include "window.h"

PageProvider::PageProvider()
    : QObject(),
#ifdef POPPLER_BACKEND
    QQuickImageProvider(QQmlImageProviderBase::Image, QQmlImageProviderBase::ForceAsynchronousImageLoading),
#else
    QDeclarativeImageProvider(QDeclarativeImageProvider::Pixmap),
#endif
    doc_(new OkularDocument()),
    currentPage_(-1),
    prevPage_(-1),
    scaleFactor_(0),
	scaleIndex_(0),
	pageCache_(CACHE_SIZE),
    requestingPixmap_(false),
	cacheSize_(0)
{
  connect(this, SIGNAL(pageRequest(int, qreal)), doc_, SLOT(onPageRequest(int, qreal)));
  connect(doc_, SIGNAL(pageReady(int, const OkularDocument::PageContentType*)),
	  this, SLOT(onPageReady(int, const OkularDocument::PageContentType*)));
}

PageProvider::~PageProvider()
{
  delete doc_;
}

void PageProvider::onPageReady(int page, const OkularDocument::PageContentType *pix)
{
  qDebug() << "PageProvider::onPageReady" << page;

  pageCache_[page % CACHE_SIZE].page = pix;
  pageCache_[page % CACHE_SIZE].status = PageCache::VALID;
  if (evt_.isRunning()) {
    evt_.quit();//this is a synchronous request
  }
}

#ifdef POPPLER_BACKEND
QImage PageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
#else
QPixmap PageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
#endif
{
  qDebug() << "PageProvider::request[Pixmap|Image]" << id;
  requestingPixmap_ = true;//needed to make sure that a second request does not come to quickly

  prevPage_ = currentPage_;
  currentPage_ = id.toInt();
  int width = 100;//default size
  int height =100;

  //the document is not loaded or the cache is not valid
  //the GUI should make sure that the cache is valid before issuing a request
  if ((NULL == doc_) || (0 == doc_->numPages()) ||
  (PageCache::VALID != pageCache_[currentPage_%CACHE_SIZE].status)) {
    //return a default pixmap
    qDebug() << "no document or invalid cache";
    if (size) {
      *size = QSize(width, height);
    }
    requestingPixmap_ = false;
#ifdef POPPLER_BACKEND
	return QImage(width,height, QImage::Format_RGB32);
#else
    return QPixmap(width,height);
#endif
  }

  qDebug() << "valid cache" << currentPage_;
  const OkularDocument::PageContentType *pPix = pageCache_[currentPage_%CACHE_SIZE].page;
  bool isValid = ((NULL != pPix) && (false == pPix->isNull()));
  if (true == isValid) {
    width = requestedSize.width() > 0 ? requestedSize.width() : pPix->width();
    height = requestedSize.height() > 0 ? requestedSize.height() : pPix->height();
    qDebug() << "width" << width << ", height" << height;
  }
  if (size) {
    *size = QSize(width, height);
  }

  updateCache();

  requestingPixmap_ = false;
  return (true == isValid)?*pPix:OkularDocument::PageContentType();
}

bool PageProvider::setDocument(const QString &filePath)
{
  qDebug() << "PageProvider::setDocument" << filePath;

  bool out = false;
  if (NULL != doc_) {
    if(true == doc_->load(filePath)) {
      cacheSize_ = qMin(static_cast<uint>(pageCache_.count()), numPages());
      currentPage_ = -1;
      filePath_ = filePath;
      out = true;
    }
    else if (!filePath_.isEmpty()) {
      //an error occured -> restore previous document
      out = (EXIT_SUCCESS == doc_->load(filePath_));
    }
  }
  return out;
}

void PageProvider::updateCache()
{
  qDebug() << "PageProvider::updateCache: prev" << prevPage_ << ", current" << currentPage_;

  if (prevPage_ == currentPage_) {
    qDebug() << "same page";
  }
  else if (1 == (currentPage_-prevPage_)) {
    //update next page
    qDebug() << "next page";
    sendPageRequest(currentPage_+1);
  }
  else if (-1 == (currentPage_-prevPage_)) {
    //update prev page
    qDebug() << "prev page";
    sendPageRequest(currentPage_-1);
  }
}

void PageProvider::gotoPage(uint page)
{
  qDebug() << "PageProvider::gotoPage";
  if (numPages() <= page) {
	qDebug() << "Invalid page number" << page;
	return;
  }

  //request synchronously the current page
  sendPageRequest(page);
  if (PageCache::VALID != pageCache_[page % CACHE_SIZE].status) {
    evt_.exec();
  }
  //asynchronous requests (make sure that the entire cache is marked as filled)
  if (1 <= page) {
    sendPageRequest(page-1);
  }
  else if ((numPages() > (page+2))) {
    pageCache_[(page+2) % CACHE_SIZE].status = PageCache::VALID;
  }
  if (numPages() > (page+1)) {
    sendPageRequest(page+1);
  }
  else if (2 <= page) {
    pageCache_[(page-2) % CACHE_SIZE].status = PageCache::VALID;
  }
}
