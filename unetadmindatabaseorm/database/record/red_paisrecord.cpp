#include "red_paisrecord.h"

red_paisRecord::red_paisRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="red_pais";
}

red_paisRecord::red_paisRecord(const red_paisRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="red_pais";
	clone(other);
}

red_paisRecord::~red_paisRecord() {}

void red_paisRecord::set_id(int id)
{
	setProperty("id",id);
}

void red_paisRecord::set_pais(QString pais)
{
	setProperty("pais",pais);
}

int red_paisRecord::id()
{
	return property("id").toInt();
}

QString red_paisRecord::pais()
{
	return property("pais").toString();
}

red_paisRecord &red_paisRecord::operator=(const red_paisRecord &other)
{
	clone(other);
	return *this;
}
