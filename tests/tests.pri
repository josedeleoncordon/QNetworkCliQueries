# Configuración común para todos los tests.
# Cada tst_*.pro debe hacer include(../tests.pri)

include(../base.pri)
include(../qnetworkcliqueries/qnetworkcliqueries.pri)

QT += testlib
QT -= gui
CONFIG += console testcase c++17
CONFIG -= app_bundle

DESTDIR = $$APP_BUILD_TREE/tests
QMAKE_LFLAGS += -Wl,-rpath,\'\$$ORIGIN/..\' -Wl,-rpath,/usr/local/lib64
