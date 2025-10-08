#ifndef BGP_PUBLICACIONES_A_PROVEEDORESRECORD_H
#define BGP_PUBLICACIONES_A_PROVEEDORESRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_publicaciones_a_proveedoresRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_publicaciones_a_proveedoresRecord(QObject *qparent=0);
	bgp_publicaciones_a_proveedoresRecord(const bgp_publicaciones_a_proveedoresRecord &other);
	~bgp_publicaciones_a_proveedoresRecord();

	void set_asn(QString asn);
	void set_comunidad_base(QString comunidad_base);
	void set_grupo(int grupo);
	void set_interface(QString interface);
	void set_pe_hostname(QString pe_hostname);
	void set_pe_ip(QString pe_ip);
	void set_peer_ip(QString peer_ip);
	void set_peer_tipo(QString peer_tipo);
	void set_prepends(QString prepends);
	void set_proveedor(QString proveedor);
	void set_red(QString red);

	QString asn();
	QString comunidad_base();
	int grupo();
	QString interface();
	QString pe_hostname();
	QString pe_ip();
	QString peer_ip();
	QString peer_tipo();
	QString prepends();
	QString proveedor();
	QString red();


	bgp_publicaciones_a_proveedoresRecord& operator=(const bgp_publicaciones_a_proveedoresRecord &record);
};

Q_DECLARE_METATYPE(bgp_publicaciones_a_proveedoresRecord*)

#endif
