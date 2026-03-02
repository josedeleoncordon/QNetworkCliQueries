#ifndef A9K_VRF_SIN_LABEL_PERVRFRECORD_H
#define A9K_VRF_SIN_LABEL_PERVRFRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_vrf_sin_label_pervrfRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_vrf_sin_label_pervrfRecord(QObject *qparent=0);
	a9k_vrf_sin_label_pervrfRecord(const a9k_vrf_sin_label_pervrfRecord &other);
	~a9k_vrf_sin_label_pervrfRecord();

	void set_address_family(QString address_family);
	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_ok(int ok);
	void set_pais(QString pais);
	void set_vrf(QString vrf);

	QString address_family();
	QDate date();
	QString detalle();
	QString device_ip();
	QString device_name();
	int ok();
	QString pais();
	QString vrf();


	a9k_vrf_sin_label_pervrfRecord& operator=(const a9k_vrf_sin_label_pervrfRecord &record);
};

Q_DECLARE_METATYPE(a9k_vrf_sin_label_pervrfRecord*)

#endif
