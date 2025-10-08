#include "ken_consultarecord.h"

ken_consultaRecord::ken_consultaRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="ken_consulta";
}

ken_consultaRecord::ken_consultaRecord(const ken_consultaRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="ken_consulta";
	clone(other);
}

ken_consultaRecord::~ken_consultaRecord() {}

void ken_consultaRecord::set_asn(QString asn)
{
	setProperty("asn",asn);
}

void ken_consultaRecord::set_asn_origen(QString asn_origen)
{
	setProperty("asn_origen",asn_origen);
}

void ken_consultaRecord::set_destino_ip(QString destino_ip)
{
	setProperty("destino_ip",destino_ip);
}

void ken_consultaRecord::set_equipo(QString equipo)
{
	setProperty("equipo",equipo);
}

void ken_consultaRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void ken_consultaRecord::set_interface(QString interface)
{
	setProperty("interface",interface);
}

void ken_consultaRecord::set_interface_salida(QString interface_salida)
{
	setProperty("interface_salida",interface_salida);
}

void ken_consultaRecord::set_max_bps(double max_bps)
{
	setProperty("max_bps",max_bps);
}

void ken_consultaRecord::set_p95_bps(double p95_bps)
{
	setProperty("p95_bps",p95_bps);
}

void ken_consultaRecord::set_pe_destino_ip(QString pe_destino_ip)
{
	setProperty("pe_destino_ip",pe_destino_ip);
}

void ken_consultaRecord::set_pe_destino_ip2(QString pe_destino_ip2)
{
	setProperty("pe_destino_ip2",pe_destino_ip2);
}

void ken_consultaRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

QString ken_consultaRecord::asn()
{
	return property("asn").toString();
}

QString ken_consultaRecord::asn_origen()
{
	return property("asn_origen").toString();
}

QString ken_consultaRecord::destino_ip()
{
	return property("destino_ip").toString();
}

QString ken_consultaRecord::equipo()
{
	return property("equipo").toString();
}

QDate ken_consultaRecord::fecha()
{
	return property("fecha").toDate();
}

QString ken_consultaRecord::interface()
{
	return property("interface").toString();
}

QString ken_consultaRecord::interface_salida()
{
	return property("interface_salida").toString();
}

double ken_consultaRecord::max_bps()
{
	return property("max_bps").toDouble();
}

double ken_consultaRecord::p95_bps()
{
	return property("p95_bps").toDouble();
}

QString ken_consultaRecord::pe_destino_ip()
{
	return property("pe_destino_ip").toString();
}

QString ken_consultaRecord::pe_destino_ip2()
{
	return property("pe_destino_ip2").toString();
}

QString ken_consultaRecord::pe_ip()
{
	return property("pe_ip").toString();
}

ken_consultaRecord &ken_consultaRecord::operator=(const ken_consultaRecord &other)
{
	clone(other);
	return *this;
}
