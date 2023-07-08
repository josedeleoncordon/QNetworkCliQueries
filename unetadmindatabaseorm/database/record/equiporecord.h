#ifndef EQUIPORECORD_H
#define EQUIPORECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "arearecord.h"

class QANT3MVSHARED_EXPORT equipoRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	equipoRecord(QObject *qparent=0);
	equipoRecord(const equipoRecord &other);
	~equipoRecord();

	void set_area_id(int area_id);
	void set_estado(bool estado);
	void set_id(unsigned int id);
	void set_ip(QString ip);
	void set_nombre(QString nombre);

	int area_id();
	bool estado();
	unsigned int id();
	QString ip();
	QString nombre();

	areaRecord *r_area_id();

	equipoRecord& operator=(const equipoRecord &record);
};

Q_DECLARE_METATYPE(equipoRecord*)

#endif
