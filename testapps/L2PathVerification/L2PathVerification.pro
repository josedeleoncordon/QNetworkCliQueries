#-------------------------------------------------
#
# Project created by QtCreator 2017-09-18T01:49:15
#
#-------------------------------------------------

include(../../base.pri)
include(../../qremoteshell/qremoteshell.pri)
include(../../prtlfunc/prtlfunc.pri)

QT       += core gui
DESTDIR = $$APP_BUILD_TREE

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = L2PathVerification
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

unix {
    #esto sirve para que el ejecutable en linux busque librerias tambien en el directorio donde el se encuentra

    #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
    # this expands to $ORIGIN (after qmake and make), it does NOT read a qmake var
    QMAKE_RPATHDIR += \$\$ORIGIN
    #QMAKE_RPATHDIR += \$\$ORIGIN/plugins
    APP_PLUGINS_RPATH = $$join(QMAKE_RPATHDIR, ":")

    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${APP_PLUGINS_RPATH}\'
    QMAKE_RPATHDIR =
}

SOURCES += main.cpp\
        mainwindow.cpp \
    interfaceinfoqueriesmodel.cpp

HEADERS  += mainwindow.h \
    interfaceinfoqueriesmodel.h

FORMS    += mainwindow.ui

RESOURCES += \
    resourses.qrc
