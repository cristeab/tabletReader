/****************************************************************************
**
** Copyright (C) 2011, Bogdan Cristea. All rights reserved.
**
** This program is free software; you can redistribute it and/or modify it under
** the terms of the GNU General Public License as published by the Free Software
** Foundation; either version 2, or (at your option)  any later version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along with
** this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
** Street - Fifth Floor, Boston, MA 02110-1301, USA.
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

//Header for Intel AppUp(TM) software
#include "adpcppf.h"

class QScrollArea;
class QSpinBox;
class QComboBox;
class SlidingStackedWidget;
class FileBrowserModel;
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
    void closeGotoPage(const QString &pageNb = "");
    void showZoomPage();
    void closeZoomPage(int index = -1);
    void showCommandPopupMenu();
    void closeCommandPopupMenu(const QString &cmd = "");
    void openFile(const QString &filePath);    
    void fullScreen();
    void normalScreen();
    void increaseScale();
    void decreaseScale();
    void setAnimationFlag();
    void togglePageDisplay();
    void onSendCommand(const QString &cmd);
    void showHelp(bool slideNext = true);
    void showAboutDialog();
    void closeAboutDialog();
    void showWarningMessage(const QString &title, const QString &explanation = "");

private:
    enum {TOOLTIP_VISIBLE_TIME_MS = 1500,
         HORIZONTAL_SLIDE_SPEED_MS = 500,
         SWIPE_THRESHOLD = 5,
         LONG_PRESS_TIMEOUT_MS = 1000,
         FULL_SCREEN_WIDTH = 1024,
         FULL_SCREEN_HEIGHT = 768,
         MIN_SCREEN_WIDTH = 800,
         MIN_SCREEN_HEIGHT = 600};

    bool eventFilter(QObject *, QEvent *);
    bool showNextPage();
    bool showPrevPage();
    void showPageNumber(int currentPage, int nbPages);
    void setupDocDisplay(unsigned int pageNumber, const QString &filePath);
    void gotoPage(int pageNb, int numPages);
    void setZoomFactor(int index);

    SlidingStackedWidget *slidingStacked_;
    DocumentWidget *document_;
    QDeclarativeView *toolBar_;
    QString lastFilePath_;
    QVector<qreal> scaleFactors_;
    int currentZoomIndex_;
    QPoint startPoint_;
    QPoint endPoint_;
    QMenu *pagePopupMenu_;    
    bool animationFinished_;
    QDeclarativeView *fileBrowser_;
    QDeclarativeView *gotoPage_;
    QDeclarativeView *zoomPage_;
    QDeclarativeView *commandPopupMenu_;
    QDeclarativeView *aboutDialog_;
    QElapsedTimer pressTimer_;
    Worker *worker_;
    bool showPageNumber_;
    Flickable *flickable_;
    FileBrowserModel* fileBrowserModel_;
	Application *appupApp_;
};

#endif
