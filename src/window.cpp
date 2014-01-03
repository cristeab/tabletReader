/****************************************************************************
**
** Copyright (C) 2013, Bogdan Cristea. All rights reserved.
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

#include "config.h"
#ifdef QT5
#include <QGuiApplication>
#include <QQmlContext>
#include <QScreen>
#include <QWidget>
#include <QSettings>
#endif
#ifndef NO_QTMOBILITY
#include <QtSystemInfo/QSystemDeviceInfo>
#include <QtSystemInfo/QSystemBatteryInfo>
#endif
#include "window.h"
#include "filebrowsermodel.h"
#include "qmlcppmediator.h"
#include "settings_keys.h"

#define HELP_FILE "/../share/doc/tabletReaderHelp.pdf"

#ifndef NO_QTMOBILITY
QTM_USE_NAMESPACE
#endif

Window::Window()
  : document_(NULL)
    , fileBrowserModel_(NULL)
    , fullScreen_(false)
    , helpFile_(QCoreApplication::applicationDirPath()+QString(HELP_FILE))
    , rootObj_(NULL)
    , mediator_(NULL)
	, requestedPage_(0)
#ifndef NO_QTMOBILITY
	, batteryInfo_(new QSystemBatteryInfo(this))
#endif
{
#ifdef QT5
  setResizeMode(QQuickView::SizeRootObjectToView);
#else  
  setResizeMode(QDeclarativeView::SizeRootObjectToView);
#endif

  eTime_.start();//used to measure the elapsed time since the app was started
#ifdef QT5
  QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#else
  QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

  //load previous document if any or the help
  document_ = new PageProvider();//deleted by QDeclarativeView
  bool loadRes = loadSettings();

  normalScreen();//need to set early the window width in order to use the fit width zoom factor

  //create file browser (uses supported file types given by OkularDocument)
  const QStringList &formats = document_->supportedFilePatterns();
  fileBrowserModel_ = new FileBrowserModel(this, document_, formats);
  if (loadRes && (0 != helpFile_.compare(document_->filePath()))) {
    fileBrowserModel_->setCurrentDir(document_->filePath());
  }

  //add page provider before setting the source
#ifdef QT5
  QQmlEngine *eng = engine();
#else
  QDeclarativeEngine *eng = engine();
#endif
  if (NULL != eng) {
    eng->addImageProvider("pageprovider", document_);
    eng->rootContext()->setContextProperty("pdfPreviewModel", fileBrowserModel_);
    connect(eng, SIGNAL(quit()), this, SLOT(onQuit()));
  }
  //set context property
  mediator_ = new QmlCppMediator(document_, fileBrowserModel_);
  setWindowSize(normScrGeometry_.width(), normScrGeometry_.height());
  rootContext()->setContextProperty("mediator", mediator_);
#ifdef QT5
  setSource(QUrl("qrc:/qml/qml/qt5_main.qml"));
#else
  setSource(QUrl("qrc:/qml/qml/main.qml"));
#endif

  rootObj_ = rootObject();
  if (NULL != rootObj_) {
    //setup GUI
    setZoomIndex(document_->scaleIndex());
    //set about props
    if (false == rootObj_->setProperty("version", TR_VERSION)) {
      qDebug() << "cannot set version";
    }
    if (false == rootObj_->setProperty("formats", formats.join(", "))) {
      qDebug() << "cannot set formats";
    }
    //set filebrowser handlers
    connect(rootObj_, SIGNAL(chDir(int)), fileBrowserModel_, SLOT(changeCurrentDir(int)));
    connect(rootObj_, SIGNAL(showDoc(QString,int)), this, SLOT(onShowDocument(QString,int)));
    connect(fileBrowserModel_, SIGNAL(quit()), this, SLOT(onQuit()));
    //set full screen button handlers
    connect(rootObj_, SIGNAL(fullScreen()), this, SLOT(onFullScreen()));
    connect(rootObj_, SIGNAL(normalScreen()), this, SLOT(onNormalScreen()));
    //set goto page handler
    connect(rootObj_, SIGNAL(gotoPage(int)), this, SLOT(onGotoPage(int)));
    //set zoom handler
    connect(rootObj_, SIGNAL(setZoomFactor(int,int)), this, SLOT(onSetZoomFactor(int,int)));
    //set properties handler
    connect(rootObj_, SIGNAL(setProperties()), this, SLOT(onSetProperties()));
    //set help handler
    connect(rootObj_, SIGNAL(showHelp(bool)), this, SLOT(onShowHelp(bool)));
    //set quit handler
    connect(rootObj_, SIGNAL(quit()), this, SLOT(onQuitApp()));
    //set wait handler
    connect(this, SIGNAL(wait()), rootObj_, SLOT(onWait()));
    //set warning handler
    connect(this, SIGNAL(warning(QVariant)), rootObj_, SLOT(onWarning(QVariant)));
  }

  if (true == loadRes) {
    gotoPage(requestedPage_, document_->numPages());
    onQuit();
    refreshPage();
  }
  else {
    //no document has been found, just show a warning
    emit warning(tr("No document found"));
  }

  Qt::WindowFlags winFlags = Qt::Window | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint |
                             Qt::WindowCloseButtonHint;
#ifdef QT5
  setFlags(winFlags);
  setTitle("TabletReader " TR_VERSION);
  QGuiApplication::restoreOverrideCursor();
#else
  setWindowFlags(winFlags);
  setWindowTitle("TabletReader " TR_VERSION);
  QApplication::restoreOverrideCursor();
#endif
}

Window::~Window()
{
  delete fileBrowserModel_;
  delete mediator_;
}

void Window::onShowDocument(const QString &doc, int page)
{
  qDebug() << "Window::onShowDocument" << doc << ", page" << page;

  if(fileBrowserModel_->closeFileBrowserText() != doc) {
    openFile(doc, page);
  }
  else {
    onQuit();
  }
}

void Window::onGotoPage(int page)
{
  qDebug() << "Window::onGotoPage: " << page;

  //set current page
  int currentPage = document_->currentPage();
  int numPages = document_->numPages();
  if((page != currentPage) && (0 <= page) && (page < numPages)) {
    //change page
    gotoPage(page, numPages);
    if(currentPage < page) {
      showNextPage();
    }
    else {
      showPrevPage();
    }
  }
  onQuit();
}

void Window::onSetZoomFactor(int value, int index)
{
  qDebug() << "Window::onSetZoomFactor" << value << ", index" << index;

  //convert value to qreal
  qreal scale = value/100.0;//fit width is coded by -100
  if (scale != document_->scaleFactor()) {
    document_->setScale(scale, index);
    setZoomIndex(index);
    gotoPage(document_->currentPage(), document_->numPages());
    refreshPage();
  }
  onQuit();
}

void Window::openFile(const QString &filePath, int page)
{
  qDebug() << "Window::openFile";

  //open document
  if(document_->setDocument(filePath)) {
    //load document
    gotoPage(page, document_->numPages());
    refreshPage();
    //remove wait page if any
    onQuit();
  }
  else {
    emit warning(tr("Failed to open file") + tr("<br>%1 cannot be opened").arg(filePath));
  }
}

void Window::onFullScreen()
{
  qDebug() << "Window::fullScreen";

  if (true == fullScreen_) {
    return;
  }
  normScrGeometry_ = geometry();//store current geometry

  if(true == hasTouchScreen()) {
#ifdef QT5
    QGuiApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
#else
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
#endif
  }
#ifdef QT5
  int width = QGuiApplication::primaryScreen()->size().width();
  int height = QGuiApplication::primaryScreen()->size().height();
#else
  int width = QApplication::desktop()->width();
  int height = QApplication::desktop()->height();
#endif
  qDebug() << "width " << width << ", height " << height;
#ifndef _WIN32
  setGeometry(QRect(0, 0, width, height));//TODO: meant only for PA
#endif
  //set QML size
  setWindowSize(width, height);
  //set window width for fit width
  document_->setWinWidth(width);
  //update page width if needed
  updateViewForFitWidth();

  showFullScreen();//seems to work only on desktops

  fullScreen_ = true;
  onQuit();
}

void Window::normalScreen()
{
  qDebug() << "Window::normalScreen";

  fullScreen_ = false;

  if (false == normScrGeometry_.isValid()) {
    //compute a geometry if none available
#ifdef QT5
    int desktopWidth = QGuiApplication::primaryScreen()->size().width();
    int desktopHeight = QGuiApplication::primaryScreen()->size().height();
#else
    int desktopWidth = QApplication::desktop()->width();
    int desktopHeight = QApplication::desktop()->height();
#endif
  	int width = 0;
	  int height = 0;
    if((MIN_SCREEN_WIDTH >= desktopWidth) && (MIN_SCREEN_HEIGHT >= desktopHeight)) {
      qDebug() << "using full screen mode with toolbar";
	  width = desktopWidth;
	  height = desktopHeight;
      showFullScreen();
      if(true == hasTouchScreen()) {
#ifdef QT5
        QGuiApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
#else
        QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
#endif
      }
    }
    else {
      qDebug() << "using normal mode";
      width = (MIN_SCREEN_WIDTH < desktopWidth) ? MIN_SCREEN_WIDTH : desktopWidth;
      height = (MIN_SCREEN_HEIGHT < desktopHeight) ? MIN_SCREEN_HEIGHT : desktopHeight;
      showNormal();
      if(true == hasTouchScreen()) {
#ifdef QT5
        QGuiApplication::restoreOverrideCursor();
#else
        QApplication::restoreOverrideCursor();
#endif
      }
#ifdef QT5
	  resize(width, height);
      setPosition((desktopWidth-width)/2, (desktopHeight-height)/2);
#else
      move((desktopWidth-width)/2, (desktopHeight-height)/2);
#endif
    }
    normScrGeometry_.setWidth(width);
    normScrGeometry_.setHeight(height);
  }
  else {
    setGeometry(normScrGeometry_);
    showNormal();
  }

  //set window width for fit width
  document_->setWinWidth(normScrGeometry_.width());
}

void Window::onNormalScreen()
{
  qDebug() << "Window::onNormalScreen";

  normalScreen();
  //set QML size
  setWindowSize(normScrGeometry_.width(), normScrGeometry_.height());
  //update page width if needed
  updateViewForFitWidth();
  onQuit();
}

void Window::showNextPage()
{
  qDebug() << "Window::showNextPage";
  QObject *obj = rootObj_->findChild<QObject*>("view");
  if (NULL != obj) {
    if (false == QMetaObject::invokeMethod(obj, "incrementCurrentIndex")) {
      qDebug() << "cannot invoke method";
    }
  }
  else {
    qDebug() << "cannot find view";
  }
}

void Window::showPrevPage()
{
  qDebug() << "Window::showPrevPage";
  if (false == document_->cacheReady()) {
    qDebug() << "ignore request";
    return;
  }
  QObject *obj = rootObj_->findChild<QObject*>("view");
  if (NULL != obj) {
    if (false == QMetaObject::invokeMethod(obj, "decrementCurrentIndex")) {
      qDebug() << "cannot invoke method";
    }
  }
  else {
    qDebug() << "cannot find view";
  }
}

#ifdef QT5
bool Window::event(QEvent *evt)
{
  if ((NULL != evt) && (QEvent::Close == evt->type())) {
    saveSettings();
  }

  return QQuickView::event(evt);
}
#else
void Window::closeEvent(QCloseEvent *evt)
{
  qDebug() << "Window::closeEvent";
  saveSettings();
  QWidget::closeEvent(evt);
}
#endif

void Window::gotoPage(int pageNb, int numPages)
{
  qDebug() << "Window::gotoPage: page nb" << pageNb << ", numPages" << numPages;

  //show wait page
  emit wait();

  //update the entire cache
  document_->gotoPage(pageNb);

  //set current page and the total number of pages on the GUI
  setCurrentPage(pageNb, numPages);
}

void Window::onShowHelp(bool show)
{
  qDebug() << "Window::onShowHelp" << show;

  QString curFileName = "";
  int curPage = 0;

  if (true == show) {
    //store the current file name and page number
    prev_.fileName = document_->filePath();
    prev_.page = document_->currentPage();
    curFileName = helpFile_;
    curPage = 0;
  }
  else {
    //restore previous file name and page number
    curFileName = prev_.fileName;
    curPage = prev_.page;
  }

  if(document_->setDocument(curFileName)) {
    gotoPage(curPage, document_->numPages());
    refreshPage();
    onQuit();
    //display "Back" icon next time
    if (NULL != mediator_) {
      mediator_->setHlpBck(false);
    }
  }
  else {
    qDebug() << "cannot open file";
    prev_.page = 0;
    emit warning(tr("Cannot open help file"));
  }
}

bool Window::hasTouchScreen()
{
#ifndef NO_QTMOBILITY
  QSystemDeviceInfo systemInfo;
  QSystemDeviceInfo::InputMethodFlags flags = systemInfo.inputMethodType();
  return ((flags & (QSystemDeviceInfo::SingleTouch |
                    QSystemDeviceInfo::MultiTouch)) != 0) ? true : false;
#else
  return false;
#endif
}

QString Window::batteryStatus()
{
  QString msg("N/A");
#ifndef NO_QTMOBILITY
  switch(batteryInfo_->chargerType()) {
  case QSystemBatteryInfo::NoCharger:
    msg = tr("no charger");
    break;
  case QSystemBatteryInfo::WallCharger:
    msg = tr("wall charger");
    break;
  case QSystemBatteryInfo::USBCharger:
  case QSystemBatteryInfo::USB_500mACharger:
  case QSystemBatteryInfo::USB_100mACharger:
  case QSystemBatteryInfo::VariableCurrentCharger:
    msg = tr("charging");
    break;
  case QSystemBatteryInfo::UnknownCharger:
  default:
    msg = tr("unknown");
  }
  int remCap = batteryInfo_->remainingCapacityPercent();
  if(-1 != remCap) {
    msg += tr(", %1% remaining capacity").arg(remCap);
  }
  else {
    msg += tr(", unknown remaining capacity");
  }
#endif
  return msg;
}

QString Window::elapsedTime()
{
  QString msg = "";
  qint64 eTimeMs = eTime_.elapsed();
  qint64 eTimeHrs = eTimeMs / (1000 * 3600);
  if(0 != eTimeHrs) {
    if(1 == eTimeHrs) {
      msg += tr("1 hour");
    }
    else {
      msg += tr("%1 hours").arg(eTimeHrs);
    }
  }
  qint64 eTimeMin = (eTimeMs - eTimeHrs * 1000 * 3600) / (1000 * 60);
  if(0 != eTimeMin) {
    if(0 != eTimeHrs) {
      msg += ", ";
    }
    if(1 == eTimeMin) {
      msg += tr("1 minute");
    }
    else {
      msg += tr("%1 minutes").arg(eTimeMin);
    }
  }
  qint64 eTimeSec = (eTimeMs - eTimeHrs * 1000 * 3600 - eTimeMin * 1000 * 60) / 1000;
  if(0 != eTimeSec) {
    if((0 != eTimeMin) || (0 != eTimeHrs)) {
      msg += ", ";
    }
    if(1 == eTimeSec) {
      msg += tr("1 second");
    }
    else {
      msg += tr("%1 seconds").arg(eTimeSec);
    }
  }
  return msg;
}

bool Window::loadSettings()
{
  qDebug() << "Window::loadSettings";
  if (NULL == document_) {
	  qDebug() << "Document object not created";
	  return false;
  }
  QSettings settings(ORGANIZATION, APPLICATION);
  QString filePath = settings.value(KEY_FILE_PATH).toString();
  if (filePath.isEmpty()) {
	  filePath = helpFile_;
  }
  bool rc = document_->setDocument(filePath);
  if (false == rc) {
	  qDebug() << "Cannot load document" << filePath;
  }
  requestedPage_ = settings.value(KEY_PAGE, 0).toInt();
  if (requestedPage_ >= document_->numPages()) {
    requestedPage_ = 0;//make sure that this is a valid value
  }
  qreal scaleFactor = settings.value(KEY_ZOOM_LEVEL, FIT_WIDTH_ZOOM_FACTOR).toFloat();
  int scaleIndex = settings.value(KEY_ZOOM_INDEX, FIT_WIDTH_ZOOM_INDEX).toInt();
  document_->setScale(scaleFactor, scaleIndex);
  return rc;
}

void Window::saveSettings()
{
  qDebug() << "PageProvider::saveSettings";
  if(true == document_->isLoaded()) {
    const QString &fileName = document_->filePath();
    if (fileName != helpFile_) {
      QSettings settings(ORGANIZATION, APPLICATION);
      settings.setValue(KEY_PAGE, document_->currentPage());
      settings.setValue(KEY_FILE_PATH, fileName);
      settings.setValue(KEY_ZOOM_LEVEL, document_->scaleFactor());
      settings.setValue(KEY_ZOOM_INDEX, document_->scaleIndex());
    }
  }
}

void Window::onQuit()
{
  qDebug() << "Window::onQuit";
  if (NULL != rootObj_) {
    QObject *obj = rootObj_->findChild<QObject*>("mainLoader");
    if ((NULL != obj) && (false == obj->setProperty("source", ""))) {
      qDebug() << "cannot set source";
    }
    obj = rootObj_->findChild<QObject*>("toolbarmouse");
    if ((NULL != obj) && (false == obj->setProperty("enabled", true))) {
      qDebug() << "cannot set toolbar mouse area";
    }
  }
}

void Window::onQuitApp()
{
  qDebug() << "Window::onQuitApp";
  saveSettings();
  close();
}

void Window::onSetProperties()
{
  qDebug() << "Window::onSetProperties";
  if (NULL != rootObj_) {
    if (false == rootObj_->setProperty("filePath", document_->filePath())) {
      qDebug() << "cannot set file path";
    }
    if (false == rootObj_->setProperty("elapsedTime", elapsedTime())) {
      qDebug() << "cannot set elapsed time";
    }
    if (false == rootObj_->setProperty("batteryStatus", batteryStatus())) {
      qDebug() << "cannot set battery status";
    }
  }
}
