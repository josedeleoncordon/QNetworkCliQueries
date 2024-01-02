#include "recordfactory.h"

#include "record/tb_addressing_consultarecord.h"
#include "record/a9k_bgp_inter_asrecord.h"
#include "record/a9k_bgp_sesiones_sin_lp200record.h"
#include "record/a9k_bgp_sesiones_weightrecord.h"
#include "record/a9k_energiarecord.h"
#include "record/a9k_equiporecord.h"
#include "record/a9k_internet_redistribucion_estaticas_conectadasrecord.h"
#include "record/a9k_memoriarecord.h"
#include "record/a9k_mpls_label_table_summaryrecord.h"
#include "record/a9k_paisrecord.h"
#include "record/a9k_peer_caida_prefix_inrecord.h"
#include "record/a9k_sesiones_colarecord.h"
#include "record/a9k_slotsrecord.h"
#include "record/a9k_vrf_sin_label_pervrfrecord.h"
#include "record/arearecord.h"
#include "record/enlacerecord.h"
#include "record/equiporecord.h"
#include "record/gruporecord.h"
#include "record/grupo_equiporecord.h"
#include "record/interfazrecord.h"
#include "record/interfaz_estadorecord.h"
#include "record/interfaz_tiporecord.h"
#include "record/ken_consultarecord.h"
#include "record/ken_equiporecord.h"
#include "record/paisrecord.h"
#include "record/pcp_paisrecord.h"
#include "record/pcp_perecord.h"
#include "record/pcp_peerrecord.h"
#include "record/pcp_peer_tiporecord.h"
#include "record/pcp_proveedorrecord.h"
#include "record/pcp_regionrecord.h"
#include "record/proveedorrecord.h"
#include "record/red_consultarecord.h"
#include "record/red_consulta_detallerecord.h"
#include "record/red_paisrecord.h"
#include "record/red_sectorrecord.h"

using namespace QAnt;
using namespace Dao;

RecordBase *RecordFactory::newRecord(QString table)
{
	if ( table == "TB_Addressing_consulta" )
		return new TB_Addressing_consultaRecord;
	if ( table == "a9k_bgp_inter_as" )
		return new a9k_bgp_inter_asRecord;
	if ( table == "a9k_bgp_sesiones_sin_lp200" )
		return new a9k_bgp_sesiones_sin_lp200Record;
	if ( table == "a9k_bgp_sesiones_weight" )
		return new a9k_bgp_sesiones_weightRecord;
	if ( table == "a9k_energia" )
		return new a9k_energiaRecord;
	if ( table == "a9k_equipo" )
		return new a9k_equipoRecord;
	if ( table == "a9k_internet_redistribucion_estaticas_conectadas" )
		return new a9k_internet_redistribucion_estaticas_conectadasRecord;
	if ( table == "a9k_memoria" )
		return new a9k_memoriaRecord;
	if ( table == "a9k_mpls_label_table_summary" )
		return new a9k_mpls_label_table_summaryRecord;
	if ( table == "a9k_pais" )
		return new a9k_paisRecord;
	if ( table == "a9k_peer_caida_prefix_in" )
		return new a9k_peer_caida_prefix_inRecord;
	if ( table == "a9k_sesiones_cola" )
		return new a9k_sesiones_colaRecord;
	if ( table == "a9k_slots" )
		return new a9k_slotsRecord;
	if ( table == "a9k_vrf_sin_label_pervrf" )
		return new a9k_vrf_sin_label_pervrfRecord;
	if ( table == "area" )
		return new areaRecord;
	if ( table == "enlace" )
		return new enlaceRecord;
	if ( table == "equipo" )
		return new equipoRecord;
	if ( table == "grupo" )
		return new grupoRecord;
	if ( table == "grupo_equipo" )
		return new grupo_equipoRecord;
	if ( table == "interfaz" )
		return new interfazRecord;
	if ( table == "interfaz_estado" )
		return new interfaz_estadoRecord;
	if ( table == "interfaz_tipo" )
		return new interfaz_tipoRecord;
	if ( table == "ken_consulta" )
		return new ken_consultaRecord;
	if ( table == "ken_equipo" )
		return new ken_equipoRecord;
	if ( table == "pais" )
		return new paisRecord;
	if ( table == "pcp_pais" )
		return new pcp_paisRecord;
	if ( table == "pcp_pe" )
		return new pcp_peRecord;
	if ( table == "pcp_peer" )
		return new pcp_peerRecord;
	if ( table == "pcp_peer_tipo" )
		return new pcp_peer_tipoRecord;
	if ( table == "pcp_proveedor" )
		return new pcp_proveedorRecord;
	if ( table == "pcp_region" )
		return new pcp_regionRecord;
	if ( table == "proveedor" )
		return new proveedorRecord;
	if ( table == "red_consulta" )
		return new red_consultaRecord;
	if ( table == "red_consulta_detalle" )
		return new red_consulta_detalleRecord;
	if ( table == "red_pais" )
		return new red_paisRecord;
	if ( table == "red_sector" )
		return new red_sectorRecord;
	return nullptr;
}
