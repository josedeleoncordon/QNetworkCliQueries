#ifndef A9K_BGP_INTER_ASRECORD_H
#define A9K_BGP_INTER_ASRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_bgp_inter_asRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_bgp_inter_asRecord(QObject *qparent=0);
	a9k_bgp_inter_asRecord(const a9k_bgp_inter_asRecord &other);
	~a9k_bgp_inter_asRecord();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_asn(QString asn);
	void set_date(QDate date);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_id(int id);
	void set_neighbor(QString neighbor);
	void set_pais(QString pais);

	int a9k_equipo_id();
	QString asn();
	QDate date();
	QString device_ip();
	QString device_name();
	int id();
	QString neighbor();
	QString pais();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_bgp_inter_asRecord& operator=(const a9k_bgp_inter_asRecord &record);
};

Q_DECLARE_METATYPE(a9k_bgp_inter_asRecord*)

#endif
