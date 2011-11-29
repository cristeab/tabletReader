/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QTimer>
#include <QElapsedTimer>
#include "documentwidget.h"
#include "worker.h"

class QScrollArea;
class QSpinBox;
class QComboBox;
class SlidingStackedWidget;
class FileBrowser;
class QDeclarativeView;
class Flickable;

class Window : public QMainWindow
{
    Q_OBJECT    

public:
    Window(QWidget *parent = NULL);
    ~Window();

protected:
    void closeEvent(QCloseEvent *);

signals:
    void updateCache(int);

private slots:
    void showFileBrowser();
    void closeFileBrowser(const QString &doc);
    void showGotoPage();
    void closeGotoPage(const QString &pageNb);
    void showCommandPopupMenu();
    void closeCommandPopupMenu(const QString &cmd);
    void openFile(const QString &filePath);    
    void fullScreen();
    void normalScreen();
    void scaleDocument(int index);
    void increaseScale();
    void decreaseScale();
    void setAnimationFlag();
    void togglePageDisplay();

private:
    enum {TOOLTIP_VISIBLE_TIME_MS = 1000,
         HORIZONTAL_SLIDE_SPEED_MS = 500,
         SWIPE_THRESHOLD = 5,
         LONG_PRESS_TIMEOUT_MS = 1000};

    bool eventFilter(QObject *, QEvent *);
    bool showNextPage();
    bool showPrevPage();
    void showPageNumber(int currentPage, int nbPages);
    void setupDocDisplay(unsigned int pageNumber, const QString &filePath);
    void gotoPage(int pageNb, int numPages);

    SlidingStackedWidget *slidingStacked_;
    DocumentWidget *document_;
    QToolBar *toolBar_;
    QString lastFilePath_;
    QVector<qreal> scaleFactors_;
    QSpinBox *pageSpinBox_;
    QLabel *labelNbPages_;
    QComboBox *scaleComboBox_;
    QPoint startPoint_;
    QPoint endPoint_;
    QMenu *pagePopupMenu_;    
    bool animationFinished_;
    QDeclarativeView *fileBrowser_;
    QDeclarativeView *gotoPage_;
    QDeclarativeView *commandPopupMenu_;
    QElapsedTimer pressTimer_;
    Worker *worker_;
    bool showPageNumber_;
    Flickable *flickable_;
};

#endif
