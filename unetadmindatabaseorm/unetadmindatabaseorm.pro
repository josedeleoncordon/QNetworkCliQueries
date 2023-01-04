include(unetadmindatabaseorm_dependencies.pri)

DESTDIR = $$APP_BUILD_TREE

DEFINES += QANT3_LIBRARY

TARGET = UNetadminDatabaseORM
TEMPLATE = lib
QT += sql network
INCLUDEPATH *= /usr/include/qant3
HEADERS += $$PWD/record/arearecord.h \
		$$PWD/record/enlacerecord.h \
		$$PWD/record/equiporecord.h \
		$$PWD/record/grupo_equiporecord.h \
		$$PWD/record/gruporecord.h \
		$$PWD/record/interfaz_estadorecord.h \
		$$PWD/record/interfaz_tiporecord.h \
		$$PWD/record/interfazrecord.h \
		$$PWD/record/paisrecord.h \
		$$PWD/record/pcp_paisrecord.h \
		$$PWD/record/pcp_pe_peerrecord.h \
		$$PWD/record/pcp_peer_tiporecord.h \
		$$PWD/record/pcp_peerrecord.h \
		$$PWD/record/pcp_perecord.h \
		$$PWD/record/pcp_proveedorrecord.h \
		$$PWD/record/proveedorrecord.h \
		$$PWD/qant3_global.h \
		$$PWD/recordfactory.h \
		$$PWD/record/pcp_regionrecord.h

SOURCES += $$PWD/record/arearecord.cpp \
		$$PWD/record/enlacerecord.cpp \
		$$PWD/record/equiporecord.cpp \
		$$PWD/record/grupo_equiporecord.cpp \
		$$PWD/record/gruporecord.cpp \
		$$PWD/record/interfaz_estadorecord.cpp \
		$$PWD/record/interfaz_tiporecord.cpp \
		$$PWD/record/interfazrecord.cpp \
		$$PWD/record/paisrecord.cpp \
		$$PWD/record/pcp_paisrecord.cpp \
		$$PWD/record/pcp_pe_peerrecord.cpp \
		$$PWD/record/pcp_peer_tiporecord.cpp \
		$$PWD/record/pcp_peerrecord.cpp \
		$$PWD/record/pcp_perecord.cpp \
		$$PWD/record/pcp_proveedorrecord.cpp \
		$$PWD/record/proveedorrecord.cpp \
		$$PWD/recordfactory.cpp \
		$$PWD/record/pcp_regionrecord.cpp

RESOURCES += schema.qrc

unix:LIBS += -lqant3

# Default rules for deployment.
unix {
    target.path = /usr/lib64
}
!isEmpty(target.path): INSTALLS += target
