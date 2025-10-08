#include "interfaz_consumorecord.h"

interfaz_consumoRecord::interfaz_consumoRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="interfaz_consumo";
}

interfaz_consumoRecord::interfaz_consumoRecord(const interfaz_consumoRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="interfaz_consumo";
	clone(other);
}

interfaz_consumoRecord::~interfaz_consumoRecord() {}

void interfaz_consumoRecord::set_bandwidth(long long bandwidth)
{
	setProperty("bandwidth",bandwidth);
}

void interfaz_consumoRecord::set_descripcion(QString descripcion)
{
	setProperty("descripcion",descripcion);
}

void interfaz_consumoRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void interfaz_consumoRecord::set_grupo(QString grupo)
{
	setProperty("grupo",grupo);
}

void interfaz_consumoRecord::set_id(int id)
{
	setProperty("id",id);
}

void interfaz_consumoRecord::set_interfaz(QString interfaz)
{
	setProperty("interfaz",interfaz);
}

void interfaz_consumoRecord::set_pe_ip(QString pe_ip)
{
	setProperty("pe_ip",pe_ip);
}

void interfaz_consumoRecord::set_pe_nombre(QString pe_nombre)
{
	setProperty("pe_nombre",pe_nombre);
}

void interfaz_consumoRecord::set_ratein(long long ratein)
{
	setProperty("ratein",ratein);
}

void interfaz_consumoRecord::set_rateout(long long rateout)
{
	setProperty("rateout",rateout);
}

long long interfaz_consumoRecord::bandwidth()
{
	return property("bandwidth").toLongLong();
}

QString interfaz_consumoRecord::descripcion()
{
	return property("descripcion").toString();
}

QDate interfaz_consumoRecord::fecha()
{
	return property("fecha").toDate();
}

QString interfaz_consumoRecord::grupo()
{
	return property("grupo").toString();
}

int interfaz_consumoRecord::id()
{
	return property("id").toInt();
}

QString interfaz_consumoRecord::interfaz()
{
	return property("interfaz").toString();
}

QString interfaz_consumoRecord::pe_ip()
{
	return property("pe_ip").toString();
}

QString interfaz_consumoRecord::pe_nombre()
{
	return property("pe_nombre").toString();
}

long long interfaz_consumoRecord::ratein()
{
	return property("ratein").toLongLong();
}

long long interfaz_consumoRecord::rateout()
{
	return property("rateout").toLongLong();
}

interfaz_consumoRecord &interfaz_consumoRecord::operator=(const interfaz_consumoRecord &other)
{
	clone(other);
	return *this;
}
