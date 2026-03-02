#ifndef A9K_MPLS_LABEL_TABLE_SUMMARYRECORD_H
#define A9K_MPLS_LABEL_TABLE_SUMMARYRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_mpls_label_table_summaryRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_mpls_label_table_summaryRecord(QObject *qparent=0);
	a9k_mpls_label_table_summaryRecord(const a9k_mpls_label_table_summaryRecord &other);
	~a9k_mpls_label_table_summaryRecord();

	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_etiquetas_asignadas(int etiquetas_asignadas);
	void set_ok(int ok);
	void set_pais(QString pais);

	QDate date();
	QString detalle();
	QString device_ip();
	QString device_name();
	int etiquetas_asignadas();
	int ok();
	QString pais();


	a9k_mpls_label_table_summaryRecord& operator=(const a9k_mpls_label_table_summaryRecord &record);
};

Q_DECLARE_METATYPE(a9k_mpls_label_table_summaryRecord*)

#endif
