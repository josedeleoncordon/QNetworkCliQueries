#ifndef A9K_MEMORIARECORD_H
#define A9K_MEMORIARECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_memoriaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_memoriaRecord(QObject *qparent=0);
	a9k_memoriaRecord(const a9k_memoriaRecord &other);
	~a9k_memoriaRecord();

	void set_app_disponible(int app_disponible);
	void set_bgp(int bgp);
	void set_bgp_limit(int bgp_limit);
	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_fisica_disponible(int fisica_disponible);
	void set_fisica_total(int fisica_total);
	void set_ok(int ok);
	void set_pais(QString pais);

	int app_disponible();
	int bgp();
	int bgp_limit();
	QDate date();
	QString detalle();
	QString device_ip();
	QString device_name();
	int fisica_disponible();
	int fisica_total();
	int ok();
	QString pais();


	a9k_memoriaRecord& operator=(const a9k_memoriaRecord &record);
};

Q_DECLARE_METATYPE(a9k_memoriaRecord*)

#endif
