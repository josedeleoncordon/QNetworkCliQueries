#include "bgp_sesiones_clientes_sin_prefix_filtrorecord.h"

bgp_sesiones_clientes_sin_prefix_filtroRecord::bgp_sesiones_clientes_sin_prefix_filtroRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_sesiones_clientes_sin_prefix_filtro";
}

bgp_sesiones_clientes_sin_prefix_filtroRecord::bgp_sesiones_clientes_sin_prefix_filtroRecord(const bgp_sesiones_clientes_sin_prefix_filtroRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_sesiones_clientes_sin_prefix_filtro";
	clone(other);
}

bgp_sesiones_clientes_sin_prefix_filtroRecord::~bgp_sesiones_clientes_sin_prefix_filtroRecord() {}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_detalle(QString detalle)
{
	setProperty("detalle",detalle);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_id(int id)
{
	setProperty("id",id);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_neighbor(QString neighbor)
{
	setProperty("neighbor",neighbor);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_os(QString os)
{
	setProperty("os",os);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_pe_nombre(QString pe_nombre)
{
	setProperty("pe_nombre",pe_nombre);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_pecpe(QString pecpe)
{
	setProperty("pecpe",pecpe);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_pfxincount(QString pfxincount)
{
	setProperty("pfxincount",pfxincount);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_platform(QString platform)
{
	setProperty("platform",platform);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_prefix(QString prefix)
{
	setProperty("prefix",prefix);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_routepolicy(QString routepolicy)
{
	setProperty("routepolicy",routepolicy);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_status(QString status)
{
	setProperty("status",status);
}

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_vendor(QString vendor)
{
	setProperty("vendor",vendor);
}

QDate bgp_sesiones_clientes_sin_prefix_filtroRecord::date()
{
	return property("date").toDate();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::detalle()
{
	return property("detalle").toString();
}

int bgp_sesiones_clientes_sin_prefix_filtroRecord::id()
{
	return property("id").toInt();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::neighbor()
{
	return property("neighbor").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::os()
{
	return property("os").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::pais()
{
	return property("pais").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::pe_nombre()
{
	return property("pe_nombre").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::pecpe()
{
	return property("pecpe").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::pfxincount()
{
	return property("pfxincount").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::platform()
{
	return property("platform").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::prefix()
{
	return property("prefix").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::routepolicy()
{
	return property("routepolicy").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::status()
{
	return property("status").toString();
}

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::vendor()
{
	return property("vendor").toString();
}

bgp_sesiones_clientes_sin_prefix_filtroRecord &bgp_sesiones_clientes_sin_prefix_filtroRecord::operator=(const bgp_sesiones_clientes_sin_prefix_filtroRecord &other)
{
	clone(other);
	return *this;
}
