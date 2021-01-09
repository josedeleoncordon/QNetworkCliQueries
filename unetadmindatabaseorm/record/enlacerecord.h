#ifndef ENLACERECORD_H
#define ENLACERECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "interfazrecord.h"
#include "interfazrecord.h"

class QANT3MVSHARED_EXPORT enlaceRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	enlaceRecord(QObject *qparent=0);
	enlaceRecord(const enlaceRecord &other);
	~enlaceRecord();

	void set_destino_interfaz_id(unsigned int destino_interfaz_id);
	void set_estado(bool estado);
	void set_etiqueta(QString etiqueta);
	void set_id(unsigned long long id);
	void set_origen_interfaz_id(unsigned int origen_interfaz_id);

	unsigned int destino_interfaz_id();
	bool estado();
	QString etiqueta();
	unsigned long long id();
	unsigned int origen_interfaz_id();

	interfazRecord *r_destino_interfaz_id();
	interfazRecord *r_origen_interfaz_id();

	enlaceRecord& operator=(const enlaceRecord &record);
};

Q_DECLARE_METATYPE(enlaceRecord*)

#endif
