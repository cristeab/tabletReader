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
