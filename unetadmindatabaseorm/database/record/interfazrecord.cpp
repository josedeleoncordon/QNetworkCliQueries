#include "interfazrecord.h"

interfazRecord::interfazRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="interfaz";
}

interfazRecord::interfazRecord(const interfazRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="interfaz";
	clone(other);
}

interfazRecord::~interfazRecord() {}

void interfazRecord::set_bandwidth(long long bandwidth)
{
	setProperty("bandwidth",bandwidth);
}

void interfazRecord::set_descripcion(QString descripcion)
{
	setProperty("descripcion",descripcion);
}

void interfazRecord::set_equipo_id(unsigned int equipo_id)
{
	setProperty("equipo_id",equipo_id);
}

void interfazRecord::set_estado(bool estado)
{
	setProperty("estado",estado);
}

void interfazRecord::set_id(unsigned int id)
{
	setProperty("id",id);
}

void interfazRecord::set_interfaz(QString interfaz)
{
	setProperty("interfaz",interfaz);
}

void interfazRecord::set_interfaz_tipo_id(int interfaz_tipo_id)
{
	setProperty("interfaz_tipo_id",interfaz_tipo_id);
}

void interfazRecord::set_ospfcosto(int ospfcosto)
{
	setProperty("ospfcosto",ospfcosto);
}

void interfazRecord::set_proveedor_id(QString proveedor_id)
{
	setProperty("proveedor_id",proveedor_id);
}

long long interfazRecord::bandwidth()
{
	return property("bandwidth").toLongLong();
}

QString interfazRecord::descripcion()
{
	return property("descripcion").toString();
}

unsigned int interfazRecord::equipo_id()
{
	return property("equipo_id").toUInt();
}

bool interfazRecord::estado()
{
	return property("estado").toBool();
}

unsigned int interfazRecord::id()
{
	return property("id").toUInt();
}

QString interfazRecord::interfaz()
{
	return property("interfaz").toString();
}

int interfazRecord::interfaz_tipo_id()
{
	return property("interfaz_tipo_id").toInt();
}

int interfazRecord::ospfcosto()
{
	return property("ospfcosto").toInt();
}

QString interfazRecord::proveedor_id()
{
	return property("proveedor_id").toString();
}

equipoRecord *interfazRecord::r_equipo_id()
{
	return dynamic_cast<equipoRecord*>( property("r_equipo_id").value<RecordBase*>() );
}

interfaz_tipoRecord *interfazRecord::r_interfaz_tipo_id()
{
	return dynamic_cast<interfaz_tipoRecord*>( property("r_interfaz_tipo_id").value<RecordBase*>() );
}

proveedorRecord *interfazRecord::r_proveedor_id()
{
	return dynamic_cast<proveedorRecord*>( property("r_proveedor_id").value<RecordBase*>() );
}

interfazRecord &interfazRecord::operator=(const interfazRecord &other)
{
	clone(other);
	return *this;
}
