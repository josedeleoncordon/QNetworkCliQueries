#ifndef BGP_PREFIX_IN_REFRECORD_H
#define BGP_PREFIX_IN_REFRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_prefix_in_refRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_prefix_in_refRecord(QObject *qparent=0);
	bgp_prefix_in_refRecord(const bgp_prefix_in_refRecord &other);
	~bgp_prefix_in_refRecord();

	void set_address_family(QString address_family);
	void set_pe_ip(QString pe_ip);
	void set_peer_ip(QString peer_ip);
	void set_pfxincount(int pfxincount);

	QString address_family();
	QString pe_ip();
	QString peer_ip();
	int pfxincount();


	bgp_prefix_in_refRecord& operator=(const bgp_prefix_in_refRecord &record);
};

Q_DECLARE_METATYPE(bgp_prefix_in_refRecord*)

#endif
