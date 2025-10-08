#include "servicios_homologadosrecord.h"

servicios_homologadosRecord::servicios_homologadosRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="servicios_homologados";
}

servicios_homologadosRecord::servicios_homologadosRecord(const servicios_homologadosRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="servicios_homologados";
	clone(other);
}

servicios_homologadosRecord::~servicios_homologadosRecord() {}

void servicios_homologadosRecord::set_diagrama_completo(int diagrama_completo)
{
	setProperty("diagrama_completo",diagrama_completo);
}

void servicios_homologadosRecord::set_estandard_count(int estandard_count)
{
	setProperty("estandard_count",estandard_count);
}

void servicios_homologadosRecord::set_fecha(QDate fecha)
{
	setProperty("fecha",fecha);
}

void servicios_homologadosRecord::set_id(long long id)
{
	setProperty("id",id);
}

void servicios_homologadosRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

void servicios_homologadosRecord::set_total(int total)
{
	setProperty("total",total);
}

int servicios_homologadosRecord::diagrama_completo()
{
	return property("diagrama_completo").toInt();
}

int servicios_homologadosRecord::estandard_count()
{
	return property("estandard_count").toInt();
}

QDate servicios_homologadosRecord::fecha()
{
	return property("fecha").toDate();
}

long long servicios_homologadosRecord::id()
{
	return property("id").toLongLong();
}

QString servicios_homologadosRecord::pais()
{
	return property("pais").toString();
}

int servicios_homologadosRecord::total()
{
	return property("total").toInt();
}

servicios_homologadosRecord &servicios_homologadosRecord::operator=(const servicios_homologadosRecord &other)
{
	clone(other);
	return *this;
}
