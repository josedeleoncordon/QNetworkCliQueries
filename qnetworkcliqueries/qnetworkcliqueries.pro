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
#    base/placeinfo.h \
    bdhosts.h \
    bdhostsupdater.h \
    factory.h \
    funciones.h \
    properties.h \
    queries.h \
    queriesthread.h \
    base/platforminfo.h \
    Cisco/macinfocisco.h \
    Cisco/ospfinfocisco.h \
    Huawei/macinfohuawei.h \
    Huawei/ospfinfohuawei.h \
    RAD/macinforad.h \
    base/config.h \    
    dbhosts.h \
    Cisco/arpinfocisco.h \
    Huawei/arpinfohuawei.h \
    qnetworkcliqueries_global.h

SOURCES += \
    base/exitinfo.cpp \
    base/mplstetunnelsinfo.cpp \
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
#    base/placeinfo.cpp \
    bdhosts.cpp \
    bdhostsupdater.cpp \
    factory.cpp \
    funciones.cpp \
    properties.cpp \
    queries.cpp \
    queriesthread.cpp \
    base/platforminfo.cpp \
    Cisco/macinfocisco.cpp \
    Cisco/ospfinfocisco.cpp \
    Huawei/macinfohuawei.cpp \
    Huawei/ospfinfohuawei.cpp \
    RAD/macinforad.cpp \
    base/config.cpp \    
    dbhosts.cpp \
    Cisco/arpinfocisco.cpp \
    Huawei/arpinfohuawei.cpp
