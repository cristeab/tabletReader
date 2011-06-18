#include "loadpages.h"
#include "documentwidget.h"
#include <QDebug>

LoadPages::LoadPages(DocumentWidget* doc) :
    doc_(doc)
{    
    start(QThread::IdlePriority);
    QObject::moveToThread(this);
}

void LoadPages::run()
{
    exec();
}

void LoadPages::renderPage(int page)
{
    static int prevPage = 0;
    qDebug() << "current page " << page;
    qDebug() << "previous page" << prevPage;
    prevPage = page;
}
