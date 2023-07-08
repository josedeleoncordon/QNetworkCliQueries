#include "a9k_bgp_inter_asrecord.h"

a9k_bgp_inter_asRecord::a9k_bgp_inter_asRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_bgp_inter_as";
}

a9k_bgp_inter_asRecord::a9k_bgp_inter_asRecord(const a9k_bgp_inter_asRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_bgp_inter_as";
	clone(other);
}

a9k_bgp_inter_asRecord::~a9k_bgp_inter_asRecord() {}

void a9k_bgp_inter_asRecord::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_bgp_inter_asRecord::set_asn(QString asn)
{
	setProperty("asn",asn);
}

void a9k_bgp_inter_asRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_bgp_inter_asRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_bgp_inter_asRecord::set_neighbor(QString neighbor)
{
	setProperty("neighbor",neighbor);
}

int a9k_bgp_inter_asRecord::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

QString a9k_bgp_inter_asRecord::asn()
{
	return property("asn").toString();
}

QDate a9k_bgp_inter_asRecord::date()
{
	return property("date").toDate();
}

int a9k_bgp_inter_asRecord::id()
{
	return property("id").toInt();
}

QString a9k_bgp_inter_asRecord::neighbor()
{
	return property("neighbor").toString();
}

a9k_equipoRecord *a9k_bgp_inter_asRecord::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_bgp_inter_asRecord &a9k_bgp_inter_asRecord::operator=(const a9k_bgp_inter_asRecord &other)
{
	clone(other);
	return *this;
}
