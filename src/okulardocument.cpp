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

#ifdef POPPLER_BACKEND
#include <poppler-qt5.h>
#else
#include <core/generator.h>
#include <core/settings_core.h>
#include <core/page.h>
#endif
#include "window.h"
#include "okulardocument.h"

#ifndef POPPLER_BACKEND
//main entry point into okular core libray
class PagePainter
{
public:
  PagePainter(Okular::Document *doc, OkularDocument *obs):
    doc_(doc), obs_(obs)
  {}
  ~PagePainter() {
  }
  //send request for page pixmap
  void sendRequest(const Okular::Page *page, int width, int height)
  {
    qDebug() << "PagePainter::sendRequest";
    if(false == page->hasPixmap(obs_)) {
      qDebug() << "making pixmap request";
    Okular::PixmapRequest *pixmapRequest = new Okular::PixmapRequest(obs_, page->number(), width, height, 0, Okular::PixmapRequest::Asynchronous);//the request is deleted by okular core library
      req_.clear();
      req_.push_back(pixmapRequest);
      doc_->requestPixmaps(req_);
    }
  }
  //get page pixmap when notifyPageChanged() is called
  const QPixmap* getPagePixmap(int page) const 
  {
    qDebug() << "PagePainter::getPagePixmap";
    const QPixmap *pix = NULL;
    if (NULL != doc_) {
      const Okular::Page *p = doc_->page(page);
      if (NULL != p) {
        pix = p->_o_nearestPixmap(obs_, -1, -1);
      }
    }
    return pix;
  }
private:
  Okular::Document *doc_;
  QLinkedList<Okular::PixmapRequest*> req_;
  OkularDocument *obs_;
};
#endif

OkularDocument::OkularDocument() :
  doc_(NULL),
#ifndef POPPLER_BACKEND
  painter_(NULL),
#endif
  winWidth_(0),
  zoomFactor_(0)
  {
#ifndef POPPLER_BACKEND
  Okular::SettingsCore::instance("");//need to call this before creating the documnent
  doc_ = new Okular::Document(NULL);
  painter_ = new PagePainter(doc_, this);
  if(NULL != doc_) {
    doc_->addObserver(this);
  }
#endif
}

OkularDocument::~OkularDocument()
{
  delete doc_;
#ifndef POPPLER_BACKEND
  delete painter_;
#endif
}

bool OkularDocument::load(const QString &fileName)
{
  bool res = false;
#ifdef POPPLER_BACKEND
  delete doc_;//release previous document if any
  doc_ = Poppler::Document::load(fileName);
  if ((NULL == doc_) || doc_->isLocked()) {
    qDebug() << "Cannot load document or document is locked" << fileName;
    delete doc_;
	doc_ = NULL;
  } else {
    res = true;
  }
#else
  if (NULL != doc_) {
    doc_->closeDocument();//close previous document if any
    mimeType_ = KMimeType::findByPath(fileName);
    res = doc_->openDocument(fileName, KUrl::fromPath(fileName), mimeType_);
  }
#endif
  return res;
}

uint OkularDocument::numPages() const {
#ifdef POPPLER_BACKEND
  return (NULL != doc_)?static_cast<uint>(doc_->numPages()):0;
#else
  return (NULL != doc_)?doc_->pages():0;
#endif
}

#ifdef POPPLER_BACKEND
void OkularDocument::preProcessPage(int &width, int &height, const Poppler::Page *page)
{
  qreal factor = 0;
  if (Window::FIT_WIDTH_ZOOM_FACTOR == zoomFactor_) {
    //adjust scale factor to occupy the entire window width
    factor = qreal(winWidth_)/page->pageSize().width();
  } else {
    factor = zoomFactor_;
  }
  width = int(factor*(page->pageSize().width()));
  height = int(factor*(page->pageSize().height()));
}
#else
void OkularDocument::preProcessPage(int &width, int &height, const Okular::Page *page)
{
  qreal factor = 0;
  if (mimeType_->is("application/vnd.ms-htmlhelp") ||
    mimeType_->is("application/x-chm")) {
    //for chm docs request page with the true size (factor = 1)
    factor = 1.0;
  } else if (Window::FIT_WIDTH_ZOOM_FACTOR == zoomFactor_) {
    //adjust scale factor to occupy the entire window width
    factor = qreal(winWidth_)/page->width();
  } else {
    factor = zoomFactor_;
  }
  width = int(factor*(page->width()));
  height = int(factor*(page->height()));
  //adjust size in order to stay below this threshold (used by okular core library)
  const qint64 area = qint64(width)*qint64(height);
  if(area > 20000000L) {
    qDebug() << "adjust width and height in order to stay below threshold";
    qreal factor2 = qreal(20000000L)/qreal(area);
    height = int(height*factor2);
  }
}
#endif

