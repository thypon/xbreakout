QT += xml svg

DEPENDPATH += $$PWD $$PWD/sources
INCLUDEPATH += $$PWD $$PWD/sources


HEADERS += sources/ball.h \
           sources/brick.h \
           sources/canvasitems.h \
           sources/canvaswidget.h \
           sources/fontutils.h \
           sources/gameengine.h \
           sources/gift.h \
           sources/globals.h \
           sources/item.h \
           sources/levelloader.h \
           sources/mainwindow.h \
           sources/renderer.h \
           sources/textitems.h \
    sources/settings.h \
    sources/button.h \
    sources/dialog.h

SOURCES += sources/ball.cpp \
           sources/brick.cpp \
           sources/canvasitems.cpp \
           sources/canvaswidget.cpp \
           sources/fontutils.cpp \
           sources/gameengine.cpp \
           sources/gift.cpp \
           sources/item.cpp \
           sources/levelloader.cpp \
           sources/main.cpp \
           sources/mainwindow.cpp \
           sources/renderer.cpp \
           sources/textitems.cpp \
    sources/settings.cpp \
    sources/button.cpp \
    sources/dialog.cpp

RESOURCES += \
    resources/xbreakout.qrc
