#-------------------------------------------------
#
# Project created by QtCreator 2011-10-17T14:43:17
#
#-------------------------------------------------

QT       += core gui

TARGET = sink
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    msg_tool.cpp \
    sinkcomponent.cpp \
    qmf-thread.cpp \
    dialogopen.cpp \
    dialogobjects.cpp \
    object-details.cpp \
    object-model.cpp \
    sink.cpp

HEADERS  += mainwindow.h \
    sinkcomponent.h \
    qmf-thread.h \
    dialogopen.h \
    dialogobjects.h \
    object-details.h \
    object-model.h \
    msg_tool.h \
    sink.h

FORMS    += mainwindow.ui \
    sinkcomponent.ui \
    dialogopen.ui \
    dialogobjects.ui

RESOURCES += \
    sink.qrc

OTHER_FILES += \
    license.txt
