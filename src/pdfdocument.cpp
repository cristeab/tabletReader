/****************************************************************************
**
** Copyright (C) 2012, Bogdan Cristea. All rights reserved.
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

PDFDocument::~PDFDocument()
{
    delete doc_;
}

int PDFDocument::load(const QString &fileName)
{
    delete doc_;
    if (NULL != (doc_ = Poppler::Document::load(fileName)))
    {
        doc_->setRenderHint(Poppler::Document::Antialiasing);
        doc_->setRenderHint(Poppler::Document::TextAntialiasing);
        numPages_ = doc_->numPages();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int PDFDocument::loadFromData(const QByteArray &data)
{
    delete doc_;
    if (NULL != (doc_ = Poppler::Document::loadFromData(data)))
    {
        doc_->setRenderHint(Poppler::Document::Antialiasing);
        doc_->setRenderHint(Poppler::Document::TextAntialiasing);
        numPages_ = doc_->numPages();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

QImage PDFDocument::renderToImage(int page, qreal xres, qreal yres)
{
    return (NULL != doc_)?doc_->page(page)->renderToImage(xres, yres):QImage();
}
