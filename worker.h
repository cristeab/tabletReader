#ifndef WORKER_H
#define WORKER_H

#include <QThread>

class QProgressDialog;
class DocumentWidget;

class Worker : public QThread
{
    Q_OBJECT
public:
    Worker(QWidget *parent = NULL, DocumentWidget *doc = NULL);

public slots:
    void updateCache(int page);

protected:
    void run();

private:
    DocumentWidget *doc_;
};

#endif // WORKER_H
