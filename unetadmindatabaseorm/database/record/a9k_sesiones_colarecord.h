#ifndef A9K_SESIONES_COLARECORD_H
#define A9K_SESIONES_COLARECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_sesiones_colaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_sesiones_colaRecord(QObject *qparent=0);
	a9k_sesiones_colaRecord(const a9k_sesiones_colaRecord &other);
	~a9k_sesiones_colaRecord();

	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_inq(int inq);
	void set_neighbor(QString neighbor);
	void set_ok(int ok);
	void set_outq(int outq);
	void set_pais(QString pais);
	void set_prefixin(int prefixin);

	QDate date();
	QString detalle();
	QString device_ip();
	QString device_name();
	int inq();
	QString neighbor();
	int ok();
	int outq();
	QString pais();
	int prefixin();


	a9k_sesiones_colaRecord& operator=(const a9k_sesiones_colaRecord &record);
};

Q_DECLARE_METATYPE(a9k_sesiones_colaRecord*)

#endif
