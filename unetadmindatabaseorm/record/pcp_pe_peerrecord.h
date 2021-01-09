#ifndef PCP_PE_PEERRECORD_H
#define PCP_PE_PEERRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "pcp_perecord.h"
#include "pcp_peerrecord.h"

class QANT3MVSHARED_EXPORT pcp_pe_peerRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	pcp_pe_peerRecord(QObject *qparent=0);
	pcp_pe_peerRecord(const pcp_pe_peerRecord &other);
	~pcp_pe_peerRecord();

	void set_id(unsigned int id);
	void set_pcp_pe_id(unsigned int pcp_pe_id);
	void set_pcp_peer_id(unsigned int pcp_peer_id);
	void set_prepend(int prepend);

	unsigned int id();
	unsigned int pcp_pe_id();
	unsigned int pcp_peer_id();
	int prepend();

	pcp_peRecord *r_pcp_pe_id();
	pcp_peerRecord *r_pcp_peer_id();

	pcp_pe_peerRecord& operator=(const pcp_pe_peerRecord &record);
};

Q_DECLARE_METATYPE(pcp_pe_peerRecord*)

#endif
