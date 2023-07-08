#include "interfaz_tiporecord.h"

interfaz_tipoRecord::interfaz_tipoRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="interfaz_tipo";
}

interfaz_tipoRecord::interfaz_tipoRecord(const interfaz_tipoRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="interfaz_tipo";
	clone(other);
}

interfaz_tipoRecord::~interfaz_tipoRecord() {}

void interfaz_tipoRecord::set_id(int id)
{
	setProperty("id",id);
}

void interfaz_tipoRecord::set_tipo(QString tipo)
{
	setProperty("tipo",tipo);
}

int interfaz_tipoRecord::id()
{
	return property("id").toInt();
}

QString interfaz_tipoRecord::tipo()
{
	return property("tipo").toString();
}

interfaz_tipoRecord &interfaz_tipoRecord::operator=(const interfaz_tipoRecord &other)
{
	clone(other);
	return *this;
}
