#ifndef A9K_BGP_SESIONES_SIN_LP200RECORD_H
#define A9K_BGP_SESIONES_SIN_LP200RECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_bgp_sesiones_sin_lp200Record: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_bgp_sesiones_sin_lp200Record(QObject *qparent=0);
	a9k_bgp_sesiones_sin_lp200Record(const a9k_bgp_sesiones_sin_lp200Record &other);
	~a9k_bgp_sesiones_sin_lp200Record();

	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_neighbor(QString neighbor);
	void set_ok(int ok);
	void set_pais(QString pais);
	void set_prefixin(int prefixin);
	void set_routepolicy(QString routepolicy);

	QDate date();
	QString detalle();
	QString device_ip();
	QString device_name();
	QString neighbor();
	int ok();
	QString pais();
	int prefixin();
	QString routepolicy();


	a9k_bgp_sesiones_sin_lp200Record& operator=(const a9k_bgp_sesiones_sin_lp200Record &record);
};

Q_DECLARE_METATYPE(a9k_bgp_sesiones_sin_lp200Record*)

#endif
