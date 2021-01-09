#include "pcp_peer_tiporecord.h"

pcp_peer_tipoRecord::pcp_peer_tipoRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="pcp_peer_tipo";
}

pcp_peer_tipoRecord::pcp_peer_tipoRecord(const pcp_peer_tipoRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="pcp_peer_tipo";
	clone(other);
}

pcp_peer_tipoRecord::~pcp_peer_tipoRecord() {}

void pcp_peer_tipoRecord::set_id(int id)
{
	setProperty("id",id);
}

void pcp_peer_tipoRecord::set_tipo(QString tipo)
{
	setProperty("tipo",tipo);
}

int pcp_peer_tipoRecord::id()
{
	return property("id").toInt();
}

QString pcp_peer_tipoRecord::tipo()
{
	return property("tipo").toString();
}

pcp_peer_tipoRecord &pcp_peer_tipoRecord::operator=(const pcp_peer_tipoRecord &other)
{
	clone(other);
	return *this;
}
