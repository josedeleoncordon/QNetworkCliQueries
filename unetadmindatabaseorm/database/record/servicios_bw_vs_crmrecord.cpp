#include "servicios_bw_vs_crmrecord.h"

servicios_bw_vs_crmRecord::servicios_bw_vs_crmRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="servicios_bw_vs_crm";
}

servicios_bw_vs_crmRecord::servicios_bw_vs_crmRecord(const servicios_bw_vs_crmRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="servicios_bw_vs_crm";
	clone(other);
}

servicios_bw_vs_crmRecord::~servicios_bw_vs_crmRecord() {}

void servicios_bw_vs_crmRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void servicios_bw_vs_crmRecord::set_id(long long id)
{
	setProperty("id",id);
}

void servicios_bw_vs_crmRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void servicios_bw_vs_crmRecord::set_porcentaje(double porcentaje)
{
	setProperty("porcentaje",porcentaje);
}

QDate servicios_bw_vs_crmRecord::fecha()
{
	return property("fecha").toDate();
}

long long servicios_bw_vs_crmRecord::id()
{
	return property("id").toLongLong();
}

QString servicios_bw_vs_crmRecord::pais()
{
	return property("pais").toString();
}

double servicios_bw_vs_crmRecord::porcentaje()
{
	return property("porcentaje").toDouble();
}

servicios_bw_vs_crmRecord &servicios_bw_vs_crmRecord::operator=(const servicios_bw_vs_crmRecord &other)
{
	clone(other);
	return *this;
}
