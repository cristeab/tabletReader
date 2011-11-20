#ifndef FLICKABLE_H
#define FLICKABLE_H

#include <QObject>

class FlickablePrivate;
class QWidget;

class Flickable: public QObject
{
    Q_OBJECT
public:
    Flickable(QObject *parent = 0);
    ~Flickable();
    void activateOn(QWidget *widget);
    void deactivateFrom(QWidget *widget);
    bool eventFilter(QObject *object, QEvent *event);

protected:
    void timerEvent(QTimerEvent *event);

private:
    FlickablePrivate *d;
};

#endif // FLICKABLE_H
