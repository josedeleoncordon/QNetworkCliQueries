#include "bgp_prefix_in_refrecord.h"

bgp_prefix_in_refRecord::bgp_prefix_in_refRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_prefix_in_ref";
}

bgp_prefix_in_refRecord::bgp_prefix_in_refRecord(const bgp_prefix_in_refRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_prefix_in_ref";
	clone(other);
}

bgp_prefix_in_refRecord::~bgp_prefix_in_refRecord() {}

void bgp_prefix_in_refRecord::set_address_family(QString address_family)
{
	setProperty("address_family",address_family);
}

void bgp_prefix_in_refRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void bgp_prefix_in_refRecord::set_peer_ip(QString peer_ip)
{
	setProperty("peer_ip",peer_ip);
}

void bgp_prefix_in_refRecord::set_pfxincount(int pfxincount)
{
	setProperty("pfxincount",pfxincount);
}

QString bgp_prefix_in_refRecord::address_family()
{
	return property("address_family").toString();
}

QString bgp_prefix_in_refRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString bgp_prefix_in_refRecord::peer_ip()
{
	return property("peer_ip").toString();
}

int bgp_prefix_in_refRecord::pfxincount()
{
	return property("pfxincount").toInt();
}

bgp_prefix_in_refRecord &bgp_prefix_in_refRecord::operator=(const bgp_prefix_in_refRecord &other)
{
	clone(other);
	return *this;
}
