#ifndef BGP_SERV_PRIN_BCK_COMUNIDADES_FALTANTESRECORD_H
#define BGP_SERV_PRIN_BCK_COMUNIDADES_FALTANTESRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_serv_prin_bck_comunidades_faltantesRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_serv_prin_bck_comunidades_faltantesRecord(QObject *qparent=0);
	bgp_serv_prin_bck_comunidades_faltantesRecord(const bgp_serv_prin_bck_comunidades_faltantesRecord &other);
	~bgp_serv_prin_bck_comunidades_faltantesRecord();

	void set_activo(int activo);
	void set_comunidades_faltantes(QString comunidades_faltantes);
	void set_date(QDate date);
	void set_id(int id);
	void set_neighbor_nexthop(QString neighbor_nexthop);
	void set_pais(QString pais);
	void set_pe_ip(QString pe_ip);
	void set_pe_nombre(QString pe_nombre);
	void set_red(QString red);
	void set_routepolicy(QString routepolicy);
	void set_vrf(QString vrf);

	int activo();
	QString comunidades_faltantes();
	QDate date();
	int id();
	QString neighbor_nexthop();
	QString pais();
	QString pe_ip();
	QString pe_nombre();
	QString red();
	QString routepolicy();
	QString vrf();


	bgp_serv_prin_bck_comunidades_faltantesRecord& operator=(const bgp_serv_prin_bck_comunidades_faltantesRecord &record);
};

Q_DECLARE_METATYPE(bgp_serv_prin_bck_comunidades_faltantesRecord*)

#endif
