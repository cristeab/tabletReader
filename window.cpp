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

#include <QtGui>
#include <QScrollArea>
#include <QtDeclarative>
#include "window.h"
#include "SlidingStackedWidget.h"
#include "filebrowsermodel.h"
#include "worker.h"
#include "flickable.h"

#define ORGANIZATION "cristeab"
#define APPLICATION "tabletReader"
#define KEY_PAGE "current_page"
#define KEY_FILE_PATH "current_file_path"
#define KEY_ZOOM_LEVEL "current_zoom_level"
#define HELP_FILE ":/help/help/tabletReader.pdf"

Window::Window(QWidget *parent)
    : QMainWindow(parent),
      fileBrowser_(NULL),
      gotoPage_(NULL),
      zoomPage_(NULL),
      commandPopupMenu_(NULL),
      aboutDialog_(NULL),
      showPageNumber_(false),      
      flickable_(NULL),
      fileBrowserModel_(new FileBrowserModel(this))
{
    //main window    
    QWidget *centralWidget = new QWidget(this);
    QGridLayout *gridLayout = new QGridLayout(centralWidget);
    setCentralWidget(centralWidget);
    setWindowTitle(tr(APPLICATION));
    setStyleSheet("background-color: black");

    //main toolbar
    toolBar_ = new QDeclarativeView(this);
    toolBar_->setSource(QUrl("qrc:/qml/qml/maintoolbar.qml"));
    QObject *pDisp = toolBar_->rootObject();
    if (NULL != pDisp)
    {
        pDisp->setProperty("width", width());
        QObject *pToolbar = pDisp->findChild<QObject*>("toolbar");
        if (NULL != pToolbar)
        {
            connect(pToolbar, SIGNAL(sendCommand(QString)), this, SLOT(onSendCommand(QString)));
        } else
        {
            qDebug() << "cannot find toolbar object";
        }
    }
    gridLayout->addWidget(toolBar_, 0, 0, 1, 1);

    //actions for zoom in/out
    QAction *increaseScaleAction = new QAction(this);
    increaseScaleAction->setShortcut(tr("Ctrl++"));
    QAction *decreaseScaleAction = new QAction(this);
    decreaseScaleAction->setShortcut(tr("Ctrl+-"));
    addAction(increaseScaleAction);
    addAction(decreaseScaleAction);

    //zoom scale factors
    scaleFactors_ << 0.25 << 0.5 << 0.75 << 1.
                  << 1.25 << 1.5 << 2. << 3. << 4.;
    currentZoomIndex_ = 3;//zoom 100%

    //create main document
    document_ = new DocumentWidget(this);

    //worker thread
    worker_ = new Worker(centralWidget, document_);

    //create sliding animation
    slidingStacked_ = new SlidingStackedWidget(this);

    //create flickable object
    flickable_ = new Flickable(this);

    //init document pages and the sliding animation
    QScrollArea *scroll = NULL;
    QLabel *label = NULL;
    register int n = 0;
    for (n = 0; n < DocumentWidget::CACHE_SIZE; ++n)
    {
        //scroll areas (one for each page)
        scroll = new QScrollArea(centralWidget);
        scroll->setWidgetResizable(true);
        scroll->setAlignment(Qt::AlignCenter);
        label = new QLabel();//QLabel is used to display a page
        label->setAlignment(Qt::AlignCenter);
        scroll->setWidget(label);
        scroll->installEventFilter(this);
        slidingStacked_->addWidget(scroll);//scroll areas are switched by the stacked widget
        flickable_->activateOn(scroll);
    }
    document_->setStackedWidget(slidingStacked_);
    document_->setPhysicalDpi(label->physicalDpiX(), label->physicalDpiY());
    slidingStacked_->setSpeed(HORIZONTAL_SLIDE_SPEED_MS);
    slidingStacked_->setWrap(true);
    slidingStacked_->setVerticalMode(false);
    slidingStacked_->setStyleSheet("background:black");
    slidingStacked_->setAttribute(Qt::WA_DeleteOnClose);
    gridLayout->addWidget(slidingStacked_, 1, 0, 1, 1);

    connect(slidingStacked_, SIGNAL(animationFinished()),
            this, SLOT(setAnimationFlag()));
    connect(increaseScaleAction, SIGNAL(triggered()), this, SLOT(increaseScale()));
    connect(decreaseScaleAction, SIGNAL(triggered()), this, SLOT(decreaseScale()));
    connect(this, SIGNAL(updateCache(int)), worker_, SLOT(updateCache(int)));

    statusBar()->hide();

    //page number popup menu
    pagePopupMenu_ = new QMenu(this);
    pagePopupMenu_->setBackgroundRole(QPalette::ToolTipText);
    pagePopupMenu_->addAction("");
    pagePopupMenu_->setEnabled(false);

    //start worker thread
    worker_->start();

    //set document if one has been previously open
    QSettings settings(ORGANIZATION, APPLICATION);
    QString filePath;
    if (NULL != (filePath = settings.value(KEY_FILE_PATH).toString()))
    {
        if (document_->setDocument(filePath))
        {
            currentZoomIndex_ = settings.value(KEY_ZOOM_LEVEL, 3).toInt();
            setupDocDisplay(settings.value(KEY_PAGE, 0).toInt()+1, filePath);
        }
    } else
    {
        qDebug() << "no document found";
        showHelp(false);
    }
    animationFinished_ = true;

    normalScreen();
}

