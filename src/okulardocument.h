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

#ifndef OKULAR_DOCUMENT_H
#define OKULAR_DOCUMENT_H

#include <QPixmap>
#ifdef POPPLER_BACKEND
namespace Poppler
{
  class Document;
  class Page;
}
#else
#include <kmimetype.h>
#include <core/document.h>
#include <core/observer.h>

namespace Okular
{
  class Document;
  class Page;
  class DocumentObserver;
}
class PagePainter;
#endif

#ifndef POPPLER_BACKEND
class OkularDocument : public QObject, public Okular::DocumentObserver
#else
class OkularDocument : public QObject
#endif
{
  Q_OBJECT

public:
#ifdef POPPLER_BACKEND
  typedef QImage PageContentType;
#else
  typedef QPixmap PageContentType;
#endif
  OkularDocument();
  bool load(const QString &fileName);
  uint numPages() const;
  void deletePage(const PageContentType *p) {
    delete p;
  }
  void setWinWidth(int width) {
    winWidth_ = width;
  }
  ~OkularDocument();
#ifndef POPPLER_BACKEND
  void notifyPageChanged(int page, int flags);
#endif
  const QStringList& supportedFilePatterns();
signals:
  void pageReady(int page, const OkularDocument::PageContentType *pageContent);

public slots:
  void onPageRequest(int page, qreal factor);

private:
  const PageContentType* postProcessPage(const PageContentType *pageContent);
  int winWidth_;
  qreal zoomFactor_;
  QStringList supportedFilePatterns_;
#ifdef POPPLER_BACKEND
  Poppler::Document *doc_;
  void preProcessPage(int &width, int &height, const Poppler::Page *page);
#else
  void preProcessPage(int &width, int &height, const Okular::Page *page);
  Okular::Document *doc_;
  PagePainter *painter_;
  KMimeType::Ptr mimeType_;
#endif
};

#endif // OKULAR_DOCUMENT_H
