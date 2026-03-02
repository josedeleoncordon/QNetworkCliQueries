#ifndef A9K_BGP_SESIONES_WEIGHTRECORD_H
#define A9K_BGP_SESIONES_WEIGHTRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_bgp_sesiones_weightRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_bgp_sesiones_weightRecord(QObject *qparent=0);
	a9k_bgp_sesiones_weightRecord(const a9k_bgp_sesiones_weightRecord &other);
	~a9k_bgp_sesiones_weightRecord();

	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_neighbor(QString neighbor);
	void set_ok(int ok);
	void set_pais(QString pais);
	void set_routepolicy(QString routepolicy);
	void set_vrf(QString vrf);

	QDate date();
	QString detalle();
	QString device_ip();
	QString device_name();
	QString neighbor();
	int ok();
	QString pais();
	QString routepolicy();
	QString vrf();


	a9k_bgp_sesiones_weightRecord& operator=(const a9k_bgp_sesiones_weightRecord &record);
};

Q_DECLARE_METATYPE(a9k_bgp_sesiones_weightRecord*)

#endif
