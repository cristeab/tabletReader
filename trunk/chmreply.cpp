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

#include <QDebug>
#include "chmreply.h"

CHMReply::CHMReply(QObject *parent, const QNetworkRequest &req,
                   const QNetworkAccessManager::Operation &op,
                   chmFile* file) : QNetworkReply(parent)
{
    qDebug() << "CHMReply::CHMReply";
    this->setRequest(req);
    this->setOperation(op);
    this->setUrl(req.url());
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");

    m_file = file;
    QString filename = req.url().toString(QUrl::RemoveScheme | QUrl::RemoveFragment).mid(2); //remove first two slash
    qDebug() << "url: " << filename;

    int i = chm_resolve_object(m_file,filename.toUtf8().data(),&cui);
    if (i == CHM_RESOLVE_SUCCESS)
    {
        bytesavail = cui.length;
        this->setHeader(QNetworkRequest::ContentLengthHeader, cui.length);
        this->setHeader(QNetworkRequest::ContentTypeHeader, "text/html");
        this->open(QIODevice::ReadOnly);
        QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
    } else
    {
        this->setError(QNetworkReply::ContentNotFoundError, tr("404 not found"));
        QMetaObject::invokeMethod(this, "error", Qt::QueuedConnection,
                                  Q_ARG(QNetworkReply::NetworkError, QNetworkReply::ContentNotFoundError));
        QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
    }
}

qint64 CHMReply::bytesAvailable() const
{
    qDebug() << "CHMReply::bytesAvailable" << bytesavail;
    return bytesavail+QNetworkReply::bytesAvailable();
}

void CHMReply::abort()
{
    qDebug() << "CHMReply::abort";
    QNetworkReply::close();
}

qint64 CHMReply::readData(char* data, qint64 maxlen)
{
    qDebug() << "CHMReply::readData";
    int i = chm_retrieve_object(m_file, &cui, (unsigned char *)data, cui.length-bytesavail, maxlen);
    if(i>0)
    {
        bytesavail -= i;
        return i;
    }
    return -1;
}
