#ifndef TB_PAIS_ANILLO_BUS_DETALLERECORD_H
#define TB_PAIS_ANILLO_BUS_DETALLERECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT TB_Pais_anillo_bus_detalleRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	TB_Pais_anillo_bus_detalleRecord(QObject *qparent=0);
	TB_Pais_anillo_bus_detalleRecord(const TB_Pais_anillo_bus_detalleRecord &other);
	~TB_Pais_anillo_bus_detalleRecord();

	void set_con_reporte(bool con_reporte);
	void set_fecha(QDate fecha);
	void set_hostname(QString hostname);
	void set_id(unsigned long long id);
	void set_ip(QString ip);
	void set_local_port(QString local_port);
	void set_local_port_description(QString local_port_description);
	void set_pais(QString pais);
	void set_remote_host(QString remote_host);
	void set_remote_port(QString remote_port);
	void set_tipo(QString tipo);
	void set_txt(QString txt);

	bool con_reporte();
	QDate fecha();
	QString hostname();
	unsigned long long id();
	QString ip();
	QString local_port();
	QString local_port_description();
	QString pais();
	QString remote_host();
	QString remote_port();
	QString tipo();
	QString txt();


	TB_Pais_anillo_bus_detalleRecord& operator=(const TB_Pais_anillo_bus_detalleRecord &record);
};

Q_DECLARE_METATYPE(TB_Pais_anillo_bus_detalleRecord*)

#endif
