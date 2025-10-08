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

void a9k_peer_caida_prefix_inRecord::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_peer_caida_prefix_inRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_peer_caida_prefix_inRecord::set_device_ip(QString device_ip)
{
	setProperty("device_ip",device_ip);
}

void a9k_peer_caida_prefix_inRecord::set_device_name(QString device_name)
{
	setProperty("device_name",device_name);
}

void a9k_peer_caida_prefix_inRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_peer_caida_prefix_inRecord::set_neighbor(QString neighbor)
{
	setProperty("neighbor",neighbor);
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

int a9k_peer_caida_prefix_inRecord::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

QDate a9k_peer_caida_prefix_inRecord::date()
{
	return property("date").toDate();
}

QString a9k_peer_caida_prefix_inRecord::device_ip()
{
	return property("device_ip").toString();
}

QString a9k_peer_caida_prefix_inRecord::device_name()
{
	return property("device_name").toString();
}

int a9k_peer_caida_prefix_inRecord::id()
{
	return property("id").toInt();
}

QString a9k_peer_caida_prefix_inRecord::neighbor()
{
	return property("neighbor").toString();
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

a9k_equipoRecord *a9k_peer_caida_prefix_inRecord::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_peer_caida_prefix_inRecord &a9k_peer_caida_prefix_inRecord::operator=(const a9k_peer_caida_prefix_inRecord &other)
{
	clone(other);
	return *this;
}