Window::~Window()
{
    delete fileBrowserModel_;
}

void Window::onSendCommand(const QString &cmd)
{
    qDebug() << "Window::onSendCommand" << cmd;
    if ("Open" == cmd)
    {
        showFileBrowser();
    } else if ("Full Screen" == cmd)
    {
        fullScreen();
    } else if ("Go To Page" == cmd)
    {
        showGotoPage();
    } else if ("Zoom" == cmd)
    {
        showZoomPage();
    } else if ("Help" == cmd)
    {
        showHelp();
    } else if ("About" == cmd)
    {
        showAboutDialog();
    } else if ("Exit" == cmd)
    {
        close();
    } else
    {
        qDebug() << "unknown command";
    }
}

void Window::showFileBrowser()
{
    qDebug() << "Window::showFileBrowser";
    if (NULL == fileBrowser_)
    {
        if (NULL == (fileBrowser_ = new QDeclarativeView(this)))
        {
            showWarningMessage("Cannot create fileBrowser object", "not enough memory");
            return;
        }
        if (NULL == fileBrowserModel_)
        {
            showWarningMessage("fileBrowserObject is NULL");
            return;
        }
        fileBrowserModel_->searchPdfFiles();
        fileBrowser_->engine()->rootContext()->setContextProperty("pdfPreviewModel", fileBrowserModel_);
        fileBrowser_->setSource(QUrl("qrc:/qml/qml/filebrowser.qml"));
        fileBrowser_->setStyleSheet("background:transparent");
        fileBrowser_->setAttribute(Qt::WA_TranslucentBackground);
        fileBrowser_->setAttribute(Qt::WA_DeleteOnClose);
        fileBrowser_->setWindowFlags(Qt::FramelessWindowHint);
        fileBrowser_->move(0, 0);
        QObject *pDisp = fileBrowser_->rootObject();
        if (NULL != pDisp)
        {
            if (false == pDisp->setProperty("width", width()))
            {
                qDebug() << "cannot set width";
            }
            if (false == pDisp->setProperty("height", height()))
            {
                qDebug() << "cannot set height";
            }
            connect(pDisp, SIGNAL(changeDirectory(int)), fileBrowserModel_, SLOT(changeCurrentDir(int)));
            connect(pDisp, SIGNAL(showDocument(QString)), this, SLOT(closeFileBrowser(QString)));
            fileBrowser_->show();
        } else {
            qDebug() << "cannot get root object";
            delete fileBrowser_;
            fileBrowser_ = NULL;
        }
    }
}

void Window::closeFileBrowser(const QString &doc)
{
    qDebug() << "Window::closeFileBrowser" << doc;
    if ((NULL != fileBrowser_) && (true == fileBrowser_->close()))
    {
        qDebug() << "widget closed";
        fileBrowser_ = NULL;
        if (QString(CLOSE_FILE_BROWSER_TEXT) != doc)
        {
            openFile(doc);
        } else
        {
            qDebug() << "closing file browser";
        }
    }
}

