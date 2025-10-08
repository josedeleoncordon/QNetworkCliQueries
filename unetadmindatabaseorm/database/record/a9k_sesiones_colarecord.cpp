#include "a9k_sesiones_colarecord.h"

a9k_sesiones_colaRecord::a9k_sesiones_colaRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_sesiones_cola";
}

a9k_sesiones_colaRecord::a9k_sesiones_colaRecord(const a9k_sesiones_colaRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_sesiones_cola";
	clone(other);
}

a9k_sesiones_colaRecord::~a9k_sesiones_colaRecord() {}

void a9k_sesiones_colaRecord::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_sesiones_colaRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_sesiones_colaRecord::set_device_ip(QString device_ip)
{
	setProperty("device_ip",device_ip);
}

void a9k_sesiones_colaRecord::set_device_name(QString device_name)
{
	setProperty("device_name",device_name);
}

void a9k_sesiones_colaRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_sesiones_colaRecord::set_inq(int inq)
{
	setProperty("inq",inq);
}

void a9k_sesiones_colaRecord::set_neighbor(QString neighbor)
{
	setProperty("neighbor",neighbor);
}

void a9k_sesiones_colaRecord::set_outq(int outq)
{
	setProperty("outq",outq);
}

void a9k_sesiones_colaRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void a9k_sesiones_colaRecord::set_prefixin(int prefixin)
{
	setProperty("prefixin",prefixin);
}

int a9k_sesiones_colaRecord::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

QDate a9k_sesiones_colaRecord::date()
{
	return property("date").toDate();
}

QString a9k_sesiones_colaRecord::device_ip()
{
	return property("device_ip").toString();
}

QString a9k_sesiones_colaRecord::device_name()
{
	return property("device_name").toString();
}

int a9k_sesiones_colaRecord::id()
{
	return property("id").toInt();
}

int a9k_sesiones_colaRecord::inq()
{
	return property("inq").toInt();
}

QString a9k_sesiones_colaRecord::neighbor()
{
	return property("neighbor").toString();
}

int a9k_sesiones_colaRecord::outq()
{
	return property("outq").toInt();
}

QString a9k_sesiones_colaRecord::pais()
{
	return property("pais").toString();
}

int a9k_sesiones_colaRecord::prefixin()
{
	return property("prefixin").toInt();
}

a9k_equipoRecord *a9k_sesiones_colaRecord::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_sesiones_colaRecord &a9k_sesiones_colaRecord::operator=(const a9k_sesiones_colaRecord &other)
{
	clone(other);
	return *this;
}
