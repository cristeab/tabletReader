#include "loadpages.h"
#include "documentwidget.h"
#include <QDebug>

LoadPages::LoadPages(DocumentWidget* doc) :
    doc_(doc)
{    
    start(QThread::IdlePriority);
}

void LoadPages::run()
{
    exec();
}

void LoadPages::loadPages(int currentPage)
{
    static int prevPage = 0;    
    prevPage = currentPage;
    qDebug() << "LoadPages::loadPages(" << currentPage << ")";
}