void Window::showGotoPage()
{
    qDebug() << "Window::showGotoPage";
    if (NULL == gotoPage_)
    {
        gotoPage_ = new QDeclarativeView(this);
        gotoPage_->setSource(QUrl("qrc:/qml/qml/gotopage.qml"));
        gotoPage_->setStyleSheet("background:transparent");
        gotoPage_->setAttribute(Qt::WA_TranslucentBackground);
        gotoPage_->setAttribute(Qt::WA_DeleteOnClose);
        gotoPage_->setWindowFlags(Qt::FramelessWindowHint);
        gotoPage_->move((width()-gotoPage_->width())/2, (height()-gotoPage_->height())/2);
        QObject *pDisp = gotoPage_->rootObject()->findChild<QObject*>("disp");
        if (NULL != pDisp)
        {
            connect(pDisp, SIGNAL(setPage(QString)), this, SLOT(closeGotoPage(QString)));
            gotoPage_->show();
        } else {
            qDebug() << "cannot get disp object";
            delete gotoPage_;
            gotoPage_ = NULL;
        }
    }
}

void Window::closeGotoPage(const QString &pageNb)
{
    qDebug() << "Window::closeGotoPage: " << pageNb;
    if ((NULL != gotoPage_) && (true == gotoPage_->close()))
    {
        qDebug() << "widget closed";
        gotoPage_ = NULL;
        //set current page
        bool ok = false;
        int newPageNb = pageNb.toInt(&ok);
        int currentPage = document_->currentPage()+1;
        int numPages = document_->numPages();
        if ((true == ok) && (newPageNb != currentPage) &&
                (0 != newPageNb) && (newPageNb <= numPages))
        {
            gotoPage(newPageNb, numPages);
            if (currentPage < newPageNb)
            {
                slidingStacked_->slideInNext();
            } else {
                slidingStacked_->slideInPrev();
            }
        } else {
            qDebug() << "nothing to do";
        }
    }
}

void Window::showZoomPage()
{
    qDebug() << "Window::showZoomPage";
    if (NULL == zoomPage_)
    {
        zoomPage_ = new QDeclarativeView(this);
        zoomPage_->setSource(QUrl("qrc:/qml/qml/zoompage.qml"));
        zoomPage_->setStyleSheet("background:transparent");
        zoomPage_->setAttribute(Qt::WA_TranslucentBackground);
        zoomPage_->setAttribute(Qt::WA_DeleteOnClose);
        zoomPage_->setWindowFlags(Qt::FramelessWindowHint);
        zoomPage_->move((width()-zoomPage_->width())/2, (height()-zoomPage_->height())/2);
        zoomPage_->show();
        QObject *pZoomReel = zoomPage_->rootObject()->findChild<QObject*>("zoomreel");
        if (NULL != pZoomReel)
        {
            if (false == pZoomReel->setProperty("zoomIndex", currentZoomIndex_))
            {
                qDebug() << "cannot set property";
            }
            connect(pZoomReel, SIGNAL(setZoomFactor(int)), this, SLOT(closeZoomPage(int)));
            zoomPage_->show();
        } else {
            qDebug() << "cannot get disp object";
            delete zoomPage_;
            zoomPage_ = NULL;
        }
    }
}

void Window::closeZoomPage(int index)
{
    qDebug() << "Window::closeZoomPage" << scaleFactors_[index];
    if ((NULL != zoomPage_) && (true == zoomPage_->close()))
    {
        qDebug() << "widget closed";
        zoomPage_ = NULL;
        setZoomFactor(index);
    }
}

void Window::showCommandPopupMenu()
{
    qDebug() << "Window::showCommandPopupMenu";
    if (false == toolBar_->isHidden())
    {
        return;//show command popup menu only when the toolbar of hidden
    }
    if (NULL == commandPopupMenu_)
    {
        commandPopupMenu_ = new QDeclarativeView(this);
        commandPopupMenu_->setSource(QUrl("qrc:/qml/qml/popupmenu.qml"));
        commandPopupMenu_->setStyleSheet("background:transparent");
        commandPopupMenu_->setAttribute(Qt::WA_TranslucentBackground);
        commandPopupMenu_->setAttribute(Qt::WA_DeleteOnClose);
        commandPopupMenu_->setWindowFlags(Qt::FramelessWindowHint);
        commandPopupMenu_->move((width()-commandPopupMenu_->width())/2, (height()-commandPopupMenu_->height())/2);
        QObject *pDisp = commandPopupMenu_->rootObject()->findChild<QObject*>("popuplist");
        if (NULL != pDisp)
        {
            connect(pDisp, SIGNAL(itemClicked(QString)), this, SLOT(closeCommandPopupMenu(QString)));
            commandPopupMenu_->show();
        } else {
            qDebug() << "cannot get popuplist object";
            delete commandPopupMenu_;
            commandPopupMenu_ = NULL;
        }
    }
}

