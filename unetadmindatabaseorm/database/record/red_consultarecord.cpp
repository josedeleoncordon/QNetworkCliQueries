#include "red_consultarecord.h"

red_consultaRecord::red_consultaRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="red_consulta";
}

red_consultaRecord::red_consultaRecord(const red_consultaRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="red_consulta";
	clone(other);
}

red_consultaRecord::~red_consultaRecord() {}

void red_consultaRecord::set_conexion_error(int conexion_error)
{
	setProperty("conexion_error",conexion_error);
}

void red_consultaRecord::set_excepcion(int excepcion)
{
	setProperty("excepcion",excepcion);
}

void red_consultaRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void red_consultaRecord::set_id(int id)
{
	setProperty("id",id);
}

void red_consultaRecord::set_no(int no)
{
	setProperty("no",no);
}

void red_consultaRecord::set_nohomologado(int nohomologado)
{
	setProperty("nohomologado",nohomologado);
}

void red_consultaRecord::set_noservicio(int noservicio)
{
	setProperty("noservicio",noservicio);
}

void red_consultaRecord::set_ok(int ok)
{
	setProperty("ok",ok);
}

void red_consultaRecord::set_principalcaido(int principalcaido)
{
	setProperty("principalcaido",principalcaido);
}

void red_consultaRecord::set_red_sector_id(int red_sector_id)
{
	setProperty("red_sector_id",red_sector_id);
}

void red_consultaRecord::set_total(int total)
{
	setProperty("total",total);
}

int red_consultaRecord::conexion_error()
{
	return property("conexion_error").toInt();
}

int red_consultaRecord::excepcion()
{
	return property("excepcion").toInt();
}

QDate red_consultaRecord::fecha()
{
	return property("fecha").toDate();
}

int red_consultaRecord::id()
{
	return property("id").toInt();
}

int red_consultaRecord::no()
{
	return property("no").toInt();
}

int red_consultaRecord::nohomologado()
{
	return property("nohomologado").toInt();
}

int red_consultaRecord::noservicio()
{
	return property("noservicio").toInt();
}

int red_consultaRecord::ok()
{
	return property("ok").toInt();
}

int red_consultaRecord::principalcaido()
{
	return property("principalcaido").toInt();
}

int red_consultaRecord::red_sector_id()
{
	return property("red_sector_id").toInt();
}

int red_consultaRecord::total()
{
	return property("total").toInt();
}

red_sectorRecord *red_consultaRecord::r_red_sector_id()
{
	return dynamic_cast<red_sectorRecord*>( property("r_red_sector_id").value<RecordBase*>() );
}

red_consultaRecord &red_consultaRecord::operator=(const red_consultaRecord &other)
{
	clone(other);
	return *this;
}