const OkularDocument::PageContentType* OkularDocument::postProcessPage(const OkularDocument::PageContentType *pixmap)
{
  PageContentType *out = NULL;
#ifdef POPPLER_BACKEND
  out = new PageContentType(*pixmap);//this copy is needed for the upper layer
#else
  if(mimeType_->is("application/epub+zip") ||
      mimeType_->is("application/vnd.oasis.opendocument.text")) {
    //TODO: check if this can be done from okular core library
    out = new PageContentType(pixmap->width(), pixmap->height());
    if(NULL != out) {
      out->fill(Qt::white);
      QPainter p;
      p.begin(out);
      p.setBackgroundMode(Qt::OpaqueMode);
      p.drawPixmap(0, 0, *pixmap);
      p.end();
    }
  } else if (mimeType_->is("application/vnd.ms-htmlhelp") ||
    mimeType_->is("application/x-chm")) {
    //apply zoom on the obtained pixmap
    if (Window::FIT_WIDTH_ZOOM_FACTOR == zoomFactor_) {
      out = new PageContentType(pixmap->scaledToWidth(winWidth_, Qt::SmoothTransformation));
    } else {
      int width = int(pixmap->width()*zoomFactor_);
      int height = int(pixmap->height()*zoomFactor_);
      out = new PageContentType(pixmap->scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
  } else {
    //just copy the pixmap
    out = new PageContentType(*pixmap);
  }
#endif
  return out;
}

//send Pixmap request to okular core library
void OkularDocument::onPageRequest(int page, qreal factor)
{
  qDebug() << "OkularDocument::onPageRequest: pageNb" << page << ", scaleFactor" << factor;
#ifdef POPPLER_BACKEND
  if (NULL == doc_) {
    return;
  }
  zoomFactor_ = factor;
  const Poppler::Page *p = doc_->page(page);
  if (NULL != p) {
    int width = 0;
    int height = 0;
    preProcessPage(width, height, p);
    QSize pageSize = p->pageSize();
#ifdef POPPLER_BACKEND
    QImage tmpPageContent = p->renderToImage(width*72.0/pageSize.width(), height*72.0/pageSize.height(), -1, -1, -1, -1);
#else
    QPixmap tmpPageContent = QPixmap::fromImage(img);
#endif
    if (!tmpPageContent.isNull()) {
      const PageContentType *pageContent = postProcessPage(&tmpPageContent);
      emit pageReady(page, pageContent);
    } else {
      qDebug() << "page content is empty";
    }
  } else {
    qDebug() << "Cannot get page from doc";
  }
#else
  if((NULL == doc_) || (NULL == painter_)) {
    return;
  }
  zoomFactor_ = factor;
  const Okular::Page *p = doc_->page(page);
  if(NULL != p) {
    int width = 0;
    int height = 0;
    preProcessPage(width, height, p);
    painter_->sendRequest(p, width, height);
  }
#endif
}

#ifndef POPPLER_BACKEND
void OkularDocument::notifyPageChanged(int page, int flags)
{
  if(flags & DocumentObserver::Pixmap) {
    qDebug() << "DocumentObserver::Pixmap" << page;
    const QPixmap *pix = painter_->getPagePixmap(page);
    if (NULL != pix) {
      pix = postProcessPage(pix);//TODO: remove unneeded duplication ?
      //TODO: use QPixmap::scaledToWidth with Qt::SmoothTransformation
      //delete immediatelly internal pixmap
      Okular::Page *p = const_cast<Okular::Page*>(doc_->page(page));
      if (NULL != p) {
        p->deletePixmap(this);
      }
      emit pageReady(page, pix);
    }
  }
  else if(flags & DocumentObserver::Bookmark) {
    qDebug() << "DocumentObserver::Bookmark" << page;
  }
  else if(flags & DocumentObserver::Highlights) {
    qDebug() << "DocumentObserver::Highlights" << page;
  }
  else if(flags & DocumentObserver::TextSelection) {
    qDebug() << "DocumentObserver::TextSelection" << page;
  }
  else if(flags & DocumentObserver::Annotations) {
    qDebug() << "DocumentObserver::Annotations" << page;
  }
  else if(flags & DocumentObserver::BoundingBox) {
    qDebug() << "DocumentObserver::BoundingBox" << page;
  }
  else if(flags & DocumentObserver::NeedSaveAs) {
    qDebug() << "DocumentObserver::NeedSaveAs" << page;
  }
  else {
    qDebug() << "Unknown notification" << flags << " for page" << page;
  }
}
#endif

const QStringList& OkularDocument::supportedFilePatterns()
{
  if ((NULL != doc_) && supportedFilePatterns_.isEmpty()) {
#ifdef POPPLER_BACKEND
  supportedFilePatterns_ << "*.pdf";
#else
    //shall be called only once
    QStringList supportedMimeTypes = doc_->supportedMimeTypes();
    QStringList::ConstIterator it;
    for (it = supportedMimeTypes.begin(); it != supportedMimeTypes.end(); ++it) {
      KMimeType::Ptr type = KMimeType::mimeType(*it);
      if (false == type.isNull()) {
        supportedFilePatterns_ << type->patterns();
      }
    }
    supportedFilePatterns_.removeDuplicates();
    supportedFilePatterns_.sort();
#endif
  }
  return supportedFilePatterns_;
}
