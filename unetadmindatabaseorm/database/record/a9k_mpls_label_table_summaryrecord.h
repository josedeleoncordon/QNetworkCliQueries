#ifndef A9K_MPLS_LABEL_TABLE_SUMMARYRECORD_H
#define A9K_MPLS_LABEL_TABLE_SUMMARYRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_mpls_label_table_summaryRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_mpls_label_table_summaryRecord(QObject *qparent=0);
	a9k_mpls_label_table_summaryRecord(const a9k_mpls_label_table_summaryRecord &other);
	~a9k_mpls_label_table_summaryRecord();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_date(QDate date);
	void set_etiquetas_asignadas(int etiquetas_asignadas);
	void set_id(int id);

	int a9k_equipo_id();
	QDate date();
	int etiquetas_asignadas();
	int id();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_mpls_label_table_summaryRecord& operator=(const a9k_mpls_label_table_summaryRecord &record);
};

Q_DECLARE_METATYPE(a9k_mpls_label_table_summaryRecord*)

#endif
