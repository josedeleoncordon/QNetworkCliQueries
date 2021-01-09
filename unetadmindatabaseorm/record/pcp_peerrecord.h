#ifndef PCP_PEERRECORD_H
#define PCP_PEERRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "pcp_paisrecord.h"
#include "pcp_peer_tiporecord.h"
#include "pcp_proveedorrecord.h"

class QANT3MVSHARED_EXPORT pcp_peerRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	pcp_peerRecord(QObject *qparent=0);
	pcp_peerRecord(const pcp_peerRecord &other);
	~pcp_peerRecord();

	void set_comunidad_base(QString comunidad_base);
	void set_estado(bool estado);
	void set_id(unsigned int id);
	void set_ip_pe(QString ip_pe);
	void set_ip_peer(QString ip_peer);
	void set_pcp_pais_id(int pcp_pais_id);
	void set_pcp_peer_tipo_id(int pcp_peer_tipo_id);
	void set_pcp_proveedor_id(unsigned int pcp_proveedor_id);
	void set_vrf(QString vrf);

	QString comunidad_base();
	bool estado();
	unsigned int id();
	QString ip_pe();
	QString ip_peer();
	int pcp_pais_id();
	int pcp_peer_tipo_id();
	unsigned int pcp_proveedor_id();
	QString vrf();

	pcp_paisRecord *r_pcp_pais_id();
	pcp_peer_tipoRecord *r_pcp_peer_tipo_id();
	pcp_proveedorRecord *r_pcp_proveedor_id();

	pcp_peerRecord& operator=(const pcp_peerRecord &record);
};

Q_DECLARE_METATYPE(pcp_peerRecord*)

#endif
