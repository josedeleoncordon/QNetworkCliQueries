#include "interfaz_estadorecord.h"

interfaz_estadoRecord::interfaz_estadoRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="interfaz_estado";
}

interfaz_estadoRecord::interfaz_estadoRecord(const interfaz_estadoRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="interfaz_estado";
	clone(other);
}

interfaz_estadoRecord::~interfaz_estadoRecord() {}

void interfaz_estadoRecord::set_estado(bool estado)
{
	setProperty("estado",estado);
}

void interfaz_estadoRecord::set_fecha_hora(QDateTime fecha_hora)
{
	setProperty("fecha_hora",fecha_hora);
}

void interfaz_estadoRecord::set_id(unsigned long long id)
{
	setProperty("id",id);
}

void interfaz_estadoRecord::set_interfaz_id(unsigned int interfaz_id)
{
	setProperty("interfaz_id",interfaz_id);
}

void interfaz_estadoRecord::set_ratein(long long ratein)
{
	setProperty("ratein",ratein);
}

void interfaz_estadoRecord::set_rateout(long long rateout)
{
	setProperty("rateout",rateout);
}

bool interfaz_estadoRecord::estado()
{
	return property("estado").toBool();
}

QDateTime interfaz_estadoRecord::fecha_hora()
{
	return property("fecha_hora").toDateTime();
}

unsigned long long interfaz_estadoRecord::id()
{
	return property("id").toULongLong();
}

unsigned int interfaz_estadoRecord::interfaz_id()
{
	return property("interfaz_id").toUInt();
}

long long interfaz_estadoRecord::ratein()
{
	return property("ratein").toLongLong();
}

long long interfaz_estadoRecord::rateout()
{
	return property("rateout").toLongLong();
}

interfazRecord *interfaz_estadoRecord::r_interfaz_id()
{
	return dynamic_cast<interfazRecord*>( property("r_interfaz_id").value<RecordBase*>() );
}

interfaz_estadoRecord &interfaz_estadoRecord::operator=(const interfaz_estadoRecord &other)
{
	clone(other);
	return *this;
}
