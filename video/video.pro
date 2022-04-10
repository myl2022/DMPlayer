#-------------------------------------------------
#
# Project created by QtCreator 2020-05-19T21:58:38
#
#-------------------------------------------------

QT += core gui

TARGET = video
TEMPLATE = lib

DESTDIR = $$PWD/../lib
DLLDESTDIR = $$PWD/../bin

DEFINES += VIDEO_LIBRARY
OBJECTS_DIR = $$PWD/../build
MOC_DIR = $$PWD/../build

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../include

SOURCES += \
        render.cpp \
        video.cpp \
        videodecodec.cpp

HEADERS += \
        render.h \
        $$PWD/../include/video.h \
        $$PWD/../include/video_global.h  \
        videodecodec.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

LIBS += -L$$PWD/../lib

LIBS += -lavcodec \
             -lavformat \
             -lswscale \
             -lavutil \
             -lSDL2 \
             -lswresample
