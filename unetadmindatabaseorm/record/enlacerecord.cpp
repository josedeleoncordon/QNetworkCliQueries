#include "enlacerecord.h"

enlaceRecord::enlaceRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="enlace";
}

enlaceRecord::enlaceRecord(const enlaceRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="enlace";
	clone(other);
}

enlaceRecord::~enlaceRecord() {}

void enlaceRecord::set_destino_interfaz_id(unsigned int destino_interfaz_id)
{
	setProperty("destino_interfaz_id",destino_interfaz_id);
}

void enlaceRecord::set_estado(bool estado)
{
	setProperty("estado",estado);
}

void enlaceRecord::set_etiqueta(QString etiqueta)
{
	setProperty("etiqueta",etiqueta);
}

void enlaceRecord::set_id(unsigned long long id)
{
	setProperty("id",id);
}

void enlaceRecord::set_origen_interfaz_id(unsigned int origen_interfaz_id)
{
	setProperty("origen_interfaz_id",origen_interfaz_id);
}

unsigned int enlaceRecord::destino_interfaz_id()
{
	return property("destino_interfaz_id").toUInt();
}

bool enlaceRecord::estado()
{
	return property("estado").toBool();
}

QString enlaceRecord::etiqueta()
{
	return property("etiqueta").toString();
}

unsigned long long enlaceRecord::id()
{
	return property("id").toULongLong();
}

unsigned int enlaceRecord::origen_interfaz_id()
{
	return property("origen_interfaz_id").toUInt();
}

interfazRecord *enlaceRecord::r_destino_interfaz_id()
{
	return dynamic_cast<interfazRecord*>( property("r_destino_interfaz_id").value<RecordBase*>() );
}

interfazRecord *enlaceRecord::r_origen_interfaz_id()
{
	return dynamic_cast<interfazRecord*>( property("r_origen_interfaz_id").value<RecordBase*>() );
}

enlaceRecord &enlaceRecord::operator=(const enlaceRecord &other)
{
	clone(other);
	return *this;
}
