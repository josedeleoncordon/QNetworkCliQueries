#include "red_consulta_detallerecord.h"

red_consulta_detalleRecord::red_consulta_detalleRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="red_consulta_detalle";
}

red_consulta_detalleRecord::red_consulta_detalleRecord(const red_consulta_detalleRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="red_consulta_detalle";
	clone(other);
}

red_consulta_detalleRecord::~red_consulta_detalleRecord() {}

void red_consulta_detalleRecord::set_Descripcion(QString Descripcion)
{
	setProperty("Descripcion",Descripcion);
}

void red_consulta_detalleRecord::set_Detalle(QString Detalle)
{
	setProperty("Detalle",Detalle);
}

void red_consulta_detalleRecord::set_IP(QString IP)
{
	setProperty("IP",IP);
}

void red_consulta_detalleRecord::set_Principal_Interface(QString Principal_Interface)
{
	setProperty("Principal_Interface",Principal_Interface);
}

void red_consulta_detalleRecord::set_Principal_PE(QString Principal_PE)
{
	setProperty("Principal_PE",Principal_PE);
}

void red_consulta_detalleRecord::set_Redundancia(QString Redundancia)
{
	setProperty("Redundancia",Redundancia);
}

void red_consulta_detalleRecord::set_Remoto_IP(QString Remoto_IP)
{
	setProperty("Remoto_IP",Remoto_IP);
}

void red_consulta_detalleRecord::set_Remoto_Interface(QString Remoto_Interface)
{
	setProperty("Remoto_Interface",Remoto_Interface);
}

void red_consulta_detalleRecord::set_Respaldo_Interface(QString Respaldo_Interface)
{
	setProperty("Respaldo_Interface",Respaldo_Interface);
}

void red_consulta_detalleRecord::set_Respaldo_PE(QString Respaldo_PE)
{
	setProperty("Respaldo_PE",Respaldo_PE);
}

void red_consulta_detalleRecord::set_VCID_Principal(QString VCID_Principal)
{
	setProperty("VCID_Principal",VCID_Principal);
}

void red_consulta_detalleRecord::set_VCID_Principal_Estado(QString VCID_Principal_Estado)
{
	setProperty("VCID_Principal_Estado",VCID_Principal_Estado);
}

void red_consulta_detalleRecord::set_VCID_Respaldo(QString VCID_Respaldo)
{
	setProperty("VCID_Respaldo",VCID_Respaldo);
}

void red_consulta_detalleRecord::set_VCID_Respaldo_Estado(QString VCID_Respaldo_Estado)
{
	setProperty("VCID_Respaldo_Estado",VCID_Respaldo_Estado);
}

void red_consulta_detalleRecord::set_VRF(QString VRF)
{
	setProperty("VRF",VRF);
}

void red_consulta_detalleRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void red_consulta_detalleRecord::set_id(long long id)
{
	setProperty("id",id);
}

void red_consulta_detalleRecord::set_red_sector_id(int red_sector_id)
{
	setProperty("red_sector_id",red_sector_id);
}

QString red_consulta_detalleRecord::Descripcion()
{
	return property("Descripcion").toString();
}

QString red_consulta_detalleRecord::Detalle()
{
	return property("Detalle").toString();
}

QString red_consulta_detalleRecord::IP()
{
	return property("IP").toString();
}

QString red_consulta_detalleRecord::Principal_Interface()
{
	return property("Principal_Interface").toString();
}

QString red_consulta_detalleRecord::Principal_PE()
{
	return property("Principal_PE").toString();
}

QString red_consulta_detalleRecord::Redundancia()
{
	return property("Redundancia").toString();
}

QString red_consulta_detalleRecord::Remoto_IP()
{
	return property("Remoto_IP").toString();
}

QString red_consulta_detalleRecord::Remoto_Interface()
{
	return property("Remoto_Interface").toString();
}

QString red_consulta_detalleRecord::Respaldo_Interface()
{
	return property("Respaldo_Interface").toString();
}

QString red_consulta_detalleRecord::Respaldo_PE()
{
	return property("Respaldo_PE").toString();
}

QString red_consulta_detalleRecord::VCID_Principal()
{
	return property("VCID_Principal").toString();
}

QString red_consulta_detalleRecord::VCID_Principal_Estado()
{
	return property("VCID_Principal_Estado").toString();
}

QString red_consulta_detalleRecord::VCID_Respaldo()
{
	return property("VCID_Respaldo").toString();
}

QString red_consulta_detalleRecord::VCID_Respaldo_Estado()
{
	return property("VCID_Respaldo_Estado").toString();
}

QString red_consulta_detalleRecord::VRF()
{
	return property("VRF").toString();
}

QDate red_consulta_detalleRecord::fecha()
{
	return property("fecha").toDate();
}

long long red_consulta_detalleRecord::id()
{
	return property("id").toLongLong();
}

int red_consulta_detalleRecord::red_sector_id()
{
	return property("red_sector_id").toInt();
}

red_sectorRecord *red_consulta_detalleRecord::r_red_sector_id()
{
	return dynamic_cast<red_sectorRecord*>( property("r_red_sector_id").value<RecordBase*>() );
}

red_consulta_detalleRecord &red_consulta_detalleRecord::operator=(const red_consulta_detalleRecord &other)
{
	clone(other);
	return *this;
}
