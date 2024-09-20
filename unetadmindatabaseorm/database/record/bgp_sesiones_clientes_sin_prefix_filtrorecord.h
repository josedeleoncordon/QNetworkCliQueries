#ifndef BGP_SESIONES_CLIENTES_SIN_PREFIX_FILTRORECORD_H
#define BGP_SESIONES_CLIENTES_SIN_PREFIX_FILTRORECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_sesiones_clientes_sin_prefix_filtroRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_sesiones_clientes_sin_prefix_filtroRecord(QObject *qparent=0);
	bgp_sesiones_clientes_sin_prefix_filtroRecord(const bgp_sesiones_clientes_sin_prefix_filtroRecord &other);
	~bgp_sesiones_clientes_sin_prefix_filtroRecord();

	void set_date(QDate date);
	void set_detalle(QString detalle);
	void set_id(int id);
	void set_neighbor(QString neighbor);
	void set_pais(QString pais);
	void set_pe_ip(QString pe_ip);
	void set_pe_nombre(QString pe_nombre);
	void set_routepolicy(QString routepolicy);

	QDate date();
	QString detalle();
	int id();
	QString neighbor();
	QString pais();
	QString pe_ip();
	QString pe_nombre();
	QString routepolicy();


	bgp_sesiones_clientes_sin_prefix_filtroRecord& operator=(const bgp_sesiones_clientes_sin_prefix_filtroRecord &record);
};

Q_DECLARE_METATYPE(bgp_sesiones_clientes_sin_prefix_filtroRecord*)

#endif
