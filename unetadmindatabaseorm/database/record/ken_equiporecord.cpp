#include "ken_equiporecord.h"

ken_equipoRecord::ken_equipoRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="ken_equipo";
}

ken_equipoRecord::ken_equipoRecord(const ken_equipoRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="ken_equipo";
	clone(other);
}

ken_equipoRecord::~ken_equipoRecord() {}

void ken_equipoRecord::set_id(int id)
{
	setProperty("id",id);
}

void ken_equipoRecord::set_ip(QString ip)
{
	setProperty("ip",ip);
}

void ken_equipoRecord::set_nombre(QString nombre)
{
	setProperty("nombre",nombre);
}

int ken_equipoRecord::id()
{
	return property("id").toInt();
}

QString ken_equipoRecord::ip()
{
	return property("ip").toString();
}

QString ken_equipoRecord::nombre()
{
	return property("nombre").toString();
}

ken_equipoRecord &ken_equipoRecord::operator=(const ken_equipoRecord &other)
{
	clone(other);
	return *this;
}
