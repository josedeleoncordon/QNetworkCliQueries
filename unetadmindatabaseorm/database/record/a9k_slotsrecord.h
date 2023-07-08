#ifndef A9K_SLOTSRECORD_H
#define A9K_SLOTSRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_slotsRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_slotsRecord(QObject *qparent=0);
	a9k_slotsRecord(const a9k_slotsRecord &other);
	~a9k_slotsRecord();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_date(QDate date);
	void set_id(int id);
	void set_node(QString node);
	void set_state(QString state);
	void set_type(QString type);

	int a9k_equipo_id();
	QDate date();
	int id();
	QString node();
	QString state();
	QString type();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_slotsRecord& operator=(const a9k_slotsRecord &record);
};

Q_DECLARE_METATYPE(a9k_slotsRecord*)

#endif
