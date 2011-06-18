HEADERS       = documentwidget.h \
                window.h \
    SlidingStackedWidget.h \
    loadpages.h \
    filebrowser.h
SOURCES       = documentwidget.cpp \
                main.cpp \
                window.cpp \
    SlidingStackedWidget.cpp \
    loadpages.cpp \
    filebrowser.cpp

INCLUDEPATH  += /usr/include/poppler/qt4
LIBS         += -L/usr/lib -lpoppler-qt4
