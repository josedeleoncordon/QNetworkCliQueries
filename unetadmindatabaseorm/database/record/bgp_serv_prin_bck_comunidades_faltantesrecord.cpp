#include "bgp_serv_prin_bck_comunidades_faltantesrecord.h"

bgp_serv_prin_bck_comunidades_faltantesRecord::bgp_serv_prin_bck_comunidades_faltantesRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_serv_prin_bck_comunidades_faltantes";
}

bgp_serv_prin_bck_comunidades_faltantesRecord::bgp_serv_prin_bck_comunidades_faltantesRecord(const bgp_serv_prin_bck_comunidades_faltantesRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_serv_prin_bck_comunidades_faltantes";
	clone(other);
}

bgp_serv_prin_bck_comunidades_faltantesRecord::~bgp_serv_prin_bck_comunidades_faltantesRecord() {}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_activo(int activo)
{
	setProperty("activo",activo);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_comunidades_faltantes(QString comunidades_faltantes)
{
	setProperty("comunidades_faltantes",comunidades_faltantes);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_id(int id)
{
	setProperty("id",id);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_neighbor_nexthop(QString neighbor_nexthop)
{
	setProperty("neighbor_nexthop",neighbor_nexthop);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_pe_nombre(QString pe_nombre)
{
	setProperty("pe_nombre",pe_nombre);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_red(QString red)
{
	setProperty("red",red);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_routepolicy(QString routepolicy)
{
	setProperty("routepolicy",routepolicy);
}

void bgp_serv_prin_bck_comunidades_faltantesRecord::set_vrf(QString vrf)
{
	setProperty("vrf",vrf);
}

int bgp_serv_prin_bck_comunidades_faltantesRecord::activo()
{
	return property("activo").toInt();
}

QString bgp_serv_prin_bck_comunidades_faltantesRecord::comunidades_faltantes()
{
	return property("comunidades_faltantes").toString();
}

QDate bgp_serv_prin_bck_comunidades_faltantesRecord::date()
{
	return property("date").toDate();
}

int bgp_serv_prin_bck_comunidades_faltantesRecord::id()
{
	return property("id").toInt();
}

QString bgp_serv_prin_bck_comunidades_faltantesRecord::neighbor_nexthop()
{
	return property("neighbor_nexthop").toString();
}

QString bgp_serv_prin_bck_comunidades_faltantesRecord::pais()
{
	return property("pais").toString();
}

QString bgp_serv_prin_bck_comunidades_faltantesRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString bgp_serv_prin_bck_comunidades_faltantesRecord::pe_nombre()
{
	return property("pe_nombre").toString();
}

QString bgp_serv_prin_bck_comunidades_faltantesRecord::red()
{
	return property("red").toString();
}

QString bgp_serv_prin_bck_comunidades_faltantesRecord::routepolicy()
{
	return property("routepolicy").toString();
}

QString bgp_serv_prin_bck_comunidades_faltantesRecord::vrf()
{
	return property("vrf").toString();
}

bgp_serv_prin_bck_comunidades_faltantesRecord &bgp_serv_prin_bck_comunidades_faltantesRecord::operator=(const bgp_serv_prin_bck_comunidades_faltantesRecord &other)
{
	clone(other);
	return *this;
}
