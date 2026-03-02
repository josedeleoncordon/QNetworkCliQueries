#ifndef A9K_SLOTSRECORD_H
#define A9K_SLOTSRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_slotsRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_slotsRecord(QObject *qparent=0);
	a9k_slotsRecord(const a9k_slotsRecord &other);
	~a9k_slotsRecord();

	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_node(QString node);
	void set_ok(int ok);
	void set_pais(QString pais);
	void set_state(QString state);
	void set_type(QString type);

	QDate date();
	QString detalle();
	QString device_ip();
	QString device_name();
	QString node();
	int ok();
	QString pais();
	QString state();
	QString type();


	a9k_slotsRecord& operator=(const a9k_slotsRecord &record);
};

Q_DECLARE_METATYPE(a9k_slotsRecord*)

#endif
