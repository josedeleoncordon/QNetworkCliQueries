#ifndef BGP_PE_RRCLIENT_HACIA_ARRIBARECORD_H
#define BGP_PE_RRCLIENT_HACIA_ARRIBARECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_pe_rrclient_hacia_arribaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_pe_rrclient_hacia_arribaRecord(QObject *qparent=0);
	bgp_pe_rrclient_hacia_arribaRecord(const bgp_pe_rrclient_hacia_arribaRecord &other);
	~bgp_pe_rrclient_hacia_arribaRecord();

	void set_fecha(QDate fecha);
	void set_ok(int ok);
	void set_pais(QString pais);
	void set_pe_ip(QString pe_ip);
	void set_pe_nombre(QString pe_nombre);
	void set_peer(QString peer);

	QDate fecha();
	int ok();
	QString pais();
	QString pe_ip();
	QString pe_nombre();
	QString peer();


	bgp_pe_rrclient_hacia_arribaRecord& operator=(const bgp_pe_rrclient_hacia_arribaRecord &record);
};

Q_DECLARE_METATYPE(bgp_pe_rrclient_hacia_arribaRecord*)

#endif
