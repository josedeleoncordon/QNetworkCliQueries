#include "a9k_peer_caida_prefix_inrecord.h"

a9k_peer_caida_prefix_inRecord::a9k_peer_caida_prefix_inRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_peer_caida_prefix_in";
}

a9k_peer_caida_prefix_inRecord::a9k_peer_caida_prefix_inRecord(const a9k_peer_caida_prefix_inRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_peer_caida_prefix_in";
	clone(other);
}

a9k_peer_caida_prefix_inRecord::~a9k_peer_caida_prefix_inRecord() {}

void a9k_peer_caida_prefix_inRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_peer_caida_prefix_inRecord::set_detalle(QString detalle)
{
	setProperty("detalle",detalle);
}

void a9k_peer_caida_prefix_inRecord::set_device_ip(QString device_ip)
{
	setProperty("device_ip",device_ip);
}

void a9k_peer_caida_prefix_inRecord::set_device_name(QString device_name)
{
	setProperty("device_name",device_name);
}

void a9k_peer_caida_prefix_inRecord::set_neighbor(QString neighbor)
{
	setProperty("neighbor",neighbor);
}

void a9k_peer_caida_prefix_inRecord::set_ok(int ok)
{
	setProperty("ok",ok);
}

void a9k_peer_caida_prefix_inRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void a9k_peer_caida_prefix_inRecord::set_prefixin(int prefixin)
{
	setProperty("prefixin",prefixin);
}

void a9k_peer_caida_prefix_inRecord::set_proveedor(QString proveedor)
{
	setProperty("proveedor",proveedor);
}

QDate a9k_peer_caida_prefix_inRecord::date()
{
	return property("date").toDate();
}

QString a9k_peer_caida_prefix_inRecord::detalle()
{
	return property("detalle").toString();
}

QString a9k_peer_caida_prefix_inRecord::device_ip()
{
	return property("device_ip").toString();
}

QString a9k_peer_caida_prefix_inRecord::device_name()
{
	return property("device_name").toString();
}

QString a9k_peer_caida_prefix_inRecord::neighbor()
{
	return property("neighbor").toString();
}

int a9k_peer_caida_prefix_inRecord::ok()
{
	return property("ok").toInt();
}

QString a9k_peer_caida_prefix_inRecord::pais()
{
	return property("pais").toString();
}

int a9k_peer_caida_prefix_inRecord::prefixin()
{
	return property("prefixin").toInt();
}

QString a9k_peer_caida_prefix_inRecord::proveedor()
{
	return property("proveedor").toString();
}

a9k_peer_caida_prefix_inRecord &a9k_peer_caida_prefix_inRecord::operator=(const a9k_peer_caida_prefix_inRecord &other)
{
	clone(other);
	return *this;
}
