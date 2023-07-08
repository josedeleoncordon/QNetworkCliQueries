#ifndef A9K_VRF_SIN_LABEL_PERVRFRECORD_H
#define A9K_VRF_SIN_LABEL_PERVRFRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_vrf_sin_label_pervrfRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_vrf_sin_label_pervrfRecord(QObject *qparent=0);
	a9k_vrf_sin_label_pervrfRecord(const a9k_vrf_sin_label_pervrfRecord &other);
	~a9k_vrf_sin_label_pervrfRecord();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_address_family(QString address_family);
	void set_date(QDate date);
	void set_id(int id);
	void set_vrf(QString vrf);

	int a9k_equipo_id();
	QString address_family();
	QDate date();
	int id();
	QString vrf();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_vrf_sin_label_pervrfRecord& operator=(const a9k_vrf_sin_label_pervrfRecord &record);
};

Q_DECLARE_METATYPE(a9k_vrf_sin_label_pervrfRecord*)

#endif
