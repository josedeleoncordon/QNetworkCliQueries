#ifndef BGP_VPNV_NHS_RRCLIENTRECORD_H
#define BGP_VPNV_NHS_RRCLIENTRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_vpnv_nhs_rrclientRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_vpnv_nhs_rrclientRecord(QObject *qparent=0);
	bgp_vpnv_nhs_rrclientRecord(const bgp_vpnv_nhs_rrclientRecord &other);
	~bgp_vpnv_nhs_rrclientRecord();

	void set_address_family(QString address_family);
	void set_fecha(QDate fecha);
	void set_ok(int ok);
	void set_pais(QString pais);
	void set_pe_ip(QString pe_ip);
	void set_pe_nombre(QString pe_nombre);
	void set_peer(QString peer);

	QString address_family();
	QDate fecha();
	int ok();
	QString pais();
	QString pe_ip();
	QString pe_nombre();
	QString peer();


	bgp_vpnv_nhs_rrclientRecord& operator=(const bgp_vpnv_nhs_rrclientRecord &record);
};

Q_DECLARE_METATYPE(bgp_vpnv_nhs_rrclientRecord*)

#endif