void Window::closeCommandPopupMenu(const QString &cmd)
{
    qDebug() << "Window::closeCommandPopupMenu" << cmd;
    if ((NULL != commandPopupMenu_) && (true == commandPopupMenu_->close()))
    {
        qDebug() << "widget closed";
        commandPopupMenu_ = NULL;
        if (QString("Open") == cmd)
        {
            showFileBrowser();
        } else if (QString("Go To Page") == cmd)
        {
            showGotoPage();
        } else if (QString("Show Page Number") == cmd)
        {
            togglePageDisplay();
        } else if (QString("Zoom") == cmd)
        {
            showZoomPage();
        } else if (QString("Exit") == cmd)
        {
            close();//TODO: close is not done
        } else if (QString("Normal Screen") == cmd)
        {
            normalScreen();
        } else
        {
            qDebug() << "unknown command";
        }
    }
}

void Window::openFile(const QString &filePath)
{
    qDebug() << "Window::openFile";
    //open document
    if (document_->setDocument(filePath))
    {
        setupDocDisplay(1, filePath);
        slidingStacked_->slideInNext();
    } else
    {
        showWarningMessage(tr(APPLICATION " - Failed to open file"),
                             tr("The specified file cannot be opened"));
    }
}

void Window::fullScreen()
{
    qDebug() << "Window::fullScreen";
    showFullScreen();
    toolBar_->hide();
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
}

void Window::normalScreen()
{
    qDebug() << "Window::normalScreen";

    QDesktopWidget *pDesktop = QApplication::desktop();
    if (NULL != pDesktop)
    {
        int width = pDesktop->width();
        int height = pDesktop->height();
        if ((FULL_SCREEN_WIDTH >= width) || (FULL_SCREEN_HEIGHT >= height))
        {
            qDebug() << "using full screen mode with toolbar";
            setFixedSize(width, height);
            showFullScreen();
            QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
        } else
        {
            qDebug() << "using normal mode";
            showNormal();
            resize((MIN_SCREEN_WIDTH<width)?MIN_SCREEN_WIDTH:width,
                         (MIN_SCREEN_HEIGHT<height)?MIN_SCREEN_HEIGHT:height);
            QApplication::restoreOverrideCursor();
        }
    }

    toolBar_->show();    
}

void Window::increaseScale()
{
    qDebug() << "Window::increaseScale";
}

void Window::decreaseScale()
{
    qDebug() << "Window::decreaseScale";
}

bool Window::eventFilter(QObject *, QEvent *event)
{
    if (QEvent::Wheel == event->type())
    {
        // * handle mouse wheel events
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);        
        if (0 > wheelEvent->delta())
        {
            showNextPage();
            return true;//stop further processing
        }
        if (0 < wheelEvent->delta())
        {
            showPrevPage();
            return true;
        }
    } else if (QEvent::MouseButtonPress == event->type()) {
        // * handle mouse events
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*> (event);
        startPoint_ = mouseEvent->pos();
        pressTimer_.start();
    } else if (QEvent::MouseButtonRelease == event->type()) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*> (event);
        endPoint_ = mouseEvent->pos();

        //process distance and direction
        int xDiff = qAbs(startPoint_.x() - endPoint_.x());
        int yDiff = qAbs(startPoint_.y() - endPoint_.y());
        if (xDiff <= 2*SWIPE_THRESHOLD && yDiff <= SWIPE_THRESHOLD) {
            if (pressTimer_.isValid() && pressTimer_.elapsed() >= LONG_PRESS_TIMEOUT_MS) {
                pressTimer_.invalidate();                   
                showCommandPopupMenu();
            }
        } else if( xDiff > yDiff )
        {
            // horizontal swipe detected, now find direction
            if( startPoint_.x() > endPoint_.x() )
            {
                //left swipe
                showNextPage();
            } else if ( startPoint_.x() < endPoint_.x() )
            {
                //right swipe
                showPrevPage();
            }
        }
        // vertical swipe is handled by Flickable class
    } else if (QEvent::KeyPress == event->type())
    {
        // * handle key events
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (Qt::Key_Escape == keyEvent->key() && isFullScreen())
        {
            normalScreen();
        }
        if (Qt::Key_PageDown == keyEvent->key())
        {
            showNextPage();
        }
        if (Qt::Key_PageUp == keyEvent->key())
        {
            showPrevPage();
        }
        if (Qt::Key_Home == keyEvent->key())
        {
            document_->setPage(1);
        }
        if (Qt::Key_End == keyEvent->key())
        {
            document_->setPage(document_->numPages());
        }
    }

    return false;
}

