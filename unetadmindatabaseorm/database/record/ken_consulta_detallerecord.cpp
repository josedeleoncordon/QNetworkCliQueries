#include "ken_consulta_detallerecord.h"

ken_consulta_detalleRecord::ken_consulta_detalleRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="ken_consulta_detalle";
}

ken_consulta_detalleRecord::ken_consulta_detalleRecord(const ken_consulta_detalleRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="ken_consulta_detalle";
	clone(other);
}

ken_consulta_detalleRecord::~ken_consulta_detalleRecord() {}

void ken_consulta_detalleRecord::set_asn(QString asn)
{
	setProperty("asn",asn);
}

void ken_consulta_detalleRecord::set_destino_ip(QString destino_ip)
{
	setProperty("destino_ip",destino_ip);
}

void ken_consulta_detalleRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void ken_consulta_detalleRecord::set_id(long long id)
{
	setProperty("id",id);
}

void ken_consulta_detalleRecord::set_ken_equipo_id(int ken_equipo_id)
{
	setProperty("ken_equipo_id",ken_equipo_id);
}

void ken_consulta_detalleRecord::set_max_bps(double max_bps)
{
	setProperty("max_bps",max_bps);
}

void ken_consulta_detalleRecord::set_red(QString red)
{
	setProperty("red",red);
}

QString ken_consulta_detalleRecord::asn()
{
	return property("asn").toString();
}

QString ken_consulta_detalleRecord::destino_ip()
{
	return property("destino_ip").toString();
}

QDate ken_consulta_detalleRecord::fecha()
{
	return property("fecha").toDate();
}

long long ken_consulta_detalleRecord::id()
{
	return property("id").toLongLong();
}

int ken_consulta_detalleRecord::ken_equipo_id()
{
	return property("ken_equipo_id").toInt();
}

double ken_consulta_detalleRecord::max_bps()
{
	return property("max_bps").toDouble();
}

QString ken_consulta_detalleRecord::red()
{
	return property("red").toString();
}

ken_equipoRecord *ken_consulta_detalleRecord::r_ken_equipo_id()
{
	return dynamic_cast<ken_equipoRecord*>( property("r_ken_equipo_id").value<RecordBase*>() );
}

ken_consulta_detalleRecord &ken_consulta_detalleRecord::operator=(const ken_consulta_detalleRecord &other)
{
	clone(other);
	return *this;
}
