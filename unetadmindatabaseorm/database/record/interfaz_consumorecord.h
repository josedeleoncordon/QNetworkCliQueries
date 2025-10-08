#ifndef INTERFAZ_CONSUMORECORD_H
#define INTERFAZ_CONSUMORECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT interfaz_consumoRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	interfaz_consumoRecord(QObject *qparent=0);
	interfaz_consumoRecord(const interfaz_consumoRecord &other);
	~interfaz_consumoRecord();

	void set_bandwidth(long long bandwidth);
	void set_descripcion(QString descripcion);
	void set_fecha(QDate fecha);
	void set_grupo(QString grupo);
	void set_id(int id);
	void set_interfaz(QString interfaz);
	void set_pe_ip(QString pe_ip);
	void set_pe_nombre(QString pe_nombre);
	void set_ratein(long long ratein);
	void set_rateout(long long rateout);

	long long bandwidth();
	QString descripcion();
	QDate fecha();
	QString grupo();
	int id();
	QString interfaz();
	QString pe_ip();
	QString pe_nombre();
	long long ratein();
	long long rateout();


	interfaz_consumoRecord& operator=(const interfaz_consumoRecord &record);
};

Q_DECLARE_METATYPE(interfaz_consumoRecord*)

#endif
