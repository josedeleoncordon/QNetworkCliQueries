#include "proveedorrecord.h"

proveedorRecord::proveedorRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="proveedor";
}

proveedorRecord::proveedorRecord(const proveedorRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="proveedor";
	clone(other);
}

proveedorRecord::~proveedorRecord() {}

void proveedorRecord::set_id(QString id)
{
	setProperty("id",id);
}

void proveedorRecord::set_proveedor(QString proveedor)
{
	setProperty("proveedor",proveedor);
}

QString proveedorRecord::id()
{
	return property("id").toString();
}

QString proveedorRecord::proveedor()
{
	return property("proveedor").toString();
}

proveedorRecord &proveedorRecord::operator=(const proveedorRecord &other)
{
	clone(other);
	return *this;
}
