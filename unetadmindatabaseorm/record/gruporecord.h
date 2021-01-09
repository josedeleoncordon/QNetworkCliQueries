#ifndef GRUPORECORD_H
#define GRUPORECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT grupoRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	grupoRecord(QObject *qparent=0);
	grupoRecord(const grupoRecord &other);
	~grupoRecord();

	void set_grupo(QString grupo);
	void set_id(int id);

	QString grupo();
	int id();


	grupoRecord& operator=(const grupoRecord &record);
};

Q_DECLARE_METATYPE(grupoRecord*)

#endif
