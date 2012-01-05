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

#ifndef CHMDOCUMENT_H
#define CHMDOCUMENT_H

#include <QtNetwork/QNetworkAccessManager>
#include <QEventLoop>
#include "document.h"

struct chmFile;
class QStandardItemModel;
class QWebView;

class CHMDocument : public Document, public QNetworkAccessManager
{
public:
    virtual int id()
    {
        return ID_CHM;
    }
    static Document *load(const QString &fileName);
    virtual QImage renderToImage(int page, qreal xres, qreal yres);
    virtual int numPages() const
    {
        return numPages_;
    }
    virtual ~CHMDocument();
private slots:
    void onLoadFinished(bool ok);
private:
    CHMDocument();
    int init();
    int getTOC();
    int findStringInQuotes (const QString& tag, int offset,
                                         QString& value, bool firstquote);
    QNetworkReply* createRequest(Operation op, const QNetworkRequest & req,
                                 QIODevice * outgoingData = 0);
    static CHMDocument *instance_;
    chmFile *doc_;
    int numPages_;
    bool TOCResolved_;
    QString TOCName_;
    QStandardItemModel *TOCModel_;
    QByteArray codecName_;
    QStringList Spine_;
    QWebView *webView_;
    QEventLoop eventloop_;
    enum {DATA_SIZE = 256};
};

#endif // CHMDOCUMENT_H
