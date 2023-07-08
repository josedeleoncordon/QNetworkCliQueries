#include "gruporecord.h"

grupoRecord::grupoRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="grupo";
}

grupoRecord::grupoRecord(const grupoRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="grupo";
	clone(other);
}

grupoRecord::~grupoRecord() {}

void grupoRecord::set_grupo(QString grupo)
{
	setProperty("grupo",grupo);
}

void grupoRecord::set_id(int id)
{
	setProperty("id",id);
}

QString grupoRecord::grupo()
{
	return property("grupo").toString();
}

int grupoRecord::id()
{
	return property("id").toInt();
}

grupoRecord &grupoRecord::operator=(const grupoRecord &other)
{
	clone(other);
	return *this;
}
