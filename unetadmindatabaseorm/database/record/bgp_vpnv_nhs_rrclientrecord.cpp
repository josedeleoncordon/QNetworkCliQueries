#include "bgp_vpnv_nhs_rrclientrecord.h"

bgp_vpnv_nhs_rrclientRecord::bgp_vpnv_nhs_rrclientRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_vpnv_nhs_rrclient";
}

bgp_vpnv_nhs_rrclientRecord::bgp_vpnv_nhs_rrclientRecord(const bgp_vpnv_nhs_rrclientRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_vpnv_nhs_rrclient";
	clone(other);
}

bgp_vpnv_nhs_rrclientRecord::~bgp_vpnv_nhs_rrclientRecord() {}

void bgp_vpnv_nhs_rrclientRecord::set_address_family(QString address_family)
{
	setProperty("address_family",address_family);
}

void bgp_vpnv_nhs_rrclientRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void bgp_vpnv_nhs_rrclientRecord::set_ok(int ok)
{
	setProperty("ok",ok);
}

void bgp_vpnv_nhs_rrclientRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void bgp_vpnv_nhs_rrclientRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void bgp_vpnv_nhs_rrclientRecord::set_pe_nombre(QString pe_nombre)
{
	setProperty("pe_nombre",pe_nombre);
}

void bgp_vpnv_nhs_rrclientRecord::set_peer(QString peer)
{
	setProperty("peer",peer);
}

QString bgp_vpnv_nhs_rrclientRecord::address_family()
{
	return property("address_family").toString();
}

QDate bgp_vpnv_nhs_rrclientRecord::fecha()
{
	return property("fecha").toDate();
}

int bgp_vpnv_nhs_rrclientRecord::ok()
{
	return property("ok").toInt();
}

QString bgp_vpnv_nhs_rrclientRecord::pais()
{
	return property("pais").toString();
}

QString bgp_vpnv_nhs_rrclientRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString bgp_vpnv_nhs_rrclientRecord::pe_nombre()
{
	return property("pe_nombre").toString();
}

QString bgp_vpnv_nhs_rrclientRecord::peer()
{
	return property("peer").toString();
}

bgp_vpnv_nhs_rrclientRecord &bgp_vpnv_nhs_rrclientRecord::operator=(const bgp_vpnv_nhs_rrclientRecord &other)
{
	clone(other);
	return *this;
}
