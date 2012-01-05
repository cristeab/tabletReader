#include "chmreply.h"

CHMReply::CHMReply(QObject *parent, const QNetworkRequest &req,
                   const QNetworkAccessManager::Operation op,
                   chmFile* file) : QNetworkReply(parent)
{
    this->setRequest(req);
    this->setOperation(op);
    this->setUrl(req.url());
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");

    m_file = file;
    QString filename = req.url().toString(QUrl::RemoveScheme | QUrl::RemoveFragment).mid(2); //remove first two slash

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
    return bytesavail+QNetworkReply::bytesAvailable();
}

void CHMReply::abort()
{
    QNetworkReply::close();
}

qint64 CHMReply::readData(char* data, qint64 maxlen)
{
    int i = chm_retrieve_object(m_file, &cui, (unsigned char *)data, cui.length-bytesavail, maxlen);
    if(i>0)
    {
        bytesavail -= i;
        return i;
    }
    return -1;
}
