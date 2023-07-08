#ifndef A9K_EQUIPORECORD_H
#define A9K_EQUIPORECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_paisrecord.h"

class QANT3MVSHARED_EXPORT a9k_equipoRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_equipoRecord(QObject *qparent=0);
	a9k_equipoRecord(const a9k_equipoRecord &other);
	~a9k_equipoRecord();

	void set_IP(QString IP);
	void set_a9k_pais_id(int a9k_pais_id);
	void set_id(int id);
	void set_nombre(QString nombre);

	QString IP();
	int a9k_pais_id();
	int id();
	QString nombre();

	a9k_paisRecord *r_a9k_pais_id();

	a9k_equipoRecord& operator=(const a9k_equipoRecord &record);
};

Q_DECLARE_METATYPE(a9k_equipoRecord*)

#endif
