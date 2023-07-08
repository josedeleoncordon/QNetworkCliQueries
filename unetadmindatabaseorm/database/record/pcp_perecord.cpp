#include "pcp_perecord.h"

pcp_peRecord::pcp_peRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="pcp_pe";
}

pcp_peRecord::pcp_peRecord(const pcp_peRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="pcp_pe";
	clone(other);
}

pcp_peRecord::~pcp_peRecord() {}

void pcp_peRecord::set_id(unsigned int id)
{
	setProperty("id",id);
}

void pcp_peRecord::set_ip(QString ip)
{
	setProperty("ip",ip);
}

void pcp_peRecord::set_nombre(QString nombre)
{
	setProperty("nombre",nombre);
}

unsigned int pcp_peRecord::id()
{
	return property("id").toUInt();
}

QString pcp_peRecord::ip()
{
	return property("ip").toString();
}

QString pcp_peRecord::nombre()
{
	return property("nombre").toString();
}

pcp_peRecord &pcp_peRecord::operator=(const pcp_peRecord &other)
{
	clone(other);
	return *this;
}
