include(../tests.pri)

TARGET = tst_ospfinfonokia
SOURCES += tst_ospfinfonokia.cpp

# Ruta a las salidas CLI de muestra
DEFINES += FIXTURES_DIR=\\\"$$PWD/../fixtures\\\"
