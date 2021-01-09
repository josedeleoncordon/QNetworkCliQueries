#include "recordfactory.h"

#include "record/arearecord.h"
#include "record/enlacerecord.h"
#include "record/equiporecord.h"
#include "record/gruporecord.h"
#include "record/grupo_equiporecord.h"
#include "record/interfazrecord.h"
#include "record/interfaz_estadorecord.h"
#include "record/interfaz_tiporecord.h"
#include "record/paisrecord.h"
#include "record/pcp_paisrecord.h"
#include "record/pcp_perecord.h"
#include "record/pcp_pe_peerrecord.h"
#include "record/pcp_peerrecord.h"
#include "record/pcp_peer_tiporecord.h"
#include "record/pcp_proveedorrecord.h"
#include "record/proveedorrecord.h"

using namespace QAnt;
using namespace Dao;

RecordBase *RecordFactory::newRecord(QString table)
{
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
	if ( table == "pais" )
		return new paisRecord;
	if ( table == "pcp_pais" )
		return new pcp_paisRecord;
	if ( table == "pcp_pe" )
		return new pcp_peRecord;
	if ( table == "pcp_pe_peer" )
		return new pcp_pe_peerRecord;
	if ( table == "pcp_peer" )
		return new pcp_peerRecord;
	if ( table == "pcp_peer_tipo" )
		return new pcp_peer_tipoRecord;
	if ( table == "pcp_proveedor" )
		return new pcp_proveedorRecord;
	if ( table == "proveedor" )
		return new proveedorRecord;
	return nullptr;
}
