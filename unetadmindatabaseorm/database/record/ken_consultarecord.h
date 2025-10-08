#ifndef KEN_CONSULTARECORD_H
#define KEN_CONSULTARECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT ken_consultaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	ken_consultaRecord(QObject *qparent=0);
	ken_consultaRecord(const ken_consultaRecord &other);
	~ken_consultaRecord();

	void set_asn(QString asn);
	void set_asn_origen(QString asn_origen);
	void set_destino_ip(QString destino_ip);
	void set_equipo(QString equipo);
	void set_fecha(QDate fecha);
	void set_interface(QString interface);
	void set_interface_salida(QString interface_salida);
	void set_max_bps(double max_bps);
	void set_p95_bps(double p95_bps);
	void set_pe_destino_ip(QString pe_destino_ip);
	void set_pe_destino_ip2(QString pe_destino_ip2);
	void set_pe_ip(QString pe_ip);

	QString asn();
	QString asn_origen();
	QString destino_ip();
	QString equipo();
	QDate fecha();
	QString interface();
	QString interface_salida();
	double max_bps();
	double p95_bps();
	QString pe_destino_ip();
	QString pe_destino_ip2();
	QString pe_ip();


	ken_consultaRecord& operator=(const ken_consultaRecord &record);
};

Q_DECLARE_METATYPE(ken_consultaRecord*)

#endif
