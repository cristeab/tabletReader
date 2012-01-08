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

#ifndef PDFDOCUMENT_H
#define PDFDOCUMENT_H

#include "document.h"

namespace Poppler {
class Document;
}

class PDFDocument : public Document
{
public:
    virtual int id()
    {
        return ID_PDF;
    }
    static Document* instance()
    {
        if (NULL == instance_)
        {
            instance_ = new PDFDocument();
        }
        return instance_;
    }
    virtual int load(const QString &fileName);
    virtual int loadFromData(const QByteArray &data);
    virtual QImage renderToImage(int page, qreal xres, qreal yres);
    virtual ~PDFDocument();
private:
    PDFDocument() :
        Document(), doc_(NULL)
    {}
    Poppler::Document *doc_;
    static PDFDocument *instance_;
};

#endif // PDFDOCUMENT_H
