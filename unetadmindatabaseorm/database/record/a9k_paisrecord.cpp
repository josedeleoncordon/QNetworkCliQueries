#include "a9k_paisrecord.h"

a9k_paisRecord::a9k_paisRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_pais";
}

a9k_paisRecord::a9k_paisRecord(const a9k_paisRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_pais";
	clone(other);
}

a9k_paisRecord::~a9k_paisRecord() {}

void a9k_paisRecord::set_id(int id)
{
	setProperty("id",id);
}

void a9k_paisRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

int a9k_paisRecord::id()
{
	return property("id").toInt();
}

QString a9k_paisRecord::pais()
{
	return property("pais").toString();
}

a9k_paisRecord &a9k_paisRecord::operator=(const a9k_paisRecord &other)
{
	clone(other);
	return *this;
}
