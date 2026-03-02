#include "bgp_lu_sesion_hacia_rr_detallerecord.h"

bgp_lu_sesion_hacia_rr_detalleRecord::bgp_lu_sesion_hacia_rr_detalleRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_lu_sesion_hacia_rr_detalle";
}

bgp_lu_sesion_hacia_rr_detalleRecord::bgp_lu_sesion_hacia_rr_detalleRecord(const bgp_lu_sesion_hacia_rr_detalleRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_lu_sesion_hacia_rr_detalle";
	clone(other);
}

bgp_lu_sesion_hacia_rr_detalleRecord::~bgp_lu_sesion_hacia_rr_detalleRecord() {}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_detalle(QString detalle)
{
	setProperty("detalle",detalle);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_estado(QString estado)
{
	setProperty("estado",estado);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_ok(int ok)
{
	setProperty("ok",ok);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_pe_nombre(QString pe_nombre)
{
	setProperty("pe_nombre",pe_nombre);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_peer_ip(QString peer_ip)
{
	setProperty("peer_ip",peer_ip);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_pfxincount(int pfxincount)
{
	setProperty("pfxincount",pfxincount);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_pfxoutcount(int pfxoutcount)
{
	setProperty("pfxoutcount",pfxoutcount);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_rpl_in_ok(int rpl_in_ok)
{
	setProperty("rpl_in_ok",rpl_in_ok);
}

void bgp_lu_sesion_hacia_rr_detalleRecord::set_rpl_out_ok(int rpl_out_ok)
{
	setProperty("rpl_out_ok",rpl_out_ok);
}

QString bgp_lu_sesion_hacia_rr_detalleRecord::detalle()
{
	return property("detalle").toString();
}

QString bgp_lu_sesion_hacia_rr_detalleRecord::estado()
{
	return property("estado").toString();
}

QDate bgp_lu_sesion_hacia_rr_detalleRecord::fecha()
{
	return property("fecha").toDate();
}

int bgp_lu_sesion_hacia_rr_detalleRecord::ok()
{
	return property("ok").toInt();
}

QString bgp_lu_sesion_hacia_rr_detalleRecord::pais()
{
	return property("pais").toString();
}

QString bgp_lu_sesion_hacia_rr_detalleRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString bgp_lu_sesion_hacia_rr_detalleRecord::pe_nombre()
{
	return property("pe_nombre").toString();
}

QString bgp_lu_sesion_hacia_rr_detalleRecord::peer_ip()
{
	return property("peer_ip").toString();
}

int bgp_lu_sesion_hacia_rr_detalleRecord::pfxincount()
{
	return property("pfxincount").toInt();
}

int bgp_lu_sesion_hacia_rr_detalleRecord::pfxoutcount()
{
	return property("pfxoutcount").toInt();
}

int bgp_lu_sesion_hacia_rr_detalleRecord::rpl_in_ok()
{
	return property("rpl_in_ok").toInt();
}

int bgp_lu_sesion_hacia_rr_detalleRecord::rpl_out_ok()
{
	return property("rpl_out_ok").toInt();
}

bgp_lu_sesion_hacia_rr_detalleRecord &bgp_lu_sesion_hacia_rr_detalleRecord::operator=(const bgp_lu_sesion_hacia_rr_detalleRecord &other)
{
	clone(other);
	return *this;
}
