#include "tb_addressing_consultarecord.h"

TB_Addressing_consultaRecord::TB_Addressing_consultaRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="TB_Addressing_consulta";
}

TB_Addressing_consultaRecord::TB_Addressing_consultaRecord(const TB_Addressing_consultaRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="TB_Addressing_consulta";
	clone(other);
}

TB_Addressing_consultaRecord::~TB_Addressing_consultaRecord() {}

void TB_Addressing_consultaRecord::set_Fecha(QDate Fecha)
{
	setProperty("Fecha",Fecha);
}

void TB_Addressing_consultaRecord::set_Hostname(QString Hostname)
{
	setProperty("Hostname",Hostname);
}

void TB_Addressing_consultaRecord::set_IP(QString IP)
{
	setProperty("IP",IP);
}

void TB_Addressing_consultaRecord::set_Interface(QString Interface)
{
	setProperty("Interface",Interface);
}

void TB_Addressing_consultaRecord::set_VRF(QString VRF)
{
	setProperty("VRF",VRF);
}

void TB_Addressing_consultaRecord::set_id(long long id)
{
	setProperty("id",id);
}

QDate TB_Addressing_consultaRecord::Fecha()
{
	return property("Fecha").toDate();
}

QString TB_Addressing_consultaRecord::Hostname()
{
	return property("Hostname").toString();
}

QString TB_Addressing_consultaRecord::IP()
{
	return property("IP").toString();
}

QString TB_Addressing_consultaRecord::Interface()
{
	return property("Interface").toString();
}

QString TB_Addressing_consultaRecord::VRF()
{
	return property("VRF").toString();
}

long long TB_Addressing_consultaRecord::id()
{
	return property("id").toLongLong();
}

TB_Addressing_consultaRecord &TB_Addressing_consultaRecord::operator=(const TB_Addressing_consultaRecord &other)
{
	clone(other);
	return *this;
}
