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
#include "window.h"
#include "SlidingStackedWidget.h"
#include "filebrowser.h"

#define ORGANIZATION "home"
#define APPLICATION "tabletReader"
#define KEY_PAGE "current_page"
#define KEY_FILE_PATH "current_file_path"
#define KEY_ZOOM_LEVEL "current_zoom_level"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    //main window
    resize(858, 600);
    QWidget *centralWidget = new QWidget(this);
    QGridLayout *gridLayout = new QGridLayout(centralWidget);
    setCentralWidget(centralWidget);
    setWindowTitle(tr(APPLICATION));

    //browser dialog
    fileBrowser_  = new FileBrowser("*.pdf", this);
    connect(fileBrowser_, SIGNAL(picked(QString)), this, SLOT(openFile(QString)));

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

    //create main document
    document_ = new DocumentWidget(this);

    //create sliding animation
    slidingStacked_ = new SlidingStackedWidget(this);

    //init document pages and the sliding animation
    QScrollArea *scroll = NULL;
    QLabel *label = NULL;
    register int n = 0;
    for (n = 0; n < DocumentWidget::BUFFER_LEN; ++n)
    {
        //scroll areas (one for each page)
        scroll = new QScrollArea(centralWidget);
        scroll->setWidgetResizable(true);
        scroll->setAlignment(Qt::AlignCenter);
        label = new QLabel();//QLabel is used to display a page
        label->setAlignment(Qt::AlignCenter);
        scroll->setWidget(label);
        scroll->installEventFilter(this);
        document_->setScrollArea(scroll);//scroll areas used by the document widget
        slidingStacked_->addWidget(scroll);//scroll areas are switched by the stacked widget
    }
    document_->setPhysicalDpi(label->physicalDpiX(), label->physicalDpiY());
    slidingStacked_->setSpeed(HORIZONTAL_SLIDE_SPEED_MS);
    slidingStacked_->setWrap(true);
    slidingStacked_->setVerticalMode(false);
    //assuming that the first index of the sliding stacked widget is always zero

    gridLayout->addWidget(slidingStacked_, 0, 0, 1, 1);

    //main toolbar
    toolBar_ = new QToolBar(this);
    toolBar_->setMovable(false);
    toolBar_->setAllowedAreas(Qt::TopToolBarArea);
    toolBar_->setFloatable(false);
    addToolBar(Qt::TopToolBarArea, toolBar_);

    //setup button icons
    QString appDirPath = QCoreApplication::applicationDirPath();
    QToolButton *toolButton_open = new QToolButton(toolBar_);
    QIcon iconOpen = QIcon(appDirPath+
                           "/icons/Folder-Blue-Documents-icon.png");
    toolButton_open->setIcon(iconOpen);
    toolButton_open->setText(tr("&Open"));
    toolButton_open->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar_->addWidget(toolButton_open);

    QToolButton *toolButton_fullScreen = new QToolButton(toolBar_);
    QIcon iconFullScreen = QIcon(appDirPath+
                                 "/icons/window-full-screen-icon.png");
    toolButton_fullScreen->setIcon(iconFullScreen);
    toolButton_fullScreen->setText(tr("&Full Screen"));
    toolButton_fullScreen->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar_->addWidget(toolButton_fullScreen);

    QToolButton *toolButton_exit = new QToolButton(toolBar_);
    QIcon iconExit = QIcon(appDirPath+
                           "/icons/Button-Close-icon.png");
    toolButton_exit->setIcon(iconExit);
    toolButton_exit->setText(tr("E&xit"));
    toolButton_exit->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolBar_->addWidget(toolButton_exit);

    QLabel *pageLabel = new QLabel();
    toolBar_->addWidget(pageLabel);

    pageSpinBox_ = new QSpinBox();
    toolBar_->addWidget(pageSpinBox_);

    labelNbPages_ = new QLabel();
    labelNbPages_->setText("      ");
    toolBar_->addWidget(labelNbPages_);

    scaleComboBox_ = new QComboBox();
    for (int n = 0; n < scaleFactors_.size(); ++n)
    {
        scaleComboBox_->addItem(QString("%1%").arg(scaleFactors_[n]*100));
        if (1 == scaleFactors_[n])
        {
            scaleComboBox_->setCurrentIndex(n);
        }
    }
    toolBar_->addWidget(scaleComboBox_);

    loadPagesThread_ = new LoadPages(document_);

    connect(toolButton_fullScreen, SIGNAL(clicked()), this, SLOT(fullScreen()));
    connect(toolButton_open, SIGNAL(clicked()), this, SLOT(showFileBrowser()));
    connect(toolButton_exit, SIGNAL(clicked()), this, SLOT(close()));

    connect(pageSpinBox_, SIGNAL(valueChanged(int)),
            document_, SLOT(setPage(int)));
    connect(document_, SIGNAL(pageChanged(int)),
            pageSpinBox_, SLOT(setValue(int)));
    connect(document_, SIGNAL(pageChanged(int)),
            loadPagesThread_, SLOT(loadPages(int)));
    connect(scaleComboBox_, SIGNAL(currentIndexChanged(int)),
            this, SLOT(scaleDocument(int)));
    connect(slidingStacked_, SIGNAL(animationFinished()),
            this, SLOT(setAnimationFlag()));

    connect(increaseScaleAction, SIGNAL(triggered()), this, SLOT(increaseScale()));
    connect(decreaseScaleAction, SIGNAL(triggered()), this, SLOT(decreaseScale()));

    statusBar()->hide();

    //page number popup menu
    pagePopupMenu_ = new QMenu(this);
    pagePopupMenu_->setBackgroundRole(QPalette::ToolTipText);
    pagePopupMenu_->installEventFilter(this);
    pagePopupMenu_->addAction("");//this contains the page number

    //command popup menu
    commandPopupMenu_ = new QMenu(this);
    QAction *act = commandPopupMenu_->addAction(iconOpen, tr("Open ..."));
    connect(act, SIGNAL(triggered()), this, SLOT(showFileBrowser()));
    act = commandPopupMenu_->addAction(iconFullScreen, tr("Normal Screen"));
    act->setDisabled(true);
    connect(act, SIGNAL(triggered()), this, SLOT(normalScreen()));
    act = commandPopupMenu_->addAction(iconExit, tr("Exit"));
    connect(act, SIGNAL(triggered()), this, SLOT(close()));
    commandPopupMenu_->addAction(tr("Go To Page ..."));
    //TODO: add goto page
    commandPopupMenu_->addAction(tr("Zoom ..."));
    //TODO: add zoom page

    //set document if one has been previously open
    QSettings settings(ORGANIZATION, APPLICATION);
    QString filePath;
    if (NULL == (filePath = settings.value(KEY_FILE_PATH).toString()))
    {
        return;//nothing to do
    }
    qDebug() << "Window::Window: Preparing to load an old document";
    if (document_->setDocument(filePath))
    {        
        setupDocDisplay(settings.value(KEY_PAGE, 0).toInt()+1, filePath);
        scaleComboBox_->setCurrentIndex(settings.value(KEY_ZOOM_LEVEL, 3).toInt());
        qDebug() << "Window::Window: Loaded document: " << filePath;
    }
    animationFinished_ = true;
}

