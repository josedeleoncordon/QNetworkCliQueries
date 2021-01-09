#ifndef AREARECORD_H
#define AREARECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "paisrecord.h"

class QANT3MVSHARED_EXPORT areaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	areaRecord(QObject *qparent=0);
	areaRecord(const areaRecord &other);
	~areaRecord();

	void set_area(QString area);
	void set_id(int id);
	void set_pais_id(int pais_id);

	QString area();
	int id();
	int pais_id();

	paisRecord *r_pais_id();

	areaRecord& operator=(const areaRecord &record);
};

Q_DECLARE_METATYPE(areaRecord*)

#endif
