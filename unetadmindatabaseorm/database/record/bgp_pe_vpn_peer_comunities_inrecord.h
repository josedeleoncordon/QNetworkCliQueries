#ifndef BGP_PE_VPN_PEER_COMUNITIES_INRECORD_H
#define BGP_PE_VPN_PEER_COMUNITIES_INRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_pe_vpn_peer_comunities_inRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_pe_vpn_peer_comunities_inRecord(QObject *qparent=0);
	bgp_pe_vpn_peer_comunities_inRecord(const bgp_pe_vpn_peer_comunities_inRecord &other);
	~bgp_pe_vpn_peer_comunities_inRecord();

	void set_additional_paths_enabled(bool additional_paths_enabled);
	void set_comunidades_faltantes(QString comunidades_faltantes);
	void set_comunidades_pri(QString comunidades_pri);
	void set_comunidades_sec(QString comunidades_sec);
	void set_comunidades_ter(QString comunidades_ter);
	void set_family(QString family);
	void set_fecha(QDate fecha);
	void set_pe_ip(QString pe_ip);
	void set_peer(QString peer);
	void set_prefix_in(int prefix_in);
	void set_rpl_in(QString rpl_in);

	bool additional_paths_enabled();
	QString comunidades_faltantes();
	QString comunidades_pri();
	QString comunidades_sec();
	QString comunidades_ter();
	QString family();
	QDate fecha();
	QString pe_ip();
	QString peer();
	int prefix_in();
	QString rpl_in();


	bgp_pe_vpn_peer_comunities_inRecord& operator=(const bgp_pe_vpn_peer_comunities_inRecord &record);
};

Q_DECLARE_METATYPE(bgp_pe_vpn_peer_comunities_inRecord*)

#endif
