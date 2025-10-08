#include "bgp_pe_vpn_peer_comunities_inrecord.h"

bgp_pe_vpn_peer_comunities_inRecord::bgp_pe_vpn_peer_comunities_inRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_pe_vpn_peer_comunities_in";
}

bgp_pe_vpn_peer_comunities_inRecord::bgp_pe_vpn_peer_comunities_inRecord(const bgp_pe_vpn_peer_comunities_inRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_pe_vpn_peer_comunities_in";
	clone(other);
}

bgp_pe_vpn_peer_comunities_inRecord::~bgp_pe_vpn_peer_comunities_inRecord() {}

void bgp_pe_vpn_peer_comunities_inRecord::set_additional_paths_enabled(bool additional_paths_enabled)
{
	setProperty("additional_paths_enabled",additional_paths_enabled);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_comunidades_faltantes(QString comunidades_faltantes)
{
	setProperty("comunidades_faltantes",comunidades_faltantes);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_comunidades_pri(QString comunidades_pri)
{
	setProperty("comunidades_pri",comunidades_pri);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_comunidades_sec(QString comunidades_sec)
{
	setProperty("comunidades_sec",comunidades_sec);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_comunidades_ter(QString comunidades_ter)
{
	setProperty("comunidades_ter",comunidades_ter);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_family(QString family)
{
	setProperty("family",family);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_peer(QString peer)
{
	setProperty("peer",peer);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_prefix_in(int prefix_in)
{
	setProperty("prefix_in",prefix_in);
}

void bgp_pe_vpn_peer_comunities_inRecord::set_rpl_in(QString rpl_in)
{
	setProperty("rpl_in",rpl_in);
}

bool bgp_pe_vpn_peer_comunities_inRecord::additional_paths_enabled()
{
	return property("additional_paths_enabled").toBool();
}

QString bgp_pe_vpn_peer_comunities_inRecord::comunidades_faltantes()
{
	return property("comunidades_faltantes").toString();
}

QString bgp_pe_vpn_peer_comunities_inRecord::comunidades_pri()
{
	return property("comunidades_pri").toString();
}

QString bgp_pe_vpn_peer_comunities_inRecord::comunidades_sec()
{
	return property("comunidades_sec").toString();
}

QString bgp_pe_vpn_peer_comunities_inRecord::comunidades_ter()
{
	return property("comunidades_ter").toString();
}

QString bgp_pe_vpn_peer_comunities_inRecord::family()
{
	return property("family").toString();
}

QDate bgp_pe_vpn_peer_comunities_inRecord::fecha()
{
	return property("fecha").toDate();
}

QString bgp_pe_vpn_peer_comunities_inRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString bgp_pe_vpn_peer_comunities_inRecord::peer()
{
	return property("peer").toString();
}

int bgp_pe_vpn_peer_comunities_inRecord::prefix_in()
{
	return property("prefix_in").toInt();
}

QString bgp_pe_vpn_peer_comunities_inRecord::rpl_in()
{
	return property("rpl_in").toString();
}

bgp_pe_vpn_peer_comunities_inRecord &bgp_pe_vpn_peer_comunities_inRecord::operator=(const bgp_pe_vpn_peer_comunities_inRecord &other)
{
	clone(other);
	return *this;
}
