#include "ken_consultarecord.h"

ken_consultaRecord::ken_consultaRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="ken_consulta";
}

ken_consultaRecord::ken_consultaRecord(const ken_consultaRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="ken_consulta";
	clone(other);
}

ken_consultaRecord::~ken_consultaRecord() {}

void ken_consultaRecord::set_destino_ip(QString destino_ip)
{
	setProperty("destino_ip",destino_ip);
}

void ken_consultaRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void ken_consultaRecord::set_id(long long id)
{
	setProperty("id",id);
}

void ken_consultaRecord::set_ken_equipo_id(int ken_equipo_id)
{
	setProperty("ken_equipo_id",ken_equipo_id);
}

void ken_consultaRecord::set_max_bps(double max_bps)
{
	setProperty("max_bps",max_bps);
}

QString ken_consultaRecord::destino_ip()
{
	return property("destino_ip").toString();
}

QDate ken_consultaRecord::fecha()
{
	return property("fecha").toDate();
}

long long ken_consultaRecord::id()
{
	return property("id").toLongLong();
}

int ken_consultaRecord::ken_equipo_id()
{
	return property("ken_equipo_id").toInt();
}

double ken_consultaRecord::max_bps()
{
	return property("max_bps").toDouble();
}

ken_equipoRecord *ken_consultaRecord::r_ken_equipo_id()
{
	return dynamic_cast<ken_equipoRecord*>( property("r_ken_equipo_id").value<RecordBase*>() );
}

ken_consultaRecord &ken_consultaRecord::operator=(const ken_consultaRecord &other)
{
	clone(other);
	return *this;
}
