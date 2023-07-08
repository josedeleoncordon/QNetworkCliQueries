#include "paisrecord.h"

paisRecord::paisRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="pais";
}

paisRecord::paisRecord(const paisRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="pais";
	clone(other);
}

paisRecord::~paisRecord() {}

void paisRecord::set_id(int id)
{
	setProperty("id",id);
}

void paisRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

int paisRecord::id()
{
	return property("id").toInt();
}

QString paisRecord::pais()
{
	return property("pais").toString();
}

paisRecord &paisRecord::operator=(const paisRecord &other)
{
	clone(other);
	return *this;
}
