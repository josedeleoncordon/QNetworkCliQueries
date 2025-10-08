#-------------------------------------------------
#
# Project created by QtCreator 2017-09-09T00:29:04
#
#-------------------------------------------------

include(../base.pri)

QT       += network

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

# DEFINES += HAVE_POSIX_OPENPT HAVE_SYS_TIME_H HAVE_UPDWTMPX #Pty Para cuando se usa Terminal

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix {
    #esto sirve para que el ejecutable en linux busque librerias tambien en el directorio donde el se encuentra

    #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
    # this expands to $ORIGIN (after qmake and make), it does NOT read a qmake var
    QMAKE_RPATHDIR += /usr/local/lib64/
    QMAKE_RPATHDIR += \$\$ORIGIN
    APP_RPATH = $$join(QMAKE_RPATHDIR, ":")

    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${APP_RPATH}\'
    QMAKE_RPATHDIR =
}

SOURCES += \
    qremoteshelllogging.cpp \
    qsshsession.cpp \
    qtelnet.cpp \    
    qremoteshell.cpp #\
    # terminal/Pty.cpp \
    # terminal/kprocess.cpp \
    # terminal/kpty.cpp \
    # terminal/kptydevice.cpp \
    # terminal/kptyprocess.cpp \
    # terminal/terminal.cpp \

HEADERS += \
    qremoteshelllogging.h \
    qsshsession.h \
    qtelnet.h \    
    qremoteshell.h\
    qremoteshell_global.h #\
    # terminal/Pty.h \
    # terminal/kprocess.h \
    # terminal/kpty.h \
    # terminal/kpty_p.h \
    # terminal/kptydevice.h \
    # terminal/kptyprocess.h \
    # terminal/terminal.h \

unix {
    target.path = /usr/lib64
    INSTALLS += target
}

#Previamente compilar libssh con soporte de DSA
LIBS *= -L/usr/local/lib64/ -lssh

