#ifndef KEN_EQUIPORECORD_H
#define KEN_EQUIPORECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT ken_equipoRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	ken_equipoRecord(QObject *qparent=0);
	ken_equipoRecord(const ken_equipoRecord &other);
	~ken_equipoRecord();

	void set_id(int id);
	void set_ip(QString ip);
	void set_nombre(QString nombre);

	int id();
	QString ip();
	QString nombre();


	ken_equipoRecord& operator=(const ken_equipoRecord &record);
};

Q_DECLARE_METATYPE(ken_equipoRecord*)

#endif
