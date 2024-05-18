#include "tb_pais_anillos_detallerecord.h"

TB_Pais_anillos_detalleRecord::TB_Pais_anillos_detalleRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="TB_Pais_anillos_detalle";
}

TB_Pais_anillos_detalleRecord::TB_Pais_anillos_detalleRecord(const TB_Pais_anillos_detalleRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="TB_Pais_anillos_detalle";
	clone(other);
}

TB_Pais_anillos_detalleRecord::~TB_Pais_anillos_detalleRecord() {}

void TB_Pais_anillos_detalleRecord::set_anillotxt(QString anillotxt)
{
	setProperty("anillotxt",anillotxt);
}

void TB_Pais_anillos_detalleRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void TB_Pais_anillos_detalleRecord::set_id(unsigned long long id)
{
	setProperty("id",id);
}

void TB_Pais_anillos_detalleRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

QString TB_Pais_anillos_detalleRecord::anillotxt()
{
	return property("anillotxt").toString();
}

QDate TB_Pais_anillos_detalleRecord::fecha()
{
	return property("fecha").toDate();
}

unsigned long long TB_Pais_anillos_detalleRecord::id()
{
	return property("id").toULongLong();
}

QString TB_Pais_anillos_detalleRecord::pais()
{
	return property("pais").toString();
}

TB_Pais_anillos_detalleRecord &TB_Pais_anillos_detalleRecord::operator=(const TB_Pais_anillos_detalleRecord &other)
{
	clone(other);
	return *this;
}
