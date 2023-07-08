#include "a9k_energiarecord.h"

a9k_energiaRecord::a9k_energiaRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_energia";
}

a9k_energiaRecord::a9k_energiaRecord(const a9k_energiaRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_energia";
	clone(other);
}

a9k_energiaRecord::~a9k_energiaRecord() {}

void a9k_energiaRecord::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_energiaRecord::set_bandeja0fuentes(int bandeja0fuentes)
{
	setProperty("bandeja0fuentes",bandeja0fuentes);
}

void a9k_energiaRecord::set_bandeja1fuentes(int bandeja1fuentes)
{
	setProperty("bandeja1fuentes",bandeja1fuentes);
}

void a9k_energiaRecord::set_capacidad_total(int capacidad_total)
{
	setProperty("capacidad_total",capacidad_total);
}

void a9k_energiaRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_energiaRecord::set_disponible_en_falla(int disponible_en_falla)
{
	setProperty("disponible_en_falla",disponible_en_falla);
}

void a9k_energiaRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_energiaRecord::set_nmas1(int nmas1)
{
	setProperty("nmas1",nmas1);
}

void a9k_energiaRecord::set_sin_capacidad_en_falla(int sin_capacidad_en_falla)
{
	setProperty("sin_capacidad_en_falla",sin_capacidad_en_falla);
}

void a9k_energiaRecord::set_total_requerido(int total_requerido)
{
	setProperty("total_requerido",total_requerido);
}

int a9k_energiaRecord::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

int a9k_energiaRecord::bandeja0fuentes()
{
	return property("bandeja0fuentes").toInt();
}

int a9k_energiaRecord::bandeja1fuentes()
{
	return property("bandeja1fuentes").toInt();
}

int a9k_energiaRecord::capacidad_total()
{
	return property("capacidad_total").toInt();
}

QDate a9k_energiaRecord::date()
{
	return property("date").toDate();
}

int a9k_energiaRecord::disponible_en_falla()
{
	return property("disponible_en_falla").toInt();
}

int a9k_energiaRecord::id()
{
	return property("id").toInt();
}

int a9k_energiaRecord::nmas1()
{
	return property("nmas1").toInt();
}

int a9k_energiaRecord::sin_capacidad_en_falla()
{
	return property("sin_capacidad_en_falla").toInt();
}

int a9k_energiaRecord::total_requerido()
{
	return property("total_requerido").toInt();
}

a9k_equipoRecord *a9k_energiaRecord::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_energiaRecord &a9k_energiaRecord::operator=(const a9k_energiaRecord &other)
{
	clone(other);
	return *this;
}
