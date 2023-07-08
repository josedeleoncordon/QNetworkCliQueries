#include "pcp_paisrecord.h"

pcp_paisRecord::pcp_paisRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="pcp_pais";
}

pcp_paisRecord::pcp_paisRecord(const pcp_paisRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="pcp_pais";
	clone(other);
}

pcp_paisRecord::~pcp_paisRecord() {}

void pcp_paisRecord::set_id(int id)
{
	setProperty("id",id);
}

void pcp_paisRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void pcp_paisRecord::set_pcp_region_id(int pcp_region_id)
{
	setProperty("pcp_region_id",pcp_region_id);
}

int pcp_paisRecord::id()
{
	return property("id").toInt();
}

QString pcp_paisRecord::pais()
{
	return property("pais").toString();
}

int pcp_paisRecord::pcp_region_id()
{
	return property("pcp_region_id").toInt();
}

pcp_regionRecord *pcp_paisRecord::r_pcp_region_id()
{
	return dynamic_cast<pcp_regionRecord*>( property("r_pcp_region_id").value<RecordBase*>() );
}

pcp_paisRecord &pcp_paisRecord::operator=(const pcp_paisRecord &other)
{
	clone(other);
	return *this;
}
