include(../base.pri)
include(../qremoteshell/qremoteshell.pri)

DESTDIR = $$APP_BUILD_TREE

INCLUDEPATH += base

CONFIG += c++11

QT += dbus
TARGET = qnetworkcliqueries
TEMPLATE = lib

DEFINES += QNETWORKCLIQUERIES_LIBRARY
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

HEADERS += \
    base/exitinfo.h \
    base/mplsl2transport.h \
    base/mplstetunnelsinfo.h \
    base/portchannelsinfo.h \
    base/vrfinfo.h \
    base/arpinfo.h \
    base/bgpinfo.h \
    base/equipmentneighbors.h \
    base/funcionbase.h \
    base/interfaceinfo.h \
    base/iprouteinfo.h \
    base/macinfo.h \
    base/mplsldpinfo.h \
    base/ospfinfo.h \
    base/piminfo.h \
    consultaequipos.h \
    factory.h \
    base/funcioninfo.h \
    funciones.h \
    properties.h \
    qnetworkquerieslogging.h \
    queries.h \
#    queriesdao.h \
    queriesmessagehandler.h \
    queriesthread.h \
    base/platforminfo.h \
    Cisco/macinfocisco.h \
    Cisco/ospfinfocisco.h \
    Huawei/macinfohuawei.h \
    Huawei/ospfinfohuawei.h \
    RAD/macinforad.h \
	base/config.h \
	queriesthreadworker.h \
    Cisco/arpinfocisco.h \
    Huawei/arpinfohuawei.h \
    qnetworkcliqueries_global.h

SOURCES += \
    base/exitinfo.cpp \
    base/mplsl2transport.cpp \
    base/mplstetunnelsinfo.cpp \
    base/portchannelsinfo.cpp \
    base/vrfinfo.cpp \
    base/arpinfo.cpp \
    base/bgpinfo.cpp \
    base/equipmentneighbors.cpp \
    base/funcionbase.cpp \
    base/interfaceinfo.cpp \
    base/iprouteinfo.cpp \
    base/macinfo.cpp \
    base/mplsldpinfo.cpp \
    base/ospfinfo.cpp \
    base/piminfo.cpp \
    consultaequipos.cpp \
    factory.cpp \
    base/funcioninfo.cpp \
    funciones.cpp \
    properties.cpp \
    qnetworkquerieslogging.cpp \
    queries.cpp \
#    queriesdao.cpp \
    queriesmessagehandler.cpp \
    queriesthread.cpp \
    base/platforminfo.cpp \
    Cisco/macinfocisco.cpp \
    Cisco/ospfinfocisco.cpp \
    Huawei/macinfohuawei.cpp \
    Huawei/ospfinfohuawei.cpp \
    RAD/macinforad.cpp \
	base/config.cpp \
	queriesthreadworker.cpp \
    Cisco/arpinfocisco.cpp \
    Huawei/arpinfohuawei.cpp

unix {
    target.path = /usr/lib64
    INSTALLS += target
}
