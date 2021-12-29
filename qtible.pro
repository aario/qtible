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
    models/bookmark.cpp \
    treeviews/bookmarks/treeviewsourcebookmarks.cpp \
    treeviews/library/itemdelegatelibrary.cpp \
    mediaplayer/labelseekbar.cpp \
    treeviews/library/treeviewsourcelibrary.cpp \
    mediaplayer/mediaplayer.cpp \
    mediaplayer/mpveventloop.cpp \
    sleeptimer.cpp \
    treeviews/bookmarks/treeviewbookmarks.cpp \
    treeviews/library/treeviewlibrary.cpp \
    main.cpp \
    mainwindow.cpp \
    models/progress.cpp \
    settings.cpp \
    utils.cpp

HEADERS += \
    models/bookmark.h \
    treeviews/bookmarks/treeviewsourcebookmarks.h \
    treeviews/library/itemdelegatelibrary.h \
    mediaplayer/labelseekbar.h \
    treeviews/library/treeviewsourcelibrary.h \
    mainwindow.h \
    mediaplayer/mediaplayer.h \
    mediaplayer/mpveventloop.h \
    models/progress.h \
    settings.h \
    sleeptimer.h \
    treeviews/bookmarks/treeviewbookmarks.h \
    treeviews/library/treeviewlibrary.h \
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
