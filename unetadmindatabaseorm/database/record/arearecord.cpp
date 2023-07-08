#include "arearecord.h"

areaRecord::areaRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="area";
}

areaRecord::areaRecord(const areaRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="area";
	clone(other);
}

areaRecord::~areaRecord() {}

void areaRecord::set_area(QString area)
{
	setProperty("area",area);
}

void areaRecord::set_id(int id)
{
	setProperty("id",id);
}

void areaRecord::set_pais_id(int pais_id)
{
	setProperty("pais_id",pais_id);
}

QString areaRecord::area()
{
	return property("area").toString();
}

int areaRecord::id()
{
	return property("id").toInt();
}

int areaRecord::pais_id()
{
	return property("pais_id").toInt();
}

paisRecord *areaRecord::r_pais_id()
{
	return dynamic_cast<paisRecord*>( property("r_pais_id").value<RecordBase*>() );
}

areaRecord &areaRecord::operator=(const areaRecord &other)
{
	clone(other);
	return *this;
}
