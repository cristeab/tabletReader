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

#include "kdjvu.h"
#include "djvudocument.h"

KDjVu *DJVUDocument::doc_= new KDjVu();
int DJVUDocument::numPages_ = 0;
DJVUDocument *DJVUDocument::instance_ = NULL;

DJVUDocument::~DJVUDocument()
{
    delete doc_;
    numPages_ = 0;
    instance_ = NULL;
}

Document *DJVUDocument::load(const QString &fileName)
{
    if (NULL == instance_)
    {
        instance_ = new DJVUDocument();
    }

    if ((NULL != doc_) && (true == doc_->openFile(fileName)))
    {
        numPages_ = doc_->pages().size();
        return instance_;
    }
    return NULL;
}

QImage DJVUDocument::renderToImage(int page, qreal xres, qreal yres)
{
    if (NULL == doc_)
    {
        return QImage();
    }
    KDjVu::Page* curpage = doc_->pages().at(page);
    if (NULL == curpage)
    {
        return QImage();
    }
    int width = qRound(curpage->width()*xres/curpage->dpi());
    int height = qRound(curpage->height()*yres/curpage->dpi());
    return doc_->image(page, width, height, 0);
}
