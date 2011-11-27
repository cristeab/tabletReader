#include <QDir>
#include <QDateTime>
#include <stdlib.h>
#include <QDebug>
#include "logger.h"

QTextStream Logger::ts_;
QFile *Logger::pOutFile_ = NULL;

Logger::Logger(const QString &fileLogName)
{
    if (NULL != pOutFile_)
    {
        delete pOutFile_;
    }
    pOutFile_ = new QFile(QDir::homePath()+QDir::separator()+
                          QDir::toNativeSeparators(fileLogName));
    pOutFile_->open(QIODevice::WriteOnly | QIODevice::Append);
    ts_.setDevice(pOutFile_);
    qInstallMsgHandler(debugMessageHandler);
    qDebug() << "\n\nSTART" << QDateTime::currentDateTime().toString(Qt::ISODate)
             << "START\n";
}

Logger::~Logger()
{
    delete pOutFile_;
    pOutFile_ = NULL;
}

void Logger::debugMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }
    ts_ << QDateTime::currentDateTime().toMSecsSinceEpoch()
       << " " << txt << endl;
}
