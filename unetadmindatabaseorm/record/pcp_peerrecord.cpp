#include "pcp_peerrecord.h"

pcp_peerRecord::pcp_peerRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="pcp_peer";
}

pcp_peerRecord::pcp_peerRecord(const pcp_peerRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="pcp_peer";
	clone(other);
}

pcp_peerRecord::~pcp_peerRecord() {}

void pcp_peerRecord::set_comunidad_base(QString comunidad_base)
{
	setProperty("comunidad_base",comunidad_base);
}

void pcp_peerRecord::set_estado(bool estado)
{
	setProperty("estado",estado);
}

void pcp_peerRecord::set_id(unsigned int id)
{
	setProperty("id",id);
}

void pcp_peerRecord::set_ip_pe(QString ip_pe)
{
	setProperty("ip_pe",ip_pe);
}

void pcp_peerRecord::set_ip_peer(QString ip_peer)
{
	setProperty("ip_peer",ip_peer);
}

void pcp_peerRecord::set_pcp_pais_id(int pcp_pais_id)
{
	setProperty("pcp_pais_id",pcp_pais_id);
}

void pcp_peerRecord::set_pcp_peer_tipo_id(int pcp_peer_tipo_id)
{
	setProperty("pcp_peer_tipo_id",pcp_peer_tipo_id);
}

void pcp_peerRecord::set_pcp_proveedor_id(unsigned int pcp_proveedor_id)
{
	setProperty("pcp_proveedor_id",pcp_proveedor_id);
}

void pcp_peerRecord::set_vrf(QString vrf)
{
	setProperty("vrf",vrf);
}

QString pcp_peerRecord::comunidad_base()
{
	return property("comunidad_base").toString();
}

bool pcp_peerRecord::estado()
{
	return property("estado").toBool();
}

unsigned int pcp_peerRecord::id()
{
	return property("id").toUInt();
}

QString pcp_peerRecord::ip_pe()
{
	return property("ip_pe").toString();
}

QString pcp_peerRecord::ip_peer()
{
	return property("ip_peer").toString();
}

int pcp_peerRecord::pcp_pais_id()
{
	return property("pcp_pais_id").toInt();
}

int pcp_peerRecord::pcp_peer_tipo_id()
{
	return property("pcp_peer_tipo_id").toInt();
}

unsigned int pcp_peerRecord::pcp_proveedor_id()
{
	return property("pcp_proveedor_id").toUInt();
}

QString pcp_peerRecord::vrf()
{
	return property("vrf").toString();
}

pcp_paisRecord *pcp_peerRecord::r_pcp_pais_id()
{
	return dynamic_cast<pcp_paisRecord*>( property("r_pcp_pais_id").value<RecordBase*>() );
}

pcp_peer_tipoRecord *pcp_peerRecord::r_pcp_peer_tipo_id()
{
	return dynamic_cast<pcp_peer_tipoRecord*>( property("r_pcp_peer_tipo_id").value<RecordBase*>() );
}

pcp_proveedorRecord *pcp_peerRecord::r_pcp_proveedor_id()
{
	return dynamic_cast<pcp_proveedorRecord*>( property("r_pcp_proveedor_id").value<RecordBase*>() );
}

pcp_peerRecord &pcp_peerRecord::operator=(const pcp_peerRecord &other)
{
	clone(other);
	return *this;
}
