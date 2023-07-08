#ifndef PCP_PEER_TIPORECORD_H
#define PCP_PEER_TIPORECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT pcp_peer_tipoRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	pcp_peer_tipoRecord(QObject *qparent=0);
	pcp_peer_tipoRecord(const pcp_peer_tipoRecord &other);
	~pcp_peer_tipoRecord();

	void set_id(int id);
	void set_tipo(QString tipo);

	int id();
	QString tipo();


	pcp_peer_tipoRecord& operator=(const pcp_peer_tipoRecord &record);
};

Q_DECLARE_METATYPE(pcp_peer_tipoRecord*)

#endif
