#include "pcp_pe_peerrecord.h"

pcp_pe_peerRecord::pcp_pe_peerRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="pcp_pe_peer";
}

pcp_pe_peerRecord::pcp_pe_peerRecord(const pcp_pe_peerRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="pcp_pe_peer";
	clone(other);
}

pcp_pe_peerRecord::~pcp_pe_peerRecord() {}

void pcp_pe_peerRecord::set_id(unsigned int id)
{
	setProperty("id",id);
}

void pcp_pe_peerRecord::set_pcp_pe_id(unsigned int pcp_pe_id)
{
	setProperty("pcp_pe_id",pcp_pe_id);
}

void pcp_pe_peerRecord::set_pcp_peer_id(unsigned int pcp_peer_id)
{
	setProperty("pcp_peer_id",pcp_peer_id);
}

void pcp_pe_peerRecord::set_prepend(int prepend)
{
	setProperty("prepend",prepend);
}

unsigned int pcp_pe_peerRecord::id()
{
	return property("id").toUInt();
}

unsigned int pcp_pe_peerRecord::pcp_pe_id()
{
	return property("pcp_pe_id").toUInt();
}

unsigned int pcp_pe_peerRecord::pcp_peer_id()
{
	return property("pcp_peer_id").toUInt();
}

int pcp_pe_peerRecord::prepend()
{
	return property("prepend").toInt();
}

pcp_peRecord *pcp_pe_peerRecord::r_pcp_pe_id()
{
	return dynamic_cast<pcp_peRecord*>( property("r_pcp_pe_id").value<RecordBase*>() );
}

pcp_peerRecord *pcp_pe_peerRecord::r_pcp_peer_id()
{
	return dynamic_cast<pcp_peerRecord*>( property("r_pcp_peer_id").value<RecordBase*>() );
}

pcp_pe_peerRecord &pcp_pe_peerRecord::operator=(const pcp_pe_peerRecord &other)
{
	clone(other);
	return *this;
}
