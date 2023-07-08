#include "a9k_mpls_label_table_summaryrecord.h"

a9k_mpls_label_table_summaryRecord::a9k_mpls_label_table_summaryRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="a9k_mpls_label_table_summary";
}

a9k_mpls_label_table_summaryRecord::a9k_mpls_label_table_summaryRecord(const a9k_mpls_label_table_summaryRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="a9k_mpls_label_table_summary";
	clone(other);
}

a9k_mpls_label_table_summaryRecord::~a9k_mpls_label_table_summaryRecord() {}

void a9k_mpls_label_table_summaryRecord::set_a9k_equipo_id(int a9k_equipo_id)
{
	setProperty("a9k_equipo_id",a9k_equipo_id);
}

void a9k_mpls_label_table_summaryRecord::set_date(QDate date)
{
	setProperty("date",date);
}

void a9k_mpls_label_table_summaryRecord::set_etiquetas_asignadas(int etiquetas_asignadas)
{
	setProperty("etiquetas_asignadas",etiquetas_asignadas);
}

void a9k_mpls_label_table_summaryRecord::set_id(int id)
{
	setProperty("id",id);
}

int a9k_mpls_label_table_summaryRecord::a9k_equipo_id()
{
	return property("a9k_equipo_id").toInt();
}

QDate a9k_mpls_label_table_summaryRecord::date()
{
	return property("date").toDate();
}

int a9k_mpls_label_table_summaryRecord::etiquetas_asignadas()
{
	return property("etiquetas_asignadas").toInt();
}

int a9k_mpls_label_table_summaryRecord::id()
{
	return property("id").toInt();
}

a9k_equipoRecord *a9k_mpls_label_table_summaryRecord::r_a9k_equipo_id()
{
	return dynamic_cast<a9k_equipoRecord*>( property("r_a9k_equipo_id").value<RecordBase*>() );
}

a9k_mpls_label_table_summaryRecord &a9k_mpls_label_table_summaryRecord::operator=(const a9k_mpls_label_table_summaryRecord &other)
{
	clone(other);
	return *this;
}
