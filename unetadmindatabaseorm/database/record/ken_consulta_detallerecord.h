#ifndef KEN_CONSULTA_DETALLERECORD_H
#define KEN_CONSULTA_DETALLERECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "ken_equiporecord.h"

class QANT3MVSHARED_EXPORT ken_consulta_detalleRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	ken_consulta_detalleRecord(QObject *qparent=0);
	ken_consulta_detalleRecord(const ken_consulta_detalleRecord &other);
	~ken_consulta_detalleRecord();

	void set_asn(QString asn);
	void set_destino_ip(QString destino_ip);
	void set_fecha(QDate fecha);
	void set_id(long long id);
	void set_ken_equipo_id(int ken_equipo_id);
	void set_max_bps(double max_bps);
	void set_red(QString red);

	QString asn();
	QString destino_ip();
	QDate fecha();
	long long id();
	int ken_equipo_id();
	double max_bps();
	QString red();

	ken_equipoRecord *r_ken_equipo_id();

	ken_consulta_detalleRecord& operator=(const ken_consulta_detalleRecord &record);
};

Q_DECLARE_METATYPE(ken_consulta_detalleRecord*)

#endif
