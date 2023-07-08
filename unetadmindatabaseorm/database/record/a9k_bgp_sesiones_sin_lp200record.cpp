#include "a9k_bgp_sesiones_sin_lp200record.h"

a9k_bgp_sesiones_sin_lp200Record::a9k_bgp_sesiones_sin_lp200Record(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_bgp_sesiones_sin_lp200";
}

a9k_bgp_sesiones_sin_lp200Record::a9k_bgp_sesiones_sin_lp200Record(const a9k_bgp_sesiones_sin_lp200Record &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_bgp_sesiones_sin_lp200";
	clone(other);
}

a9k_bgp_sesiones_sin_lp200Record::~a9k_bgp_sesiones_sin_lp200Record() {}

void a9k_bgp_sesiones_sin_lp200Record::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_bgp_sesiones_sin_lp200Record::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_bgp_sesiones_sin_lp200Record::set_id(int id)
{
	setProperty("id",id);
}

void a9k_bgp_sesiones_sin_lp200Record::set_neighbor(QString neighbor)
{
	setProperty("neighbor",neighbor);
}

void a9k_bgp_sesiones_sin_lp200Record::set_prefixin(int prefixin)
{
	setProperty("prefixin",prefixin);
}

void a9k_bgp_sesiones_sin_lp200Record::set_routepolicy(QString routepolicy)
{
	setProperty("routepolicy",routepolicy);
}

int a9k_bgp_sesiones_sin_lp200Record::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

QDate a9k_bgp_sesiones_sin_lp200Record::date()
{
	return property("date").toDate();
}

int a9k_bgp_sesiones_sin_lp200Record::id()
{
	return property("id").toInt();
}

QString a9k_bgp_sesiones_sin_lp200Record::neighbor()
{
	return property("neighbor").toString();
}

int a9k_bgp_sesiones_sin_lp200Record::prefixin()
{
	return property("prefixin").toInt();
}

QString a9k_bgp_sesiones_sin_lp200Record::routepolicy()
{
	return property("routepolicy").toString();
}

a9k_equipoRecord *a9k_bgp_sesiones_sin_lp200Record::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_bgp_sesiones_sin_lp200Record &a9k_bgp_sesiones_sin_lp200Record::operator=(const a9k_bgp_sesiones_sin_lp200Record &other)
{
	clone(other);
	return *this;
}
