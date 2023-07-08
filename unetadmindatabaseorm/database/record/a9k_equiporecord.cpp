#include "a9k_equiporecord.h"

a9k_equipoRecord::a9k_equipoRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_equipo";
}

a9k_equipoRecord::a9k_equipoRecord(const a9k_equipoRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_equipo";
	clone(other);
}

a9k_equipoRecord::~a9k_equipoRecord() {}

void a9k_equipoRecord::set_IP(QString IP)
{
	setProperty("IP",IP);
}

void a9k_equipoRecord::set_a9k_pais_id(int a9k_pais_id)
{
	setProperty("a9k_pais_id",a9k_pais_id);
}

void a9k_equipoRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_equipoRecord::set_nombre(QString nombre)
{
	setProperty("nombre",nombre);
}

QString a9k_equipoRecord::IP()
{
	return property("IP").toString();
}

int a9k_equipoRecord::a9k_pais_id()
{
	return property("a9k_pais_id").toInt();
}

int a9k_equipoRecord::id()
{
	return property("id").toInt();
}

QString a9k_equipoRecord::nombre()
{
	return property("nombre").toString();
}

a9k_paisRecord *a9k_equipoRecord::r_a9k_pais_id()
{
	return dynamic_cast<a9k_paisRecord*>( property("r_a9k_pais_id").value<RecordBase*>() );
}

a9k_equipoRecord &a9k_equipoRecord::operator=(const a9k_equipoRecord &other)
{
	clone(other);
	return *this;
}
