#ifndef RED_SECTORRECORD_H
#define RED_SECTORRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "red_paisrecord.h"

class QANT3MVSHARED_EXPORT red_sectorRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	red_sectorRecord(QObject *qparent=0);
	red_sectorRecord(const red_sectorRecord &other);
	~red_sectorRecord();

	void set_id(int id);
	void set_red_pais_id(int red_pais_id);
	void set_sector(QString sector);

	int id();
	int red_pais_id();
	QString sector();

	red_paisRecord *r_red_pais_id();

	red_sectorRecord& operator=(const red_sectorRecord &record);
};

Q_DECLARE_METATYPE(red_sectorRecord*)

#endif
