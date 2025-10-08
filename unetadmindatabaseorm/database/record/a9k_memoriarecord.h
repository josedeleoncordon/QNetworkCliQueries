#ifndef A9K_MEMORIARECORD_H
#define A9K_MEMORIARECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_memoriaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_memoriaRecord(QObject *qparent=0);
	a9k_memoriaRecord(const a9k_memoriaRecord &other);
	~a9k_memoriaRecord();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_app_disponible(int app_disponible);
	void set_bgp(int bgp);
	void set_bgp_limit(int bgp_limit);
	void set_date(QDate date);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_fisica_disponible(int fisica_disponible);
	void set_fisica_total(int fisica_total);
	void set_id(int id);
	void set_pais(QString pais);

	int a9k_equipo_id();
	int app_disponible();
	int bgp();
	int bgp_limit();
	QDate date();
	QString device_ip();
	QString device_name();
	int fisica_disponible();
	int fisica_total();
	int id();
	QString pais();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_memoriaRecord& operator=(const a9k_memoriaRecord &record);
};

Q_DECLARE_METATYPE(a9k_memoriaRecord*)

#endif
