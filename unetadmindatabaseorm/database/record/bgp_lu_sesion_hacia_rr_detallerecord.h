#ifndef BGP_LU_SESION_HACIA_RR_DETALLERECORD_H
#define BGP_LU_SESION_HACIA_RR_DETALLERECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_lu_sesion_hacia_rr_detalleRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_lu_sesion_hacia_rr_detalleRecord(QObject *qparent=0);
	bgp_lu_sesion_hacia_rr_detalleRecord(const bgp_lu_sesion_hacia_rr_detalleRecord &other);
	~bgp_lu_sesion_hacia_rr_detalleRecord();

	void set_detalle(QString detalle);
	void set_estado(QString estado);
	void set_fecha(QDate fecha);
	void set_ok(int ok);
	void set_pais(QString pais);
	void set_pe_ip(QString pe_ip);
	void set_pe_nombre(QString pe_nombre);
	void set_peer_ip(QString peer_ip);
	void set_pfxincount(int pfxincount);
	void set_pfxoutcount(int pfxoutcount);
	void set_rpl_in_ok(int rpl_in_ok);
	void set_rpl_out_ok(int rpl_out_ok);

	QString detalle();
	QString estado();
	QDate fecha();
	int ok();
	QString pais();
	QString pe_ip();
	QString pe_nombre();
	QString peer_ip();
	int pfxincount();
	int pfxoutcount();
	int rpl_in_ok();
	int rpl_out_ok();


	bgp_lu_sesion_hacia_rr_detalleRecord& operator=(const bgp_lu_sesion_hacia_rr_detalleRecord &record);
};

Q_DECLARE_METATYPE(bgp_lu_sesion_hacia_rr_detalleRecord*)

#endif
