#include "kdjvu.h"
#include "djvudocument.h"

KDjVu *DJVUDocument::doc_= new KDjVu();
int DJVUDocument::numPages_ = 0;
DJVUDocument *DJVUDocument::instance_ = NULL;

DJVUDocument::~DJVUDocument()
{
    delete doc_;
    numPages_ = 0;
    instance_ = NULL;
}

Document *DJVUDocument::load(const QString &fileName)
{
    if (NULL == instance_)
    {
        instance_ = new DJVUDocument();
    }

    if ((NULL != doc_) && (true == doc_->openFile(fileName)))
    {
        numPages_ = doc_->pages().size();
        return instance_;
    }
    return NULL;
}

QImage DJVUDocument::renderToImage(int page, qreal xres, qreal yres)
{
    //TODO: some conversion of xres, yres to width, heigth is needed
    if (NULL == doc_)
    {
        return QImage();
    }
    KDjVu::Page* curpage = doc_->pages().at(page);
    if (NULL == curpage)
    {
        return QImage();
    }
    int width = qRound(curpage->width()*xres/curpage->dpi());
    int height = qRound(curpage->height()*yres/curpage->dpi());
    return doc_->image(page, width, height, 0);
}
