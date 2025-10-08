#include "a9k_slotsrecord.h"

a9k_slotsRecord::a9k_slotsRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_slots";
}

a9k_slotsRecord::a9k_slotsRecord(const a9k_slotsRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_slots";
	clone(other);
}

a9k_slotsRecord::~a9k_slotsRecord() {}

void a9k_slotsRecord::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_slotsRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_slotsRecord::set_device_ip(QString device_ip)
{
	setProperty("device_ip",device_ip);
}

void a9k_slotsRecord::set_device_name(QString device_name)
{
	setProperty("device_name",device_name);
}

void a9k_slotsRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_slotsRecord::set_node(QString node)
{
	setProperty("node",node);
}

void a9k_slotsRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void a9k_slotsRecord::set_state(QString state)
{
	setProperty("state",state);
}

void a9k_slotsRecord::set_type(QString type)
{
	setProperty("type",type);
}

int a9k_slotsRecord::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

QDate a9k_slotsRecord::date()
{
	return property("date").toDate();
}

QString a9k_slotsRecord::device_ip()
{
	return property("device_ip").toString();
}

QString a9k_slotsRecord::device_name()
{
	return property("device_name").toString();
}

int a9k_slotsRecord::id()
{
	return property("id").toInt();
}

QString a9k_slotsRecord::node()
{
	return property("node").toString();
}

QString a9k_slotsRecord::pais()
{
	return property("pais").toString();
}

QString a9k_slotsRecord::state()
{
	return property("state").toString();
}

QString a9k_slotsRecord::type()
{
	return property("type").toString();
}

a9k_equipoRecord *a9k_slotsRecord::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_slotsRecord &a9k_slotsRecord::operator=(const a9k_slotsRecord &other)
{
	clone(other);
	return *this;
}
