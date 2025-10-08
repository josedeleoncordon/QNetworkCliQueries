#ifndef RED_CONSULTA_DETALLERECORD_H
#define RED_CONSULTA_DETALLERECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "red_sectorrecord.h"

class QANT3MVSHARED_EXPORT red_consulta_detalleRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	red_consulta_detalleRecord(QObject *qparent=0);
	red_consulta_detalleRecord(const red_consulta_detalleRecord &other);
	~red_consulta_detalleRecord();

	void set_Descripcion(QString Descripcion);
	void set_Detalle(QString Detalle);
	void set_IP(QString IP);
	void set_Principal_Interface(QString Principal_Interface);
	void set_Principal_PE(QString Principal_PE);
	void set_Redundancia(QString Redundancia);
	void set_Remoto_IP(QString Remoto_IP);
	void set_Remoto_Interface(QString Remoto_Interface);
	void set_Respaldo_Interface(QString Respaldo_Interface);
	void set_Respaldo_PE(QString Respaldo_PE);
	void set_VCID_Principal(QString VCID_Principal);
	void set_VCID_Principal_Estado(QString VCID_Principal_Estado);
	void set_VCID_Respaldo(QString VCID_Respaldo);
	void set_VCID_Respaldo_Estado(QString VCID_Respaldo_Estado);
	void set_VRF(QString VRF);
	void set_advertencia(QString advertencia);
	void set_bgp_ok(bool bgp_ok);
	void set_con_bgp(bool con_bgp);
	void set_fecha(QDate fecha);
	void set_id(long long id);
	void set_red_sector_id(int red_sector_id);

	QString Descripcion();
	QString Detalle();
	QString IP();
	QString Principal_Interface();
	QString Principal_PE();
	QString Redundancia();
	QString Remoto_IP();
	QString Remoto_Interface();
	QString Respaldo_Interface();
	QString Respaldo_PE();
	QString VCID_Principal();
	QString VCID_Principal_Estado();
	QString VCID_Respaldo();
	QString VCID_Respaldo_Estado();
	QString VRF();
	QString advertencia();
	bool bgp_ok();
	bool con_bgp();
	QDate fecha();
	long long id();
	int red_sector_id();

	red_sectorRecord *r_red_sector_id();

	red_consulta_detalleRecord& operator=(const red_consulta_detalleRecord &record);
};

Q_DECLARE_METATYPE(red_consulta_detalleRecord*)

#endif