bool Window::showNextPage()
{
    qDebug() << "Window::showNextPage";
    if (false == document_->isLoaded() || false == animationFinished_)
    {
        return false;
    }

    int currentPage = document_->currentPage()+2;
    int nbPages = document_->numPages();
    if (currentPage <= nbPages)
    {
        //load a new page if any
        document_->setPage(currentPage);
        document_->showCurrentPageUpper();
        showPageNumber(currentPage, nbPages);
        //make sure that the next page is ready
        animationFinished_ = false;
        slidingStacked_->slideInNext();
        //emit signal to update the cache
        if (true == document_->invalidatePageCache(currentPage)) {
            emit updateCache(currentPage);//preload next page (page no starts from 0)
        }
        return true;
    }

    return false;
}

bool Window::showPrevPage()
{
    qDebug() << "Window::showPrevPage";
    if (false == document_->isLoaded() || false == animationFinished_)
    {
        return false;
    }

    int currentPage = document_->currentPage();
    int nbPages = document_->numPages();
    if (0 < currentPage)
    {        
        document_->setPage(currentPage);
        document_->showCurrentPageLower();
        showPageNumber(currentPage, nbPages);
        animationFinished_ = false;
        slidingStacked_->slideInPrev();
        //emit signal to update the cache
        if (true == document_->invalidatePageCache(currentPage-2)) {
            emit updateCache(currentPage-2);//preload previous page (page no starts from 0)
        }
        return true;
    }

    return false;
}

void Window::showPageNumber(int currentPage, int nbPages)
{
    if (false == showPageNumber_) {
        return;//do nothing if show page number is not enabled
    }
    QAction *act = pagePopupMenu_->actions()[0];
    act->setText(tr("page %1 of %2").arg(currentPage).arg(nbPages));
    pagePopupMenu_->popup(mapToGlobal(QPoint(width(),
                                             height()))+QPoint(10-width(), -40));
    QTimer::singleShot(TOOLTIP_VISIBLE_TIME_MS, pagePopupMenu_, SLOT(hide()));
}

void Window::closeEvent(QCloseEvent *evt)
{    
    qDebug() << "Window::closeEvent" << lastFilePath_ << document_->currentPage();
    if (QString(HELP_FILE) != lastFilePath_)
    {
        //the current settings are not saved if the last file is the help file
        QSettings settings(ORGANIZATION, APPLICATION);
        settings.setValue(KEY_PAGE, document_->currentPage());
        settings.setValue(KEY_FILE_PATH, lastFilePath_);
        settings.setValue(KEY_ZOOM_LEVEL, currentZoomIndex_);
    }

    worker_->quit();//terminate worker thread

    QWidget::closeEvent(evt);
}

void Window::setAnimationFlag()
{
    qDebug() << "Window::setAnimationFlag";
    animationFinished_ = true;
}

void Window::togglePageDisplay()
{
    qDebug() << "Window::togglePageDisplay";
    showPageNumber_ ^= true;
}

void Window::setupDocDisplay(unsigned int pageNumber, const QString &filePath)
{
    qDebug() << "Window::setupDocDisplay" << pageNumber;
    lastFilePath_ = filePath;
    int numPages = document_->numPages();    
    setWindowTitle(QString("%1 : ").arg(APPLICATION)+filePath);
    //set document zoom factor
    document_->setScale(scaleFactors_[currentZoomIndex_]);
    //set current page
    gotoPage(pageNumber, numPages);
}

void Window::gotoPage(int pageNb, int numPages)
{
    qDebug() << "Window::gotoPage";
    //set current page
    document_->invalidateCache();
    document_->setPage(pageNb);
    //preload next page
    if ((numPages-pageNb) > 0)
    {
        qDebug() << "Window::gotoPage: preload next page";
        emit updateCache(pageNb);//next page (index starts from 0)
    }
    //preload previous page
    if (pageNb > 1)
    {
        qDebug() << "Window::gotoPage: preload previous page";
        emit updateCache(pageNb-2);//previous page (index starts from 0)
    }
}

