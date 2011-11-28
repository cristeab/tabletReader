HEADERS       = documentwidget.h \
                window.h \
    SlidingStackedWidget.h \
    filebrowser.h \
    flickable.h \
    worker.h \
    logger.h
SOURCES       = documentwidget.cpp \
                main.cpp \
                window.cpp \
    SlidingStackedWidget.cpp \
    filebrowser.cpp \
    flickable.cpp \
    worker.cpp \
    logger.cpp

QT           +=     declarative
INCLUDEPATH  +=     /usr/include/poppler/qt4
LIBS         +=     -L/usr/lib -lpoppler-qt4
DEFINES      +=     DEBUG

#install
target.path = $$[install_prefix]/bin
icon.files = tabletReader.png
icon.path = $$[install_prefix]/share/icons
desktop.files = tabletReader.desktop
desktop.path = $$[install_prefix]/share/applications

INSTALLS += target icon desktop

RESOURCES += \
    images.qrc

OTHER_FILES += \
    qml/gotopage.qml \
    qml/Core/images/display.png \
    qml/Core/images/button-purple.png \
    qml/Core/images/button-green.png \
    qml/Core/images/button-blue.png \
    qml/Core/images/button-.png \
    qml/Core/Display.qml \
    qml/Core/calculator.js \
    qml/Core/Button.qml \
    qml/popupmenu.qml \
    qml/PopupMenu/PopupListModel.qml \
    qml/PopupMenu/PopupList.qml \
    qml/PopupMenu/ListItem.qml
