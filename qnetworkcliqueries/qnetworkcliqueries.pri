include(../qremoteshell/qremoteshell.pri)

INCLUDEPATH *= $$PWD $$PWD/base
LIBS *= -L$$APP_BUILD_TREE -lqnetworkcliqueries
