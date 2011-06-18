#ifndef LOADPAGES_H
#define LOADPAGES_H

#include <QThread>
class DocumentWidget;

class LoadPages : public QThread
{
    Q_OBJECT

public:
    LoadPages(DocumentWidget* doc = NULL);
    void run();

public slots:
    void renderPage(int page);

private:
    DocumentWidget *doc_;
};

#endif // LOADPAGES_H
