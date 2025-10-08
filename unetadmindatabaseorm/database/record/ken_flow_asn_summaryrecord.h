#ifndef KEN_FLOW_ASN_SUMMARYRECORD_H
#define KEN_FLOW_ASN_SUMMARYRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT ken_flow_asn_summaryRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	ken_flow_asn_summaryRecord(QObject *qparent=0);
	ken_flow_asn_summaryRecord(const ken_flow_asn_summaryRecord &other);
	~ken_flow_asn_summaryRecord();

	void set_asn_destino(QString asn_destino);
	void set_asn_origen(QString asn_origen);
	void set_equipo(QString equipo);
	void set_fecha(QDate fecha);
	void set_max_bps(double max_bps);
	void set_p95_bps(double p95_bps);
	void set_pe_ip(QString pe_ip);
	void set_source_connectivity_type(QString source_connectivity_type);

	QString asn_destino();
	QString asn_origen();
	QString equipo();
	QDate fecha();
	double max_bps();
	double p95_bps();
	QString pe_ip();
	QString source_connectivity_type();


	ken_flow_asn_summaryRecord& operator=(const ken_flow_asn_summaryRecord &record);
};

Q_DECLARE_METATYPE(ken_flow_asn_summaryRecord*)

#endif
