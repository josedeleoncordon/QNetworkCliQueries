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

int pcp_paisRecord::id()
{
	return property("id").toInt();
}

QString pcp_paisRecord::pais()
{
	return property("pais").toString();
}

pcp_paisRecord &pcp_paisRecord::operator=(const pcp_paisRecord &other)
{
	clone(other);
	return *this;
}
