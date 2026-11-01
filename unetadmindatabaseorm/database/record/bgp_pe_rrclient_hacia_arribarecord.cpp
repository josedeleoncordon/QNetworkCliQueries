#include "bgp_pe_rrclient_hacia_arribarecord.h"

bgp_pe_rrclient_hacia_arribaRecord::bgp_pe_rrclient_hacia_arribaRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_pe_rrclient_hacia_arriba";
}

bgp_pe_rrclient_hacia_arribaRecord::bgp_pe_rrclient_hacia_arribaRecord(const bgp_pe_rrclient_hacia_arribaRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_pe_rrclient_hacia_arriba";
	clone(other);
}

bgp_pe_rrclient_hacia_arribaRecord::~bgp_pe_rrclient_hacia_arribaRecord() {}

void bgp_pe_rrclient_hacia_arribaRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void bgp_pe_rrclient_hacia_arribaRecord::set_ok(int ok)
{
	setProperty("ok",ok);
}

void bgp_pe_rrclient_hacia_arribaRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void bgp_pe_rrclient_hacia_arribaRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void bgp_pe_rrclient_hacia_arribaRecord::set_pe_nombre(QString pe_nombre)
{
	setProperty("pe_nombre",pe_nombre);
}

void bgp_pe_rrclient_hacia_arribaRecord::set_peer(QString peer)
{
	setProperty("peer",peer);
}

QDate bgp_pe_rrclient_hacia_arribaRecord::fecha()
{
	return property("fecha").toDate();
}

int bgp_pe_rrclient_hacia_arribaRecord::ok()
{
	return property("ok").toInt();
}

QString bgp_pe_rrclient_hacia_arribaRecord::pais()
{
	return property("pais").toString();
}

QString bgp_pe_rrclient_hacia_arribaRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString bgp_pe_rrclient_hacia_arribaRecord::pe_nombre()
{
	return property("pe_nombre").toString();
}

QString bgp_pe_rrclient_hacia_arribaRecord::peer()
{
	return property("peer").toString();
}

bgp_pe_rrclient_hacia_arribaRecord &bgp_pe_rrclient_hacia_arribaRecord::operator=(const bgp_pe_rrclient_hacia_arribaRecord &other)
{
	clone(other);
	return *this;
}
