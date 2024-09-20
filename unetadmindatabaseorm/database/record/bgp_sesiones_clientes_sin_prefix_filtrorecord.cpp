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

void bgp_sesiones_clientes_sin_prefix_filtroRecord::set_routepolicy(QString routepolicy)
{
	setProperty("routepolicy",routepolicy);
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

QString bgp_sesiones_clientes_sin_prefix_filtroRecord::routepolicy()
{
	return property("routepolicy").toString();
}

bgp_sesiones_clientes_sin_prefix_filtroRecord &bgp_sesiones_clientes_sin_prefix_filtroRecord::operator=(const bgp_sesiones_clientes_sin_prefix_filtroRecord &other)
{
	clone(other);
	return *this;
}