Window::~Window()
{
}

void Window::showFileBrowser()
{
    fileBrowser_->setMinimumSize(size());
    fileBrowser_->show();
}

void Window::openFile(const QString &filePath)
{
    qDebug() << "Window::openFile: " << filePath;
    //close dialog
    fileBrowser_->close();
    //open document
    if (document_->setDocument(filePath)) {
        setupDocDisplay(1, filePath);
    } else {
        QMessageBox::warning(this, tr("PDF Viewer - Failed to open file"),
                             tr("The specified file could not be opened"));
    }
}

void Window::fullScreen()
{
    showFullScreen();
    toolBar_->hide();
}

void Window::normalScreen()
{
    showNormal();
    toolBar_->show();
}

void Window::scaleDocument(int index)
{
    document_->setScale(scaleFactors_[index]);
}

void Window::increaseScale()
{
    int nextIdx = scaleComboBox_->currentIndex()+1;
    if (scaleComboBox_->count() > nextIdx)
    {
        scaleComboBox_->setCurrentIndex(nextIdx);
    }
}

void Window::decreaseScale()
{
    int prevIdx = scaleComboBox_->currentIndex()-1;
    if (0 <= prevIdx)
    {
        scaleComboBox_->setCurrentIndex(prevIdx);
    }
}