void Window::setZoomFactor(int index)
{
    qDebug() << "Window::setZoomFactor " << index;
    //set zoom factor
    if (currentZoomIndex_ == index)
    {
        return;//nothing to do
    }
    currentZoomIndex_ = index;
    document_->setScale(scaleFactors_[currentZoomIndex_]);
    //update all pages from circular buffer
    int pageNb = document_->currentPage()+1;
    int numPages = document_->numPages();
    document_->invalidateCache();
    document_->showPage(pageNb);
    //preload next page
    if ((numPages-pageNb) > 0)
    {
        qDebug() << "Window::gotoPage: preload next page";
        emit updateCache(pageNb);//next page (index starts from 0)
    }
    //preload previous page
    if (pageNb > 1)
    {
        qDebug() << "Window::gotoPage: preload previous page";
        emit updateCache(pageNb-2);//previous page (index starts from 0)
    }
    //update view
    slidingStacked_->slideInNext();
}

void Window::showHelp(bool slideNext)
{
    qDebug() << "Window::showHelp";
    QFile file(HELP_FILE);
    if (true == file.open(QIODevice::ReadOnly))
    {
        if (document_->loadFromData(file.readAll()))
        {
            setupDocDisplay(1, HELP_FILE);
            if (true == slideNext)
            {
                slidingStacked_->slideInNext();
            }
        } else
        {
            qDebug() << "cannot load from data";
        }
        file.close();
    } else
    {
        qDebug() << "cannot open help file";
        //TODO: display error message
    }
}

void Window::showAboutDialog()
{
    qDebug() << "Window::showAboutDialog";
    if (NULL == aboutDialog_)
    {
        aboutDialog_ = new QDeclarativeView(this);
        aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
        aboutDialog_->setStyleSheet("background:transparent");
        aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
        aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
        aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
        QObject *pAbout = aboutDialog_->rootObject();
        if (NULL != pAbout)
        {
            pAbout->setProperty("height", height());
            pAbout->setProperty("width", width());
            connect(pAbout, SIGNAL(closeAbout()), this, SLOT(closeAboutDialog()));
            QObject *pAboutDlg = pAbout->findChild<QObject*>("aboutDialog");
            if (NULL != pAboutDlg)
            {
            pAboutDlg->setProperty("text", "<H2>tabletReader v1.0</H2><br>"
                                "<H3>PDF viewer for touch-enabled devices</H3><br>"
                                "Copyright (C) 2011, Bogdan Cristea. All rights reserved.<br>"
                                "This program is distributed in the hope that it will be useful, "
                                "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                                "GNU General Public License for more details.<br><br>");
            } else
            {
                qDebug() << "cannot get aboutDialog object";
            }
            aboutDialog_->show();
        } else {
            qDebug() << "cannot get aboutDialog object";
            delete aboutDialog_;
            aboutDialog_ = NULL;
        }
    }
}

void Window::closeAboutDialog()
{
    qDebug() << "Window::closeAboutDialog";
    if ((NULL != aboutDialog_) && (true == aboutDialog_->close()))
    {
        qDebug() << "widget closed";
        aboutDialog_ = NULL;
    }
}

void Window::showWarningMessage(const QString &title, const QString &explanation)
{
    qDebug() << "Window::showWarningMessage";
    if (NULL == aboutDialog_)
    {
        aboutDialog_ = new QDeclarativeView(this);
        aboutDialog_->setSource(QUrl("qrc:/qml/qml/aboutdialog.qml"));
        aboutDialog_->setStyleSheet("background:transparent");
        aboutDialog_->setAttribute(Qt::WA_TranslucentBackground);
        aboutDialog_->setAttribute(Qt::WA_DeleteOnClose);
        aboutDialog_->setWindowFlags(Qt::FramelessWindowHint);
        QObject *pAbout = aboutDialog_->rootObject();
        if (NULL != pAbout)
        {
            pAbout->setProperty("height", height());
            pAbout->setProperty("width", width());
            connect(pAbout, SIGNAL(closeAbout()), this, SLOT(closeAboutDialog()));
            QObject *pAboutDlg = pAbout->findChild<QObject*>("aboutDialog");
            if (NULL != pAboutDlg)
            {
                if (false == pAboutDlg->setProperty("text", "<H3 style=\"color:red\">"+title+
                                       "</H3><br>"+explanation+"<br>"))
                {
                    qDebug() << "failed to set text";
                }
            } else
            {
                qDebug() << "cannot get aboutDialog object";
            }
            aboutDialog_->show();
        } else {
            qDebug() << "cannot get aboutDialog object";
            delete aboutDialog_;
            aboutDialog_ = NULL;
        }
    }
}
