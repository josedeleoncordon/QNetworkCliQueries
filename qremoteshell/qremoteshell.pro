#-------------------------------------------------
#
# Project created by QtCreator 2017-09-09T00:29:04
#
#-------------------------------------------------

include(../base.pri)

QT       += widgets network

DESTDIR = $$APP_BUILD_TREE

TARGET = qremoteshell
TEMPLATE = lib

DEFINES += QREMOTESHELL_LIBRARY
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += HAVE_POSIX_OPENPT HAVE_SYS_TIME_H HAVE_UPDWTMPX #Pty

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    terminal/Pty.cpp \
    terminal/kprocess.cpp \
    terminal/kpty.cpp \
    terminal/kptydevice.cpp \
    terminal/kptyprocess.cpp \
    terminal/terminal.cpp \
    qremoteshell.cpp

HEADERS += \
    terminal/Pty.h \
    terminal/kprocess.h \
    terminal/kpty.h \
    terminal/kpty_p.h \
    terminal/kptydevice.h \
    terminal/kptyprocess.h \
    terminal/terminal.h \
    qremoteshell.h\
    qremoteshell_global.h

