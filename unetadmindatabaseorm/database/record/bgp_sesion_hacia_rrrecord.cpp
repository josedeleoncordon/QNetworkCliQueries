#include "bgp_sesion_hacia_rrrecord.h"

bgp_sesion_hacia_rrRecord::bgp_sesion_hacia_rrRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="bgp_sesion_hacia_rr";
}

bgp_sesion_hacia_rrRecord::bgp_sesion_hacia_rrRecord(const bgp_sesion_hacia_rrRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="bgp_sesion_hacia_rr";
	clone(other);
}

bgp_sesion_hacia_rrRecord::~bgp_sesion_hacia_rrRecord() {}

void bgp_sesion_hacia_rrRecord::set_address_family(QString address_family)
{
	setProperty("address_family",address_family);
}

void bgp_sesion_hacia_rrRecord::set_cantidad_sesiones(int cantidad_sesiones)
{
	setProperty("cantidad_sesiones",cantidad_sesiones);
}

void bgp_sesion_hacia_rrRecord::set_cantidad_sesiones_caidas(int cantidad_sesiones_caidas)
{
	setProperty("cantidad_sesiones_caidas",cantidad_sesiones_caidas);
}

void bgp_sesion_hacia_rrRecord::set_cantidad_sesiones_errores(int cantidad_sesiones_errores)
{
	setProperty("cantidad_sesiones_errores",cantidad_sesiones_errores);
}

void bgp_sesion_hacia_rrRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void bgp_sesion_hacia_rrRecord::set_id(int id)
{
	setProperty("id",id);
}

void bgp_sesion_hacia_rrRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

QString bgp_sesion_hacia_rrRecord::address_family()
{
	return property("address_family").toString();
}

int bgp_sesion_hacia_rrRecord::cantidad_sesiones()
{
	return property("cantidad_sesiones").toInt();
}

int bgp_sesion_hacia_rrRecord::cantidad_sesiones_caidas()
{
	return property("cantidad_sesiones_caidas").toInt();
}

int bgp_sesion_hacia_rrRecord::cantidad_sesiones_errores()
{
	return property("cantidad_sesiones_errores").toInt();
}

QDate bgp_sesion_hacia_rrRecord::fecha()
{
	return property("fecha").toDate();
}

int bgp_sesion_hacia_rrRecord::id()
{
	return property("id").toInt();
}

QString bgp_sesion_hacia_rrRecord::pais()
{
	return property("pais").toString();
}

bgp_sesion_hacia_rrRecord &bgp_sesion_hacia_rrRecord::operator=(const bgp_sesion_hacia_rrRecord &other)
{
	clone(other);
	return *this;
}
