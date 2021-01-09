#include "pcp_proveedorrecord.h"

pcp_proveedorRecord::pcp_proveedorRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="pcp_proveedor";
}

pcp_proveedorRecord::pcp_proveedorRecord(const pcp_proveedorRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="pcp_proveedor";
	clone(other);
}

pcp_proveedorRecord::~pcp_proveedorRecord() {}

void pcp_proveedorRecord::set_id(unsigned int id)
{
	setProperty("id",id);
}

void pcp_proveedorRecord::set_logo_path(QString logo_path)
{
	setProperty("logo_path",logo_path);
}

void pcp_proveedorRecord::set_proveedor(QString proveedor)
{
	setProperty("proveedor",proveedor);
}

unsigned int pcp_proveedorRecord::id()
{
	return property("id").toUInt();
}

QString pcp_proveedorRecord::logo_path()
{
	return property("logo_path").toString();
}

QString pcp_proveedorRecord::proveedor()
{
	return property("proveedor").toString();
}

pcp_proveedorRecord &pcp_proveedorRecord::operator=(const pcp_proveedorRecord &other)
{
	clone(other);
	return *this;
}
