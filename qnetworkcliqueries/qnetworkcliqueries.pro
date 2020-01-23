include(../base.pri)
include(../qremoteshell/qremoteshell.pri)

DESTDIR = $$APP_BUILD_TREE

INCLUDEPATH += base

TARGET = qnetworkcliqueries
TEMPLATE = lib

DEFINES += QNETWORKCLIQUERIES_LIBRARY
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

HEADERS += \
    base/exitinfo.h \
    base/mplstetunnelsinfo.h \
    base/mplsvpnl2info.h \
    base/portchannelsinfo.h \
    base/vrfinfo.h \
    base/arpinfo.h \
    base/bgpneighborinfo.h \
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
    funciones.h \
    properties.h \
    qnetworkquerieslogging.h \
    queries.h \
    queriesmessagehandler.h \
    queriesthread.h \
    base/platforminfo.h \
    Cisco/macinfocisco.h \
    Cisco/ospfinfocisco.h \
    Huawei/macinfohuawei.h \
    Huawei/ospfinfohuawei.h \
    RAD/macinforad.h \
    base/config.h \    
    Cisco/arpinfocisco.h \
    Huawei/arpinfohuawei.h \
    qnetworkcliqueries_global.h

SOURCES += \
    base/exitinfo.cpp \
    base/mplstetunnelsinfo.cpp \
    base/mplsvpnl2info.cpp \
    base/portchannelsinfo.cpp \
    base/vrfinfo.cpp \
    base/arpinfo.cpp \
    base/bgpneighborinfo.cpp \
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
    funciones.cpp \
    properties.cpp \
    qnetworkquerieslogging.cpp \
    queries.cpp \
    queriesmessagehandler.cpp \
    queriesthread.cpp \
    base/platforminfo.cpp \
    Cisco/macinfocisco.cpp \
    Cisco/ospfinfocisco.cpp \
    Huawei/macinfohuawei.cpp \
    Huawei/ospfinfohuawei.cpp \
    RAD/macinforad.cpp \
    base/config.cpp \    
    Cisco/arpinfocisco.cpp \
    Huawei/arpinfohuawei.cpp
