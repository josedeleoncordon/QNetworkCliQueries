#include "ken_flow_asn_summaryrecord.h"

ken_flow_asn_summaryRecord::ken_flow_asn_summaryRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="ken_flow_asn_summary";
}

ken_flow_asn_summaryRecord::ken_flow_asn_summaryRecord(const ken_flow_asn_summaryRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="ken_flow_asn_summary";
	clone(other);
}

ken_flow_asn_summaryRecord::~ken_flow_asn_summaryRecord() {}

void ken_flow_asn_summaryRecord::set_asn_destino(QString asn_destino)
{
	setProperty("asn_destino",asn_destino);
}

void ken_flow_asn_summaryRecord::set_asn_origen(QString asn_origen)
{
	setProperty("asn_origen",asn_origen);
}

void ken_flow_asn_summaryRecord::set_equipo(QString equipo)
{
	setProperty("equipo",equipo);
}

void ken_flow_asn_summaryRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void ken_flow_asn_summaryRecord::set_max_bps(double max_bps)
{
	setProperty("max_bps",max_bps);
}

void ken_flow_asn_summaryRecord::set_p95_bps(double p95_bps)
{
	setProperty("p95_bps",p95_bps);
}

void ken_flow_asn_summaryRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void ken_flow_asn_summaryRecord::set_source_connectivity_type(QString source_connectivity_type)
{
	setProperty("source_connectivity_type",source_connectivity_type);
}

QString ken_flow_asn_summaryRecord::asn_destino()
{
	return property("asn_destino").toString();
}

QString ken_flow_asn_summaryRecord::asn_origen()
{
	return property("asn_origen").toString();
}

QString ken_flow_asn_summaryRecord::equipo()
{
	return property("equipo").toString();
}

QDate ken_flow_asn_summaryRecord::fecha()
{
	return property("fecha").toDate();
}

double ken_flow_asn_summaryRecord::max_bps()
{
	return property("max_bps").toDouble();
}

double ken_flow_asn_summaryRecord::p95_bps()
{
	return property("p95_bps").toDouble();
}

QString ken_flow_asn_summaryRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString ken_flow_asn_summaryRecord::source_connectivity_type()
{
	return property("source_connectivity_type").toString();
}

ken_flow_asn_summaryRecord &ken_flow_asn_summaryRecord::operator=(const ken_flow_asn_summaryRecord &other)
{
	clone(other);
	return *this;
}
