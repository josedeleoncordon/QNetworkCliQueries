#include "pcp_regionrecord.h"

pcp_regionRecord::pcp_regionRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="pcp_region";
}

pcp_regionRecord::pcp_regionRecord(const pcp_regionRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="pcp_region";
	clone(other);
}

pcp_regionRecord::~pcp_regionRecord() {}

void pcp_regionRecord::set_id(int id)
{
	setProperty("id",id);
}

void pcp_regionRecord::set_region(QString region)
{
	setProperty("region",region);
}

int pcp_regionRecord::id()
{
	return property("id").toInt();
}

QString pcp_regionRecord::region()
{
	return property("region").toString();
}

pcp_regionRecord &pcp_regionRecord::operator=(const pcp_regionRecord &other)
{
	clone(other);
	return *this;
}
