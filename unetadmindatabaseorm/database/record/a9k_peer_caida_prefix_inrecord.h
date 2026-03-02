#ifndef A9K_PEER_CAIDA_PREFIX_INRECORD_H
#define A9K_PEER_CAIDA_PREFIX_INRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT a9k_peer_caida_prefix_inRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_peer_caida_prefix_inRecord(QObject *qparent=0);
	a9k_peer_caida_prefix_inRecord(const a9k_peer_caida_prefix_inRecord &other);
	~a9k_peer_caida_prefix_inRecord();

	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_neighbor(QString neighbor);
	void set_ok(int ok);
	void set_pais(QString pais);
	void set_prefixin(int prefixin);
	void set_proveedor(QString proveedor);

	QDate date();
	QString detalle();
	QString device_ip();
	QString device_name();
	QString neighbor();
	int ok();
	QString pais();
	int prefixin();
	QString proveedor();


	a9k_peer_caida_prefix_inRecord& operator=(const a9k_peer_caida_prefix_inRecord &record);
};

Q_DECLARE_METATYPE(a9k_peer_caida_prefix_inRecord*)

#endif
