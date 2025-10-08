#ifndef SERVICIOS_BW_VS_CRMRECORD_H
#define SERVICIOS_BW_VS_CRMRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT servicios_bw_vs_crmRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	servicios_bw_vs_crmRecord(QObject *qparent=0);
	servicios_bw_vs_crmRecord(const servicios_bw_vs_crmRecord &other);
	~servicios_bw_vs_crmRecord();

	void set_fecha(QDate fecha);
	void set_id(long long id);
	void set_pais(QString pais);
	void set_porcentaje(double porcentaje);

	QDate fecha();
	long long id();
	QString pais();
	double porcentaje();


	servicios_bw_vs_crmRecord& operator=(const servicios_bw_vs_crmRecord &record);
};

Q_DECLARE_METATYPE(servicios_bw_vs_crmRecord*)

#endif
