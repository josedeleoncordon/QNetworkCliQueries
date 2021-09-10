#ifndef PCP_PAISRECORD_H
#define PCP_PAISRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "pcp_regionrecord.h"

class QANT3MVSHARED_EXPORT pcp_paisRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	pcp_paisRecord(QObject *qparent=0);
	pcp_paisRecord(const pcp_paisRecord &other);
	~pcp_paisRecord();

	void set_id(int id);
	void set_pais(QString pais);
	void set_pcp_region_id(int pcp_region_id);

	int id();
	QString pais();
	int pcp_region_id();

	pcp_regionRecord *r_pcp_region_id();

	pcp_paisRecord& operator=(const pcp_paisRecord &record);
};

Q_DECLARE_METATYPE(pcp_paisRecord*)

#endif
