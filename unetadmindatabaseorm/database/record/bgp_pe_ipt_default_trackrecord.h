#ifndef BGP_PE_IPT_DEFAULT_TRACKRECORD_H
#define BGP_PE_IPT_DEFAULT_TRACKRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_pe_ipt_default_trackRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_pe_ipt_default_trackRecord(QObject *qparent=0);
	bgp_pe_ipt_default_trackRecord(const bgp_pe_ipt_default_trackRecord &other);
	~bgp_pe_ipt_default_trackRecord();

	void set_default_hacia_peer(int default_hacia_peer);
	void set_detalle(QString detalle);
	void set_fecha(QDate fecha);
	void set_ok(int ok);
	void set_pais(QString pais);
	void set_pe_ip(QString pe_ip);
	void set_pe_nombre(QString pe_nombre);
	void set_peer(QString peer);
	void set_track(QString track);
	void set_track_ok(int track_ok);

	int default_hacia_peer();
	QString detalle();
	QDate fecha();
	int ok();
	QString pais();
	QString pe_ip();
	QString pe_nombre();
	QString peer();
	QString track();
	int track_ok();


	bgp_pe_ipt_default_trackRecord& operator=(const bgp_pe_ipt_default_trackRecord &record);
};

Q_DECLARE_METATYPE(bgp_pe_ipt_default_trackRecord*)

#endif
