QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig debug
PKGCONFIG += mpv

CONFIG += c++11

LIBS += -lsqlite3
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bookmark.cpp \
    bookmarks.cpp \
    itemdelegatelibrary.cpp \
    labelseekbar.cpp \
    library.cpp \
    mediaplayer.cpp \
    mpveventloop.cpp \
    sleeptimer.cpp \
    treeviewbookmarks.cpp \
    treeviewlibrary.cpp \
    main.cpp \
    mainwindow.cpp \
    progress.cpp \
    settings.cpp \
    utils.cpp

HEADERS += \
    bookmark.h \
    bookmarks.h \
    itemdelegatelibrary.h \
    labelseekbar.h \
    library.h \
    mainwindow.h \
    mediaplayer.h \
    mpveventloop.h \
    progress.h \
    settings.h \
    sleeptimer.h \
    treeviewbookmarks.h \
    treeviewlibrary.h \
    utils.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    qtible_en_US.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
