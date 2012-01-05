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

#include <QByteArray>
#include <QStandardItemModel>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebFrame>
#include <QTextCodec>
#include <QDebug>
#include "chm_lib.h"
#include "chmdocument.h"
#include "chmreply.h"

CHMDocument *CHMDocument::instance_ = NULL;

CHMDocument::CHMDocument() :
    Document(),
    doc_(NULL),
    numPages_(0),
    TOCResolved_(false),
    TOCName_(QString()),
    TOCModel_(new QStandardItemModel),
    codecName_(""),
    webView_(new QWebView)
{
    //init TOC model (this could be done in a base class)
    TOCModel_->setColumnCount(3);
    TOCModel_->setHeaderData(0, Qt::Horizontal, tr("Name"));
    TOCModel_->setHeaderData(1, Qt::Horizontal, tr("URL"));
    TOCModel_->setHeaderData(2, Qt::Horizontal, tr("Page"));
    //init the internal web browser
    webView_->page()->setNetworkAccessManager(this);
    connect(webView_, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
}

CHMDocument::~CHMDocument()
{
    instance_ = NULL;
    if (NULL !=  doc_)
    {
        chm_close(doc_);
        doc_ = NULL;
    }
    numPages_ = 0;    
    delete TOCModel_;
    delete webView_;
}

Document *CHMDocument::load(const QString &fileName)
{
    if (NULL == instance_)
    {
        instance_ = new CHMDocument();
    }

    if (NULL != instance_->doc_)
    {
        chm_close(instance_->doc_);
    }
    if (NULL != (instance_->doc_ = chm_open(fileName.toLocal8Bit().data())))
    {
        instance_->TOCResolved_ = false;
        instance_->codecName_ = "utf8";
        instance_->Spine_.clear();
        if ((EXIT_SUCCESS == instance_->init()) && (EXIT_SUCCESS == instance_->getTOC()))
        {
            instance_->numPages_ = instance_->Spine_.count();
            return instance_;
        }
    }
    return NULL;
}

QImage CHMDocument::renderToImage(int page, qreal xres, qreal)
{
    if ((NULL == doc_) || (0 > page) || (Spine_.count() <= page))
    {
        return QImage();
    }
    webView_->load(QUrl::fromLocalFile("/"+Spine_.at(page)));
    if (EXIT_FAILURE == eventloop_.exec()) //wait for load to complete
    {
        return QImage();//an error occured, nothing will be displayed
    }
    webView_->setZoomFactor(xres/webView_->physicalDpiX());
    QWebFrame *webFrame = webView_->page()->mainFrame();
    webFrame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    webFrame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    webView_->setGeometry(QRect(QPoint(0, 0), webFrame->contentsSize()));
    return QPixmap::grabWidget(webView_).toImage();
}

int CHMDocument::init()
{
    if (NULL == doc_)
    {
        return EXIT_FAILURE;
    }
    chmUnitInfo cui;
    int i = chm_resolve_object(doc_, "/#SYSTEM", &cui);

    if (i == CHM_RESOLVE_FAILURE)
    {
        return EXIT_FAILURE;
    }
    char data[DATA_SIZE] = "";
    QByteArray filename;
    char* code;
    char* length;
    /*chm_retrieve_object(doc_, &cui, (unsigned char *)data, 0, DATA_SIZE);
    //the following decodes topic file name from #system file.
    while (i < DATA_SIZE)
    {
        code = data+i;
        length = data+i+2;
        if (*code == 0x02)
        {
            filename.append(data+i+4, (int)*length);
            break;
        } else
        {
            i = i+4+(int)*length;
        }
    }
    TopicName = QString(filename);
    filename.clear();*/

    //the following code doesn't work in all conditions.
    i = chm_resolve_object(doc_, "/#STRINGS", &cui);

    if(i == CHM_RESOLVE_FAILURE)
    {
        return EXIT_FAILURE;
    }
    chm_retrieve_object(doc_, &cui, (unsigned char *)data, 0, DATA_SIZE);
    //the following decodes toc file name from #strings file.
    length = data;
    while (i < 3)
    {
        code = length;
        length = strchr(code+1, 0);
        i++;
    };
    filename.append(code+1, length-code);
    //workaround
    if (filename.contains("hhc"))
    {
        TOCName_ = "/"+QString(filename);
    } else
    {
        i = chm_resolve_object(doc_, "/toc.hhc", &cui);
        if (i == CHM_RESOLVE_SUCCESS)
        {
            TOCName_ = "/toc.hhc";
        } else
        {
            TOCName_ = "";
        }
    }

    return EXIT_SUCCESS;
}

int CHMDocument::getTOC()
{
    if(true == TOCResolved_)
    {
        return EXIT_SUCCESS;
    }
    chmUnitInfo cui;
    int i = chm_resolve_object(doc_, TOCName_.toAscii().constData(), &cui);
    if (i == CHM_RESOLVE_FAILURE)
    {
        return EXIT_FAILURE;
    }
    QByteArray qdata(cui.length,0);
    chm_retrieve_object(doc_, &cui, (unsigned char *)qdata.data(), 0, cui.length);

    //the root item
    QStandardItem* currentItem = new QStandardItem(tr("Index"));
    QStandardItem *nameItem = NULL;
    QStandardItem *urlItem = NULL;
    TOCModel_->appendRow(currentItem);
    //xmlreader report that the hhc is "not well formed", so we use a hand-made parser.

    //the following code is based on kchmviewer, and with some mods.
    /* Copyright (C) 2004-2007 by Georgy Yunaev, gyunaev@ulduzsoft.com       *
     * Portions Copyright (C) 2003  Razvan Cojocaru <razvanco@gmx.net>       */
    /*
     * FIXME: <OBJECT type="text/sitemap"><param name="Merge" value="hhaxref.chm::/HHOCX_c.hhc"></OBJECT>
     *  (from htmlhelp.chm)
    */

    //some SOB use gbk encoding, while others don't like it....
    QString src = QTextCodec::codecForName(codecName_)->toUnicode(qdata);

    int pos = 0;
    bool in_object = false;

    // Split the HHC file by HTML tags
    int stringlen = src.length();
    int out = EXIT_FAILURE;
    while ( (pos < stringlen) && ((pos = src.indexOf ('<', pos)) != -1) )
    {
        int i, j, word_end = 0;

        for ( i = ++pos; i < stringlen; i++ )
        {
            // If a " or ' is found, skip to the next one.
            if ( (src[i] == '"') || (src[i] == '\'') )
            {
                // find where quote ends, either by another quote, or by '>' symbol (some people don't know HTML)
                int nextpos = src.indexOf (src[i], i+1);
                if ( (nextpos == -1) && ((nextpos = src.indexOf ('>', i+1)) == -1) )
                {
                    qDebug() << "CHMDocument::getTOC: corrupted TOC: " << qPrintable(src.mid(i));
                    goto exit_err;
                }
                i = nextpos;
            }
            else if ( src[i] == '>' )
            {
                break;
            }
            else if ( !src[i].isLetterOrNumber() && (src[i] != '/') && !word_end )
            {
                word_end = i;
            }
        }

        QString tagword;
        QString tag = src.mid(pos, i - pos);
        if ( word_end )
        {
            tagword = src.mid (pos, word_end - pos).toLower();
        } else
        {
            tagword = tag.toLower();
        }

        // <OBJECT type="text/sitemap"> - a topic entry
        if ( tagword == "object" && tag.indexOf ("text/sitemap", 0, Qt::CaseInsensitive ) != -1 )
        {
            in_object = true;
            nameItem = new QStandardItem();
            urlItem = new QStandardItem();
        }
        else if ( (tagword == "/object") && in_object )
        {
            // a topic entry closed. Add a tree item
            if ( !nameItem->text().isEmpty() )
            {
                j = currentItem->rowCount();
                currentItem->setChild(j, 0, nameItem);
                currentItem->setChild(j, 1, urlItem);
                Spine_.append(urlItem->text());
            }
            else if ( !urlItem->text().isEmpty() )
            {
                qDebug() << "CHMDocument::getTOC: <object> tag with url is parsed, but name is empty.";
            }
            else
            {
                qDebug() << "CHMDocument::getTOC: <object> tag is parsed, but both name and url are empty.";
            }
            in_object = false;
        }
        else if ( (tagword == "param") && in_object )
        {
            // <param name="Name" value="First Page">
            int offset; // strlen("param ")
            QString name_pattern = "name=";
            QString value_pattern = "value=";
            QString pname, pvalue;

            if ( (offset = tag.indexOf (name_pattern, 0, Qt::CaseInsensitive )) == -1 )
            {
                qDebug() << "CHMDocument::getTOC: bad <param> tag '" << qPrintable(tag) << "': no name=";
                goto exit_err;
            }

            // offset+5 skips 'name='
            offset = findStringInQuotes (tag, offset + name_pattern.length(), pname, TRUE);
            pname = pname.toLower();

            if ( (offset = tag.indexOf(value_pattern, offset, Qt::CaseInsensitive )) == -1 )
            {
                qDebug() << "CHMDocument::getTOC: bad <param> tag '" << qPrintable(tag) << "': no value=";
                goto exit_err;
            }

            // offset+6 skips 'value='
            if (0 == findStringInQuotes(tag, offset + value_pattern.length(), pvalue, FALSE))
            {
                goto exit_err;
            }

            if (pname == "name")
            {
                // Some help files contain duplicate names, where the second name is empty. Work it around by keeping the first one
                if (!pvalue.isEmpty())
                {
                    nameItem->setText(pvalue);
                }
            } else if (pname == "local")
            {
                urlItem->setText(pvalue);
            }
        }
        else if (tagword == "ul") // increase indent level
        {
            if(nameItem)
            {
                currentItem = nameItem;
            }
        }
        else if (tagword == "/ul") // decrease indent level
        {
            currentItem = currentItem->parent();
        }

        pos = i;
    }
    TOCResolved_ = true;
    out = EXIT_SUCCESS;
exit_err:
    return out;
}

//this method is also stealed from kchmviewer.
int CHMDocument::findStringInQuotes (const QString& tag, int offset,
                                     QString& value, bool firstquote)
{
        int qbegin = tag.indexOf ('"', offset);
        if (qbegin == -1)
        {
            qDebug() << "CHMDocument::findStringInQuotes: cannot find first quote "
                      "in <param> tag:" << qPrintable( tag );
            return 0;//fatal error
        }
        int qend = firstquote ? tag.indexOf ('"', qbegin + 1) : tag.lastIndexOf ('"');
        if ((qend == -1) || (qend <= qbegin))
        {
            qDebug() << "CHMDocument::findStringInQuotes: cannot find last quote in "
                      "<param> tag:" << qPrintable(tag);
            return 0;//fatal error
        }
        // If we do not need to decode HTML entities, just return.
        value = tag.mid (qbegin + 1, qend - qbegin - 1);
        return qend + 1;
}

//this method is used by QWebView to load an HTML page
QNetworkReply* CHMDocument::createRequest(Operation op, const QNetworkRequest &req,
                             QIODevice *outgoingData)
{
    return (req.url().scheme()=="file")?(new CHMReply(this, req, op, doc_)):
                                         QNetworkAccessManager::createRequest(op, req, outgoingData);
}

//used to signal when the page has been loaded
void CHMDocument::onLoadFinished(bool ok)
{
    int out = EXIT_SUCCESS;
    if (false == ok)
    {
        qDebug() << "error on load";
        out = EXIT_FAILURE;
    }
    eventloop_.exit(out);
}
