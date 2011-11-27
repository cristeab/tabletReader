#ifndef LOGGER_H
#define LOGGER_H

#include <QTextStream>

class QFile;

class Logger
{
public:
    explicit Logger(const QString &fileLogName);
    ~Logger();
private:
    static void debugMessageHandler(QtMsgType type, const char *msg);
    static QTextStream ts_;
    static QFile *pOutFile_;
};

#endif // LOGGER_H
