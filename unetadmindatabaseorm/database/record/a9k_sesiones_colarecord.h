#ifndef A9K_SESIONES_COLARECORD_H
#define A9K_SESIONES_COLARECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_sesiones_colaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_sesiones_colaRecord(QObject *qparent=0);
	a9k_sesiones_colaRecord(const a9k_sesiones_colaRecord &other);
	~a9k_sesiones_colaRecord();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_date(QDate date);
	void set_id(int id);
	void set_inq(int inq);
	void set_neighbor(QString neighbor);
	void set_outq(int outq);

	int a9k_equipo_id();
	QDate date();
	int id();
	int inq();
	QString neighbor();
	int outq();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_sesiones_colaRecord& operator=(const a9k_sesiones_colaRecord &record);
};

Q_DECLARE_METATYPE(a9k_sesiones_colaRecord*)

#endif
