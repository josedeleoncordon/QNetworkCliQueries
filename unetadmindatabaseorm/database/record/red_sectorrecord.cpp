#include "red_sectorrecord.h"

red_sectorRecord::red_sectorRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="red_sector";
}

red_sectorRecord::red_sectorRecord(const red_sectorRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="red_sector";
	clone(other);
}

red_sectorRecord::~red_sectorRecord() {}

void red_sectorRecord::set_id(int id)
{
	setProperty("id",id);
}

void red_sectorRecord::set_red_pais_id(int red_pais_id)
{
	setProperty("red_pais_id",red_pais_id);
}

void red_sectorRecord::set_sector(QString sector)
{
	setProperty("sector",sector);
}

int red_sectorRecord::id()
{
	return property("id").toInt();
}

int red_sectorRecord::red_pais_id()
{
	return property("red_pais_id").toInt();
}

QString red_sectorRecord::sector()
{
	return property("sector").toString();
}

red_paisRecord *red_sectorRecord::r_red_pais_id()
{
	return dynamic_cast<red_paisRecord*>( property("r_red_pais_id").value<RecordBase*>() );
}

red_sectorRecord &red_sectorRecord::operator=(const red_sectorRecord &other)
{
	clone(other);
	return *this;
}
