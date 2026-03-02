#ifndef BGP_SESION_HACIA_RRRECORD_H
#define BGP_SESION_HACIA_RRRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT bgp_sesion_hacia_rrRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	bgp_sesion_hacia_rrRecord(QObject *qparent=0);
	bgp_sesion_hacia_rrRecord(const bgp_sesion_hacia_rrRecord &other);
	~bgp_sesion_hacia_rrRecord();

	void set_address_family(QString address_family);
	void set_cantidad_sesiones(int cantidad_sesiones);
	void set_cantidad_sesiones_caidas(int cantidad_sesiones_caidas);
	void set_cantidad_sesiones_errores(int cantidad_sesiones_errores);
	void set_fecha(QDate fecha);
	void set_id(int id);
	void set_pais(QString pais);

	QString address_family();
	int cantidad_sesiones();
	int cantidad_sesiones_caidas();
	int cantidad_sesiones_errores();
	QDate fecha();
	int id();
	QString pais();


	bgp_sesion_hacia_rrRecord& operator=(const bgp_sesion_hacia_rrRecord &record);
};

Q_DECLARE_METATYPE(bgp_sesion_hacia_rrRecord*)

#endif
