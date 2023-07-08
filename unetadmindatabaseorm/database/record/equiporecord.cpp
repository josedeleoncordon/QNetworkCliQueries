#include "equiporecord.h"

equipoRecord::equipoRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="equipo";
}

equipoRecord::equipoRecord(const equipoRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="equipo";
	clone(other);
}

equipoRecord::~equipoRecord() {}

void equipoRecord::set_area_id(int area_id)
{
	setProperty("area_id",area_id);
}

void equipoRecord::set_estado(bool estado)
{
	setProperty("estado",estado);
}

void equipoRecord::set_id(unsigned int id)
{
	setProperty("id",id);
}

void equipoRecord::set_ip(QString ip)
{
	setProperty("ip",ip);
}

void equipoRecord::set_nombre(QString nombre)
{
	setProperty("nombre",nombre);
}

int equipoRecord::area_id()
{
	return property("area_id").toInt();
}

bool equipoRecord::estado()
{
	return property("estado").toBool();
}

unsigned int equipoRecord::id()
{
	return property("id").toUInt();
}

QString equipoRecord::ip()
{
	return property("ip").toString();
}

QString equipoRecord::nombre()
{
	return property("nombre").toString();
}

areaRecord *equipoRecord::r_area_id()
{
	return dynamic_cast<areaRecord*>( property("r_area_id").value<RecordBase*>() );
}

equipoRecord &equipoRecord::operator=(const equipoRecord &other)
{
	clone(other);
	return *this;
}
