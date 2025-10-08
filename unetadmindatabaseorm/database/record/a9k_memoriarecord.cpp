#include "a9k_memoriarecord.h"

a9k_memoriaRecord::a9k_memoriaRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_memoria";
}

a9k_memoriaRecord::a9k_memoriaRecord(const a9k_memoriaRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_memoria";
	clone(other);
}

a9k_memoriaRecord::~a9k_memoriaRecord() {}

void a9k_memoriaRecord::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_memoriaRecord::set_app_disponible(int app_disponible)
{
	setProperty("app_disponible",app_disponible);
}

void a9k_memoriaRecord::set_bgp(int bgp)
{
	setProperty("bgp",bgp);
}

void a9k_memoriaRecord::set_bgp_limit(int bgp_limit)
{
	setProperty("bgp_limit",bgp_limit);
}

void a9k_memoriaRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_memoriaRecord::set_device_ip(QString device_ip)
{
	setProperty("device_ip",device_ip);
}

void a9k_memoriaRecord::set_device_name(QString device_name)
{
	setProperty("device_name",device_name);
}

void a9k_memoriaRecord::set_fisica_disponible(int fisica_disponible)
{
	setProperty("fisica_disponible",fisica_disponible);
}

void a9k_memoriaRecord::set_fisica_total(int fisica_total)
{
	setProperty("fisica_total",fisica_total);
}

void a9k_memoriaRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_memoriaRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

int a9k_memoriaRecord::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

int a9k_memoriaRecord::app_disponible()
{
	return property("app_disponible").toInt();
}

int a9k_memoriaRecord::bgp()
{
	return property("bgp").toInt();
}

int a9k_memoriaRecord::bgp_limit()
{
	return property("bgp_limit").toInt();
}

QDate a9k_memoriaRecord::date()
{
	return property("date").toDate();
}

QString a9k_memoriaRecord::device_ip()
{
	return property("device_ip").toString();
}

QString a9k_memoriaRecord::device_name()
{
	return property("device_name").toString();
}

int a9k_memoriaRecord::fisica_disponible()
{
	return property("fisica_disponible").toInt();
}

int a9k_memoriaRecord::fisica_total()
{
	return property("fisica_total").toInt();
}

int a9k_memoriaRecord::id()
{
	return property("id").toInt();
}

QString a9k_memoriaRecord::pais()
{
	return property("pais").toString();
}

a9k_equipoRecord *a9k_memoriaRecord::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_memoriaRecord &a9k_memoriaRecord::operator=(const a9k_memoriaRecord &other)
{
	clone(other);
	return *this;
}
