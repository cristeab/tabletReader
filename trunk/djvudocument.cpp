#include "kdjvu.h"
#include "djvudocument.h"

KDjVu *DJVUDocument::doc_= new KDjVu();

DJVUDocument::~DJVUDocument()
{
    delete doc_;
    numPages_ = 0;
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
    int width = int(xres);
    int height = int(yres);
    if (NULL != doc_)
    {
        return doc_->image(page, width, height, 0);
    } else
    {
        return QImage();
    }
}
