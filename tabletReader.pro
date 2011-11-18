HEADERS       = documentwidget.h \
                window.h \
    SlidingStackedWidget.h \
    filebrowser.h \
    flickable.h \
    worker.h
SOURCES       = documentwidget.cpp \
                main.cpp \
                window.cpp \
    SlidingStackedWidget.cpp \
    filebrowser.cpp \
    flickable.cpp \
    worker.cpp

INCLUDEPATH  += /usr/include/poppler/qt4
LIBS         += -L/usr/lib -lpoppler-qt4

#install
target.path = $$[install_prefix]/bin
icon.files = tabletReader.png
icon.path = $$[install_prefix]/share/icons
desktop.files = tabletReader.desktop
desktop.path = $$[install_prefix]/share/applications

INSTALLS += target icon desktop

RESOURCES += \
    images.qrc
