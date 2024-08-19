#include "tb_pais_anillos_cantidadrecord.h"

TB_Pais_anillos_cantidadRecord::TB_Pais_anillos_cantidadRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="TB_Pais_anillos_cantidad";
}

TB_Pais_anillos_cantidadRecord::TB_Pais_anillos_cantidadRecord(const TB_Pais_anillos_cantidadRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="TB_Pais_anillos_cantidad";
	clone(other);
}

TB_Pais_anillos_cantidadRecord::~TB_Pais_anillos_cantidadRecord() {}

void TB_Pais_anillos_cantidadRecord::set_cantidad(unsigned int cantidad)
{
	setProperty("cantidad",cantidad);
}

void TB_Pais_anillos_cantidadRecord::set_cantidad_buses(int cantidad_buses)
{
	setProperty("cantidad_buses",cantidad_buses);
}

void TB_Pais_anillos_cantidadRecord::set_cantidad_en_anillos(int cantidad_en_anillos)
{
	setProperty("cantidad_en_anillos",cantidad_en_anillos);
}

void TB_Pais_anillos_cantidadRecord::set_cantidad_en_buses(int cantidad_en_buses)
{
	setProperty("cantidad_en_buses",cantidad_en_buses);
}

void TB_Pais_anillos_cantidadRecord::set_cantidad_equipos(int cantidad_equipos)
{
	setProperty("cantidad_equipos",cantidad_equipos);
}

void TB_Pais_anillos_cantidadRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void TB_Pais_anillos_cantidadRecord::set_id(unsigned long long id)
{
	setProperty("id",id);
}

void TB_Pais_anillos_cantidadRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

unsigned int TB_Pais_anillos_cantidadRecord::cantidad()
{
	return property("cantidad").toUInt();
}

int TB_Pais_anillos_cantidadRecord::cantidad_buses()
{
	return property("cantidad_buses").toInt();
}

int TB_Pais_anillos_cantidadRecord::cantidad_en_anillos()
{
	return property("cantidad_en_anillos").toInt();
}

int TB_Pais_anillos_cantidadRecord::cantidad_en_buses()
{
	return property("cantidad_en_buses").toInt();
}

int TB_Pais_anillos_cantidadRecord::cantidad_equipos()
{
	return property("cantidad_equipos").toInt();
}

QDate TB_Pais_anillos_cantidadRecord::fecha()
{
	return property("fecha").toDate();
}

unsigned long long TB_Pais_anillos_cantidadRecord::id()
{
	return property("id").toULongLong();
}

QString TB_Pais_anillos_cantidadRecord::pais()
{
	return property("pais").toString();
}

TB_Pais_anillos_cantidadRecord &TB_Pais_anillos_cantidadRecord::operator=(const TB_Pais_anillos_cantidadRecord &other)
{
	clone(other);
	return *this;
}
