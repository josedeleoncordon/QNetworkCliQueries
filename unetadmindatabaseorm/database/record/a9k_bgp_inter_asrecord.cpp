#include "a9k_bgp_inter_asrecord.h"

a9k_bgp_inter_asRecord::a9k_bgp_inter_asRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_bgp_inter_as";
}

a9k_bgp_inter_asRecord::a9k_bgp_inter_asRecord(const a9k_bgp_inter_asRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_bgp_inter_as";
	clone(other);
}

a9k_bgp_inter_asRecord::~a9k_bgp_inter_asRecord() {}

void a9k_bgp_inter_asRecord::set_asn(QString asn)
{
	setProperty("asn",asn);
}

void a9k_bgp_inter_asRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_bgp_inter_asRecord::set_detalle(QString detalle)
{
	setProperty("detalle",detalle);
}

void a9k_bgp_inter_asRecord::set_device_ip(QString device_ip)
{
	setProperty("device_ip",device_ip);
}

void a9k_bgp_inter_asRecord::set_device_name(QString device_name)
{
	setProperty("device_name",device_name);
}

void a9k_bgp_inter_asRecord::set_neighbor(QString neighbor)
{
	setProperty("neighbor",neighbor);
}

void a9k_bgp_inter_asRecord::set_ok(int ok)
{
	setProperty("ok",ok);
}

void a9k_bgp_inter_asRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

QString a9k_bgp_inter_asRecord::asn()
{
	return property("asn").toString();
}

QDate a9k_bgp_inter_asRecord::date()
{
	return property("date").toDate();
}

QString a9k_bgp_inter_asRecord::detalle()
{
	return property("detalle").toString();
}

QString a9k_bgp_inter_asRecord::device_ip()
{
	return property("device_ip").toString();
}

QString a9k_bgp_inter_asRecord::device_name()
{
	return property("device_name").toString();
}

QString a9k_bgp_inter_asRecord::neighbor()
{
	return property("neighbor").toString();
}

int a9k_bgp_inter_asRecord::ok()
{
	return property("ok").toInt();
}

QString a9k_bgp_inter_asRecord::pais()
{
	return property("pais").toString();
}

a9k_bgp_inter_asRecord &a9k_bgp_inter_asRecord::operator=(const a9k_bgp_inter_asRecord &other)
{
	clone(other);
	return *this;
}
