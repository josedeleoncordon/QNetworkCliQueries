#include "grupo_equiporecord.h"

grupo_equipoRecord::grupo_equipoRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="grupo_equipo";
}

grupo_equipoRecord::grupo_equipoRecord(const grupo_equipoRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="grupo_equipo";
	clone(other);
}

grupo_equipoRecord::~grupo_equipoRecord() {}

void grupo_equipoRecord::set_equipo_id(unsigned int equipo_id)
{
	setProperty("equipo_id",equipo_id);
}

void grupo_equipoRecord::set_grupo_id(int grupo_id)
{
	setProperty("grupo_id",grupo_id);
}

void grupo_equipoRecord::set_id(int id)
{
	setProperty("id",id);
}

void grupo_equipoRecord::set_posicion_x(double posicion_x)
{
	setProperty("posicion_x",posicion_x);
}

void grupo_equipoRecord::set_posicion_y(double posicion_y)
{
	setProperty("posicion_y",posicion_y);
}

unsigned int grupo_equipoRecord::equipo_id()
{
	return property("equipo_id").toUInt();
}

int grupo_equipoRecord::grupo_id()
{
	return property("grupo_id").toInt();
}

int grupo_equipoRecord::id()
{
	return property("id").toInt();
}

double grupo_equipoRecord::posicion_x()
{
	return property("posicion_x").toDouble();
}

double grupo_equipoRecord::posicion_y()
{
	return property("posicion_y").toDouble();
}

equipoRecord *grupo_equipoRecord::r_equipo_id()
{
	return dynamic_cast<equipoRecord*>( property("r_equipo_id").value<RecordBase*>() );
}

grupoRecord *grupo_equipoRecord::r_grupo_id()
{
	return dynamic_cast<grupoRecord*>( property("r_grupo_id").value<RecordBase*>() );
}

grupo_equipoRecord &grupo_equipoRecord::operator=(const grupo_equipoRecord &other)
{
	clone(other);
	return *this;
}
