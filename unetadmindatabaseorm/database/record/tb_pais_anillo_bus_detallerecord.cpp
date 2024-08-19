#include "tb_pais_anillo_bus_detallerecord.h"

TB_Pais_anillo_bus_detalleRecord::TB_Pais_anillo_bus_detalleRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="TB_Pais_anillo_bus_detalle";
}

TB_Pais_anillo_bus_detalleRecord::TB_Pais_anillo_bus_detalleRecord(const TB_Pais_anillo_bus_detalleRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="TB_Pais_anillo_bus_detalle";
	clone(other);
}

TB_Pais_anillo_bus_detalleRecord::~TB_Pais_anillo_bus_detalleRecord() {}

void TB_Pais_anillo_bus_detalleRecord::set_con_reporte(bool con_reporte)
{
	setProperty("con_reporte",con_reporte);
}

void TB_Pais_anillo_bus_detalleRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void TB_Pais_anillo_bus_detalleRecord::set_hostname(QString hostname)
{
	setProperty("hostname",hostname);
}

void TB_Pais_anillo_bus_detalleRecord::set_id(unsigned long long id)
{
	setProperty("id",id);
}

void TB_Pais_anillo_bus_detalleRecord::set_ip(QString ip)
{
	setProperty("ip",ip);
}

void TB_Pais_anillo_bus_detalleRecord::set_local_port(QString local_port)
{
	setProperty("local_port",local_port);
}

void TB_Pais_anillo_bus_detalleRecord::set_local_port_description(QString local_port_description)
{
	setProperty("local_port_description",local_port_description);
}

void TB_Pais_anillo_bus_detalleRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void TB_Pais_anillo_bus_detalleRecord::set_remote_host(QString remote_host)
{
	setProperty("remote_host",remote_host);
}

void TB_Pais_anillo_bus_detalleRecord::set_remote_port(QString remote_port)
{
	setProperty("remote_port",remote_port);
}

void TB_Pais_anillo_bus_detalleRecord::set_tipo(QString tipo)
{
	setProperty("tipo",tipo);
}

void TB_Pais_anillo_bus_detalleRecord::set_txt(QString txt)
{
	setProperty("txt",txt);
}

bool TB_Pais_anillo_bus_detalleRecord::con_reporte()
{
	return property("con_reporte").toBool();
}

QDate TB_Pais_anillo_bus_detalleRecord::fecha()
{
	return property("fecha").toDate();
}

QString TB_Pais_anillo_bus_detalleRecord::hostname()
{
	return property("hostname").toString();
}

unsigned long long TB_Pais_anillo_bus_detalleRecord::id()
{
	return property("id").toULongLong();
}

QString TB_Pais_anillo_bus_detalleRecord::ip()
{
	return property("ip").toString();
}

QString TB_Pais_anillo_bus_detalleRecord::local_port()
{
	return property("local_port").toString();
}

QString TB_Pais_anillo_bus_detalleRecord::local_port_description()
{
	return property("local_port_description").toString();
}

QString TB_Pais_anillo_bus_detalleRecord::pais()
{
	return property("pais").toString();
}

QString TB_Pais_anillo_bus_detalleRecord::remote_host()
{
	return property("remote_host").toString();
}

QString TB_Pais_anillo_bus_detalleRecord::remote_port()
{
	return property("remote_port").toString();
}

QString TB_Pais_anillo_bus_detalleRecord::tipo()
{
	return property("tipo").toString();
}

QString TB_Pais_anillo_bus_detalleRecord::txt()
{
	return property("txt").toString();
}

TB_Pais_anillo_bus_detalleRecord &TB_Pais_anillo_bus_detalleRecord::operator=(const TB_Pais_anillo_bus_detalleRecord &other)
{
	clone(other);
	return *this;
}