bool Window::eventFilter(QObject *, QEvent *event)
{
    if (QEvent::Wheel == event->type())
    {
        // * handle mouse wheel events
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);        
        if (0 > wheelEvent->delta())
        {
            qDebug() << "Window::eventFilter: next";
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
        if (xDiff <= SWIPE_THRESHOLD && yDiff <= SWIPE_THRESHOLD) {
            if (pressTimer_.isValid() && pressTimer_.elapsed() >= LONG_PRESS_TIMEOUT_MS) {
                pressTimer_.invalidate();
                if(isFullScreen()) {
                    //enable full screen action
                    commandPopupMenu_->actions()[1]->setEnabled(true);
                } else {
                    commandPopupMenu_->actions()[1]->setDisabled(true);
                }
                commandPopupMenu_->exec(QCursor::pos());
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
        } else {
            // vertical swipe detected
            document_->showCurrentPageVertical(endPoint_.y()-startPoint_.y());
        }
    } else if (QEvent::KeyPress == event->type())
    {
        // * handle key events
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (Qt::Key_Escape == keyEvent->key() && isFullScreen())
        {
            normalScreen();
            return true;
        }
        if (Qt::Key_PageDown == keyEvent->key())
        {
            showNextPage();
            return true;
        }
        if (Qt::Key_PageUp == keyEvent->key())
        {
            showPrevPage();
            return true;
        }
        if (Qt::Key_Home == keyEvent->key())
        {
            document_->setPage(1);
            return true;
        }
        if (Qt::Key_End == keyEvent->key())
        {
            document_->setPage(document_->numPages());
            return true;
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
        return true;
    }

    return false;
}

void Window::showPageNumber(int currentPage, int nbPages)
{
    QAction *act = pagePopupMenu_->actions()[0];
    act->setText(tr("page %1 of %2").arg(currentPage).arg(nbPages));
    pagePopupMenu_->popup(mapToGlobal(QPoint(width(), height()))+QPoint(10-width(), -40));
    QTimer::singleShot(TOOLTIP_VISIBLE_TIME_MS, pagePopupMenu_, SLOT(hide()));
}

void Window::closeEvent(QCloseEvent *evt)
{
    qDebug() << "Window::closeEvent";
    if (!lastFilePath_.isEmpty()) {
        QSettings settings(ORGANIZATION, APPLICATION);
        settings.setValue(KEY_PAGE, document_->currentPage());
        settings.setValue(KEY_FILE_PATH, lastFilePath_);
        settings.setValue(KEY_ZOOM_LEVEL, scaleComboBox_->currentIndex());
        QWidget::closeEvent(evt);
    }
}

void Window::setAnimationFlag()
{
    animationFinished_ = true;
}

void Window::setupDocDisplay(unsigned int pageNumber, const QString &filePath)
{
    lastFilePath_ = filePath;
    scaleComboBox_->setEnabled(true);
    pageSpinBox_->setEnabled(true);
    loadPagesThread_->preloadPage(pageNumber-1);//preload first page(s) to be displayed
    int numPages = document_->numPages();
    pageSpinBox_->setRange(1, numPages);
    pageSpinBox_->setValue(pageNumber);//sends signal for loading document page    
    labelNbPages_->setText(tr("/ %1").arg(numPages));
    setWindowTitle(QString("%1 : ").arg(APPLICATION)+filePath);
}
