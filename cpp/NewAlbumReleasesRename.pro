# -------------------------------------------------
# Project created by QtCreator 2009-06-18T14:08:35
# -------------------------------------------------
TARGET = NewAlbumReleasesRename
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    album.cpp \
    narworker.cpp \
    filenotfoundexception.cpp \
    exception.cpp \
    queuemanager.cpp
HEADERS += mainwindow.h \
    album.h \
    narworker.h \
    irenameworkinglistener.h \
    filenotfoundexception.h \
    exception.h \
    queuemanager.h \
    iqueuelistener.h
FORMS += mainwindow.ui
INCLUDEPATH +=  /usr/include/taglib
LIBS += -ltag
