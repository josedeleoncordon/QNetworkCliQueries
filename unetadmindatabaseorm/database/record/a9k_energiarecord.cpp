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

void a9k_energiaRecord::set_a9k_energiacol(QString a9k_energiacol)
{
	setProperty("a9k_energiacol",a9k_energiacol);
}

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

void a9k_energiaRecord::set_cantidad_caida_fuentes_soportado(int cantidad_caida_fuentes_soportado)
{
	setProperty("cantidad_caida_fuentes_soportado",cantidad_caida_fuentes_soportado);
}

void a9k_energiaRecord::set_capacidad_total(int capacidad_total)
{
	setProperty("capacidad_total",capacidad_total);
}

void a9k_energiaRecord::set_cisco_caida_mitad_potencia_no_soportado(int cisco_caida_mitad_potencia_no_soportado)
{
	setProperty("cisco_caida_mitad_potencia_no_soportado",cisco_caida_mitad_potencia_no_soportado);
}

void a9k_energiaRecord::set_cisco_capacidad_no_cableada(int cisco_capacidad_no_cableada)
{
	setProperty("cisco_capacidad_no_cableada",cisco_capacidad_no_cableada);
}

void a9k_energiaRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_energiaRecord::set_device_ip(QString device_ip)
{
	setProperty("device_ip",device_ip);
}

void a9k_energiaRecord::set_device_name(QString device_name)
{
	setProperty("device_name",device_name);
}

void a9k_energiaRecord::set_disponible(int disponible)
{
	setProperty("disponible",disponible);
}

void a9k_energiaRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_energiaRecord::set_nmas1(int nmas1)
{
	setProperty("nmas1",nmas1);
}

void a9k_energiaRecord::set_no_soporta_caida_de_mas_de_una_fuente(int no_soporta_caida_de_mas_de_una_fuente)
{
	setProperty("no_soporta_caida_de_mas_de_una_fuente",no_soporta_caida_de_mas_de_una_fuente);
}

void a9k_energiaRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void a9k_energiaRecord::set_total_requerido(int total_requerido)
{
	setProperty("total_requerido",total_requerido);
}

QString a9k_energiaRecord::a9k_energiacol()
{
	return property("a9k_energiacol").toString();
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

int a9k_energiaRecord::cantidad_caida_fuentes_soportado()
{
	return property("cantidad_caida_fuentes_soportado").toInt();
}

int a9k_energiaRecord::capacidad_total()
{
	return property("capacidad_total").toInt();
}

int a9k_energiaRecord::cisco_caida_mitad_potencia_no_soportado()
{
	return property("cisco_caida_mitad_potencia_no_soportado").toInt();
}

int a9k_energiaRecord::cisco_capacidad_no_cableada()
{
	return property("cisco_capacidad_no_cableada").toInt();
}

QDate a9k_energiaRecord::date()
{
	return property("date").toDate();
}

QString a9k_energiaRecord::device_ip()
{
	return property("device_ip").toString();
}

QString a9k_energiaRecord::device_name()
{
	return property("device_name").toString();
}

int a9k_energiaRecord::disponible()
{
	return property("disponible").toInt();
}

int a9k_energiaRecord::id()
{
	return property("id").toInt();
}

int a9k_energiaRecord::nmas1()
{
	return property("nmas1").toInt();
}

int a9k_energiaRecord::no_soporta_caida_de_mas_de_una_fuente()
{
	return property("no_soporta_caida_de_mas_de_una_fuente").toInt();
}

QString a9k_energiaRecord::pais()
{
	return property("pais").toString();
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
