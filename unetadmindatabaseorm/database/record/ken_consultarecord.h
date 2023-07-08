#ifndef KEN_CONSULTARECORD_H
#define KEN_CONSULTARECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "ken_equiporecord.h"

class QANT3MVSHARED_EXPORT ken_consultaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	ken_consultaRecord(QObject *qparent=0);
	ken_consultaRecord(const ken_consultaRecord &other);
	~ken_consultaRecord();

	void set_destino_ip(QString destino_ip);
	void set_fecha(QDate fecha);
	void set_id(long long id);
	void set_ken_equipo_id(int ken_equipo_id);
	void set_max_bps(double max_bps);

	QString destino_ip();
	QDate fecha();
	long long id();
	int ken_equipo_id();
	double max_bps();

	ken_equipoRecord *r_ken_equipo_id();

	ken_consultaRecord& operator=(const ken_consultaRecord &record);
};

Q_DECLARE_METATYPE(ken_consultaRecord*)

#endif
