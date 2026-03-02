#include "a9k_bgp_sesiones_weightrecord.h"

a9k_bgp_sesiones_weightRecord::a9k_bgp_sesiones_weightRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_bgp_sesiones_weight";
}

a9k_bgp_sesiones_weightRecord::a9k_bgp_sesiones_weightRecord(const a9k_bgp_sesiones_weightRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_bgp_sesiones_weight";
	clone(other);
}

a9k_bgp_sesiones_weightRecord::~a9k_bgp_sesiones_weightRecord() {}

void a9k_bgp_sesiones_weightRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_bgp_sesiones_weightRecord::set_detalle(QString detalle)
{
	setProperty("detalle",detalle);
}

void a9k_bgp_sesiones_weightRecord::set_device_ip(QString device_ip)
{
	setProperty("device_ip",device_ip);
}

void a9k_bgp_sesiones_weightRecord::set_device_name(QString device_name)
{
	setProperty("device_name",device_name);
}

void a9k_bgp_sesiones_weightRecord::set_neighbor(QString neighbor)
{
	setProperty("neighbor",neighbor);
}

void a9k_bgp_sesiones_weightRecord::set_ok(int ok)
{
	setProperty("ok",ok);
}

void a9k_bgp_sesiones_weightRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void a9k_bgp_sesiones_weightRecord::set_routepolicy(QString routepolicy)
{
	setProperty("routepolicy",routepolicy);
}

void a9k_bgp_sesiones_weightRecord::set_vrf(QString vrf)
{
	setProperty("vrf",vrf);
}

QDate a9k_bgp_sesiones_weightRecord::date()
{
	return property("date").toDate();
}

QString a9k_bgp_sesiones_weightRecord::detalle()
{
	return property("detalle").toString();
}

QString a9k_bgp_sesiones_weightRecord::device_ip()
{
	return property("device_ip").toString();
}

QString a9k_bgp_sesiones_weightRecord::device_name()
{
	return property("device_name").toString();
}

QString a9k_bgp_sesiones_weightRecord::neighbor()
{
	return property("neighbor").toString();
}

int a9k_bgp_sesiones_weightRecord::ok()
{
	return property("ok").toInt();
}

QString a9k_bgp_sesiones_weightRecord::pais()
{
	return property("pais").toString();
}

QString a9k_bgp_sesiones_weightRecord::routepolicy()
{
	return property("routepolicy").toString();
}

QString a9k_bgp_sesiones_weightRecord::vrf()
{
	return property("vrf").toString();
}

a9k_bgp_sesiones_weightRecord &a9k_bgp_sesiones_weightRecord::operator=(const a9k_bgp_sesiones_weightRecord &other)
{
	clone(other);
	return *this;
}
