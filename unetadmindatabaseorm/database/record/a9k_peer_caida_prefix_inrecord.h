#ifndef A9K_PEER_CAIDA_PREFIX_INRECORD_H
#define A9K_PEER_CAIDA_PREFIX_INRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_peer_caida_prefix_inRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_peer_caida_prefix_inRecord(QObject *qparent=0);
	a9k_peer_caida_prefix_inRecord(const a9k_peer_caida_prefix_inRecord &other);
	~a9k_peer_caida_prefix_inRecord();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_date(QDate date);
	void set_id(int id);
	void set_neighbor(QString neighbor);
	void set_prefixin(int prefixin);
	void set_proveedor(QString proveedor);

	int a9k_equipo_id();
	QDate date();
	int id();
	QString neighbor();
	int prefixin();
	QString proveedor();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_peer_caida_prefix_inRecord& operator=(const a9k_peer_caida_prefix_inRecord &record);
};

Q_DECLARE_METATYPE(a9k_peer_caida_prefix_inRecord*)

#endif
