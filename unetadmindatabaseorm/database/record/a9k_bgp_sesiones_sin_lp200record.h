#ifndef A9K_BGP_SESIONES_SIN_LP200RECORD_H
#define A9K_BGP_SESIONES_SIN_LP200RECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_bgp_sesiones_sin_lp200Record: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_bgp_sesiones_sin_lp200Record(QObject *qparent=0);
	a9k_bgp_sesiones_sin_lp200Record(const a9k_bgp_sesiones_sin_lp200Record &other);
	~a9k_bgp_sesiones_sin_lp200Record();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_date(QDate date);
	void set_id(int id);
	void set_neighbor(QString neighbor);
	void set_prefixin(int prefixin);
	void set_routepolicy(QString routepolicy);

	int a9k_equipo_id();
	QDate date();
	int id();
	QString neighbor();
	int prefixin();
	QString routepolicy();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_bgp_sesiones_sin_lp200Record& operator=(const a9k_bgp_sesiones_sin_lp200Record &record);
};

Q_DECLARE_METATYPE(a9k_bgp_sesiones_sin_lp200Record*)

#endif
