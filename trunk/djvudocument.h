#ifndef DJVUDOCUMENT_H
#define DJVUDOCUMENT_H

#include "document.h"

class KDjVu;

class DJVUDocument : public Document
{
public:
    virtual int id()
    {
        return ID_DJVU;
    }
    static Document *load(const QString &fileName);
    virtual QImage renderToImage(int page, qreal xres, qreal yres);
    virtual int numPages() const
    {
        return numPages_;
    }
    virtual ~DJVUDocument();
private:
    static KDjVu *doc_;
    static int numPages_;
    static DJVUDocument *instance_;
};

#endif // DJVUDOCUMENT_H
