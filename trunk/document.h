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

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QString>
#include <QByteArray>
#include <QImage>

//interface for handling electronic documents (PDF, DJVU, CHM, etc)
class Document
{
public:
    enum {ID_PDF = 0, ID_DJVU, ID_CHM};//available concrete document IDs
    virtual int id() = 0;
    virtual int load(const QString &fileName) = 0;
    virtual int loadFromData(const QByteArray&)
    {
        return EXIT_FAILURE;
    }
    //this method needs to be thread safe
    virtual QImage renderToImage(int page, qreal xres, qreal yres) = 0;
    int numPages() const
    {
        return numPages_;
    }
    virtual ~Document()
    {
        numPages_ = 0;
    }
protected:
    Document() : //ctor is protected, this class is a singleton
        numPages_(0)
    {
    }
    int numPages_;
private:
    Document(const Document&);//copy ctor not allowed
    Document& operator=(const Document&);//copy assignment operator not allowed    
};

#endif // DOCUMENT_H
