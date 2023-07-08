#ifndef INTERFAZ_ESTADORECORD_H
#define INTERFAZ_ESTADORECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "interfazrecord.h"

class QANT3MVSHARED_EXPORT interfaz_estadoRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	interfaz_estadoRecord(QObject *qparent=0);
	interfaz_estadoRecord(const interfaz_estadoRecord &other);
	~interfaz_estadoRecord();

	void set_estado(bool estado);
	void set_fecha(QDate fecha);
	void set_id(unsigned long long id);
	void set_interfaz_id(unsigned int interfaz_id);
	void set_ratein(long long ratein);
	void set_rateout(long long rateout);

	bool estado();
	QDate fecha();
	unsigned long long id();
	unsigned int interfaz_id();
	long long ratein();
	long long rateout();

	interfazRecord *r_interfaz_id();

	interfaz_estadoRecord& operator=(const interfaz_estadoRecord &record);
};

Q_DECLARE_METATYPE(interfaz_estadoRecord*)

#endif
