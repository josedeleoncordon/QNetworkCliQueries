#include "bgp_pe_ipt_default_trackrecord.h"

bgp_pe_ipt_default_trackRecord::bgp_pe_ipt_default_trackRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_pe_ipt_default_track";
}

bgp_pe_ipt_default_trackRecord::bgp_pe_ipt_default_trackRecord(const bgp_pe_ipt_default_trackRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_pe_ipt_default_track";
	clone(other);
}

bgp_pe_ipt_default_trackRecord::~bgp_pe_ipt_default_trackRecord() {}

void bgp_pe_ipt_default_trackRecord::set_default_hacia_peer(int default_hacia_peer)
{
	setProperty("default_hacia_peer",default_hacia_peer);
}

void bgp_pe_ipt_default_trackRecord::set_detalle(QString detalle)
{
	setProperty("detalle",detalle);
}

void bgp_pe_ipt_default_trackRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void bgp_pe_ipt_default_trackRecord::set_ok(int ok)
{
	setProperty("ok",ok);
}

void bgp_pe_ipt_default_trackRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void bgp_pe_ipt_default_trackRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void bgp_pe_ipt_default_trackRecord::set_pe_nombre(QString pe_nombre)
{
	setProperty("pe_nombre",pe_nombre);
}

void bgp_pe_ipt_default_trackRecord::set_peer(QString peer)
{
	setProperty("peer",peer);
}

void bgp_pe_ipt_default_trackRecord::set_track(QString track)
{
	setProperty("track",track);
}

void bgp_pe_ipt_default_trackRecord::set_track_ok(int track_ok)
{
	setProperty("track_ok",track_ok);
}

int bgp_pe_ipt_default_trackRecord::default_hacia_peer()
{
	return property("default_hacia_peer").toInt();
}

QString bgp_pe_ipt_default_trackRecord::detalle()
{
	return property("detalle").toString();
}

QDate bgp_pe_ipt_default_trackRecord::fecha()
{
	return property("fecha").toDate();
}

int bgp_pe_ipt_default_trackRecord::ok()
{
	return property("ok").toInt();
}

QString bgp_pe_ipt_default_trackRecord::pais()
{
	return property("pais").toString();
}

QString bgp_pe_ipt_default_trackRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString bgp_pe_ipt_default_trackRecord::pe_nombre()
{
	return property("pe_nombre").toString();
}

QString bgp_pe_ipt_default_trackRecord::peer()
{
	return property("peer").toString();
}

QString bgp_pe_ipt_default_trackRecord::track()
{
	return property("track").toString();
}

int bgp_pe_ipt_default_trackRecord::track_ok()
{
	return property("track_ok").toInt();
}

bgp_pe_ipt_default_trackRecord &bgp_pe_ipt_default_trackRecord::operator=(const bgp_pe_ipt_default_trackRecord &other)
{
	clone(other);
	return *this;
}
