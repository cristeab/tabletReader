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

#include <poppler-qt4.h>
#include "pdfdocument.h"

Poppler::Document *PDFDocument::doc_= NULL;
int PDFDocument::numPages_ = 0;
PDFDocument *PDFDocument::instance_ = NULL;

PDFDocument::~PDFDocument()
{
    delete doc_;
    numPages_ = 0;
    instance_ = NULL;
}

Document *PDFDocument::load(const QString &fileName)
{
    if (NULL == instance_)
    {
        instance_ = new PDFDocument();
    }

    delete doc_;
    doc_ = Poppler::Document::load(fileName);
    if (NULL != doc_)
    {
        doc_->setRenderHint(Poppler::Document::Antialiasing);
        doc_->setRenderHint(Poppler::Document::TextAntialiasing);
        numPages_ = doc_->numPages();
        return instance_;
    }
    return NULL;
}

Document *PDFDocument::loadFromData(const QByteArray &data)
{
    if (NULL == instance_)
    {
        instance_ = new PDFDocument();
    }

    delete doc_;
    doc_ = Poppler::Document::loadFromData(data);
    if (NULL != doc_)
    {
        doc_->setRenderHint(Poppler::Document::Antialiasing);
        doc_->setRenderHint(Poppler::Document::TextAntialiasing);
        numPages_ = doc_->numPages();
        return instance_;
    }
    return NULL;
}

QImage PDFDocument::renderToImage(int page, qreal xres, qreal yres)
{
    return (NULL != doc_)?doc_->page(page)->renderToImage(xres, yres):QImage();
}
