include(database_dependencies.pri)

DESTDIR = $$APP_BUILD_TREE

DEFINES += QANT3_LIBRARY

TARGET = unetadmindatabaseorm
TEMPLATE = lib
QT += sql network
INCLUDEPATH *= /usr/include/qant3

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += $$PWD/record/a9k_bgp_inter_asrecord.h \
		$$PWD/record/a9k_bgp_sesiones_sin_lp200record.h \
		$$PWD/record/a9k_bgp_sesiones_weightrecord.h \
		$$PWD/record/a9k_energiarecord.h \
		$$PWD/record/a9k_equiporecord.h \
		$$PWD/record/a9k_internet_redistribucion_estaticas_conectadasrecord.h \
		$$PWD/record/a9k_memoriarecord.h \
		$$PWD/record/a9k_mpls_label_table_summaryrecord.h \
		$$PWD/record/a9k_paisrecord.h \
		$$PWD/record/a9k_peer_caida_prefix_inrecord.h \
		$$PWD/record/a9k_sesiones_colarecord.h \
		$$PWD/record/a9k_slotsrecord.h \
		$$PWD/record/a9k_vrf_sin_label_pervrfrecord.h \
		$$PWD/record/arearecord.h \
		$$PWD/record/bgp_serv_prin_bck_comunidades_faltantesrecord.h \
		$$PWD/record/bgp_sesiones_clientes_sin_prefix_filtrorecord.h \
		$$PWD/record/enlacerecord.h \
		$$PWD/record/equiporecord.h \
		$$PWD/record/grupo_equiporecord.h \
		$$PWD/record/gruporecord.h \
		$$PWD/record/interfaz_estadorecord.h \
		$$PWD/record/interfaz_tiporecord.h \
		$$PWD/record/interfazrecord.h \
		$$PWD/record/ken_consulta_detallerecord.h \
		$$PWD/record/ken_consultarecord.h \
		$$PWD/record/ken_equiporecord.h \
		$$PWD/record/paisrecord.h \
		$$PWD/record/pcp_paisrecord.h \
		$$PWD/record/pcp_peer_tiporecord.h \
		$$PWD/record/pcp_peerrecord.h \
		$$PWD/record/pcp_perecord.h \
		$$PWD/record/pcp_proveedorrecord.h \
		$$PWD/record/pcp_regionrecord.h \
		$$PWD/record/proveedorrecord.h \
		$$PWD/record/red_consulta_detallerecord.h \
		$$PWD/record/red_consultarecord.h \
		$$PWD/record/red_paisrecord.h \
		$$PWD/record/red_sectorrecord.h \
		$$PWD/record/tb_addressing_consultarecord.h \
		$$PWD/record/tb_audit_reportrecord.h \
		$$PWD/record/tb_pais_anillo_bus_detallerecord.h \
		$$PWD/record/tb_pais_anillos_cantidadrecord.h \
		$$PWD/qant3_global.h \
		$$PWD/recordfactory.h

SOURCES += $$PWD/record/a9k_bgp_inter_asrecord.cpp \
		$$PWD/record/a9k_bgp_sesiones_sin_lp200record.cpp \
		$$PWD/record/a9k_bgp_sesiones_weightrecord.cpp \
		$$PWD/record/a9k_energiarecord.cpp \
		$$PWD/record/a9k_equiporecord.cpp \
		$$PWD/record/a9k_internet_redistribucion_estaticas_conectadasrecord.cpp \
		$$PWD/record/a9k_memoriarecord.cpp \
		$$PWD/record/a9k_mpls_label_table_summaryrecord.cpp \
		$$PWD/record/a9k_paisrecord.cpp \
		$$PWD/record/a9k_peer_caida_prefix_inrecord.cpp \
		$$PWD/record/a9k_sesiones_colarecord.cpp \
		$$PWD/record/a9k_slotsrecord.cpp \
		$$PWD/record/a9k_vrf_sin_label_pervrfrecord.cpp \
		$$PWD/record/arearecord.cpp \
		$$PWD/record/bgp_serv_prin_bck_comunidades_faltantesrecord.cpp \
		$$PWD/record/bgp_sesiones_clientes_sin_prefix_filtrorecord.cpp \
		$$PWD/record/enlacerecord.cpp \
		$$PWD/record/equiporecord.cpp \
		$$PWD/record/grupo_equiporecord.cpp \
		$$PWD/record/gruporecord.cpp \
		$$PWD/record/interfaz_estadorecord.cpp \
		$$PWD/record/interfaz_tiporecord.cpp \
		$$PWD/record/interfazrecord.cpp \
		$$PWD/record/ken_consulta_detallerecord.cpp \
		$$PWD/record/ken_consultarecord.cpp \
		$$PWD/record/ken_equiporecord.cpp \
		$$PWD/record/paisrecord.cpp \
		$$PWD/record/pcp_paisrecord.cpp \
		$$PWD/record/pcp_peer_tiporecord.cpp \
		$$PWD/record/pcp_peerrecord.cpp \
		$$PWD/record/pcp_perecord.cpp \
		$$PWD/record/pcp_proveedorrecord.cpp \
		$$PWD/record/pcp_regionrecord.cpp \
		$$PWD/record/proveedorrecord.cpp \
		$$PWD/record/red_consulta_detallerecord.cpp \
		$$PWD/record/red_consultarecord.cpp \
		$$PWD/record/red_paisrecord.cpp \
		$$PWD/record/red_sectorrecord.cpp \
		$$PWD/record/tb_addressing_consultarecord.cpp \
		$$PWD/record/tb_audit_reportrecord.cpp \
		$$PWD/record/tb_pais_anillo_bus_detallerecord.cpp \
		$$PWD/record/tb_pais_anillos_cantidadrecord.cpp \
		$$PWD/recordfactory.cpp

RESOURCES += schema.qrc

unix:LIBS += -lqant3
