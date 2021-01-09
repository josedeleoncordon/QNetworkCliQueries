#ifndef PCP_PERECORD_H
#define PCP_PERECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT pcp_peRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	pcp_peRecord(QObject *qparent=0);
	pcp_peRecord(const pcp_peRecord &other);
	~pcp_peRecord();

	void set_id(unsigned int id);
	void set_ip(QString ip);
	void set_nombre(QString nombre);

	unsigned int id();
	QString ip();
	QString nombre();


	pcp_peRecord& operator=(const pcp_peRecord &record);
};

Q_DECLARE_METATYPE(pcp_peRecord*)

#endif
