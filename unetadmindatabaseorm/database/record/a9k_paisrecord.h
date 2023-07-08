#ifndef A9K_PAISRECORD_H
#define A9K_PAISRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_paisRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_paisRecord(QObject *qparent=0);
	a9k_paisRecord(const a9k_paisRecord &other);
	~a9k_paisRecord();

	void set_id(int id);
	void set_pais(QString pais);

	int id();
	QString pais();


	a9k_paisRecord& operator=(const a9k_paisRecord &record);
};

Q_DECLARE_METATYPE(a9k_paisRecord*)

#endif
