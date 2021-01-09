#ifndef PAISRECORD_H
#define PAISRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT paisRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	paisRecord(QObject *qparent=0);
	paisRecord(const paisRecord &other);
	~paisRecord();

	void set_id(int id);
	void set_pais(QString pais);

	int id();
	QString pais();


	paisRecord& operator=(const paisRecord &record);
};

Q_DECLARE_METATYPE(paisRecord*)

#endif
