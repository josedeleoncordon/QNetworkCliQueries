#ifndef RED_PAISRECORD_H
#define RED_PAISRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT red_paisRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	red_paisRecord(QObject *qparent=0);
	red_paisRecord(const red_paisRecord &other);
	~red_paisRecord();

	void set_id(int id);
	void set_pais(QString pais);

	int id();
	QString pais();


	red_paisRecord& operator=(const red_paisRecord &record);
};

Q_DECLARE_METATYPE(red_paisRecord*)

#endif
