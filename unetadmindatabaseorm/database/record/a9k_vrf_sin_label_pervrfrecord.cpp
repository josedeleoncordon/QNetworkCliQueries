#include "a9k_vrf_sin_label_pervrfrecord.h"

a9k_vrf_sin_label_pervrfRecord::a9k_vrf_sin_label_pervrfRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_vrf_sin_label_pervrf";
}

a9k_vrf_sin_label_pervrfRecord::a9k_vrf_sin_label_pervrfRecord(const a9k_vrf_sin_label_pervrfRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_vrf_sin_label_pervrf";
	clone(other);
}

a9k_vrf_sin_label_pervrfRecord::~a9k_vrf_sin_label_pervrfRecord() {}

void a9k_vrf_sin_label_pervrfRecord::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_vrf_sin_label_pervrfRecord::set_address_family(QString address_family)
{
	setProperty("address_family",address_family);
}

void a9k_vrf_sin_label_pervrfRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_vrf_sin_label_pervrfRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_vrf_sin_label_pervrfRecord::set_vrf(QString vrf)
{
	setProperty("vrf",vrf);
}

int a9k_vrf_sin_label_pervrfRecord::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

QString a9k_vrf_sin_label_pervrfRecord::address_family()
{
	return property("address_family").toString();
}

QDate a9k_vrf_sin_label_pervrfRecord::date()
{
	return property("date").toDate();
}

int a9k_vrf_sin_label_pervrfRecord::id()
{
	return property("id").toInt();
}

QString a9k_vrf_sin_label_pervrfRecord::vrf()
{
	return property("vrf").toString();
}

a9k_equipoRecord *a9k_vrf_sin_label_pervrfRecord::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_vrf_sin_label_pervrfRecord &a9k_vrf_sin_label_pervrfRecord::operator=(const a9k_vrf_sin_label_pervrfRecord &other)
{
	clone(other);
	return *this;
}
