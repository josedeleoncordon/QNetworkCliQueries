#ifndef A9K_BGP_INTER_ASRECORD_H
#define A9K_BGP_INTER_ASRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_bgp_inter_asRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_bgp_inter_asRecord(QObject *qparent=0);
	a9k_bgp_inter_asRecord(const a9k_bgp_inter_asRecord &other);
	~a9k_bgp_inter_asRecord();

	void set_asn(QString asn);
	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_neighbor(QString neighbor);
	void set_ok(int ok);
	void set_pais(QString pais);

	QString asn();
	QDate date();
	QString detalle();
	QString device_ip();
	QString device_name();
	QString neighbor();
	int ok();
	QString pais();


	a9k_bgp_inter_asRecord& operator=(const a9k_bgp_inter_asRecord &record);
};

Q_DECLARE_METATYPE(a9k_bgp_inter_asRecord*)

#endif
