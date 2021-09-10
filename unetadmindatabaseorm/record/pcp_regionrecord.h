#ifndef PCP_REGIONRECORD_H
#define PCP_REGIONRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT pcp_regionRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	pcp_regionRecord(QObject *qparent=0);
	pcp_regionRecord(const pcp_regionRecord &other);
	~pcp_regionRecord();

	void set_id(int id);
	void set_region(QString region);

	int id();
	QString region();


	pcp_regionRecord& operator=(const pcp_regionRecord &record);
};

Q_DECLARE_METATYPE(pcp_regionRecord*)

#endif
