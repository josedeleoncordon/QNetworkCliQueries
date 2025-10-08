#include "a9k_internet_redistribucion_estaticas_conectadasrecord.h"

a9k_internet_redistribucion_estaticas_conectadasRecord::a9k_internet_redistribucion_estaticas_conectadasRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_internet_redistribucion_estaticas_conectadas";
}

a9k_internet_redistribucion_estaticas_conectadasRecord::a9k_internet_redistribucion_estaticas_conectadasRecord(const a9k_internet_redistribucion_estaticas_conectadasRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_internet_redistribucion_estaticas_conectadas";
	clone(other);
}

a9k_internet_redistribucion_estaticas_conectadasRecord::~a9k_internet_redistribucion_estaticas_conectadasRecord() {}

void a9k_internet_redistribucion_estaticas_conectadasRecord::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_internet_redistribucion_estaticas_conectadasRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_internet_redistribucion_estaticas_conectadasRecord::set_descripcion(QString descripcion)
{
	setProperty("descripcion",descripcion);
}

void a9k_internet_redistribucion_estaticas_conectadasRecord::set_device_ip(QString device_ip)
{
	setProperty("device_ip",device_ip);
}

void a9k_internet_redistribucion_estaticas_conectadasRecord::set_device_name(QString device_name)
{
	setProperty("device_name",device_name);
}

void a9k_internet_redistribucion_estaticas_conectadasRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_internet_redistribucion_estaticas_conectadasRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void a9k_internet_redistribucion_estaticas_conectadasRecord::set_routepolicy(QString routepolicy)
{
	setProperty("routepolicy",routepolicy);
}

void a9k_internet_redistribucion_estaticas_conectadasRecord::set_tipo(QString tipo)
{
	setProperty("tipo",tipo);
}

int a9k_internet_redistribucion_estaticas_conectadasRecord::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

QDate a9k_internet_redistribucion_estaticas_conectadasRecord::date()
{
	return property("date").toDate();
}

QString a9k_internet_redistribucion_estaticas_conectadasRecord::descripcion()
{
	return property("descripcion").toString();
}

QString a9k_internet_redistribucion_estaticas_conectadasRecord::device_ip()
{
	return property("device_ip").toString();
}

QString a9k_internet_redistribucion_estaticas_conectadasRecord::device_name()
{
	return property("device_name").toString();
}

int a9k_internet_redistribucion_estaticas_conectadasRecord::id()
{
	return property("id").toInt();
}

QString a9k_internet_redistribucion_estaticas_conectadasRecord::pais()
{
	return property("pais").toString();
}

QString a9k_internet_redistribucion_estaticas_conectadasRecord::routepolicy()
{
	return property("routepolicy").toString();
}

QString a9k_internet_redistribucion_estaticas_conectadasRecord::tipo()
{
	return property("tipo").toString();
}

a9k_equipoRecord *a9k_internet_redistribucion_estaticas_conectadasRecord::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_internet_redistribucion_estaticas_conectadasRecord &a9k_internet_redistribucion_estaticas_conectadasRecord::operator=(const a9k_internet_redistribucion_estaticas_conectadasRecord &other)
{
	clone(other);
	return *this;
}
