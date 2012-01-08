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
** Reference: qindle project (http://code.google.com/p/qindle/)
**
****************************************************************************/

#ifndef CHMREPLY_H
#define CHMREPLY_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "chm_lib.h"

struct chmFile;

//reply class returned by createRequest method
class CHMReply : public QNetworkReply
{
    Q_OBJECT
public:
    CHMReply(QObject *parent, const QNetworkRequest &req,
             const QNetworkAccessManager::Operation &op, chmFile* file);

    virtual void abort();
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 bytesAvailable () const;
private:
    qint64 bytesavail;
    struct chmFile* m_file;
    struct chmUnitInfo cui;
};

#endif // CHMREPLY_H
