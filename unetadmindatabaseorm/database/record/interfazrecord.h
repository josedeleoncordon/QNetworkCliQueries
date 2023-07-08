#ifndef INTERFAZRECORD_H
#define INTERFAZRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "equiporecord.h"
#include "interfaz_tiporecord.h"
#include "proveedorrecord.h"

class QANT3MVSHARED_EXPORT interfazRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	interfazRecord(QObject *qparent=0);
	interfazRecord(const interfazRecord &other);
	~interfazRecord();

	void set_bandwidth(long long bandwidth);
	void set_descripcion(QString descripcion);
	void set_equipo_id(unsigned int equipo_id);
	void set_estado(bool estado);
	void set_id(unsigned int id);
	void set_interfaz(QString interfaz);
	void set_interfaz_tipo_id(int interfaz_tipo_id);
	void set_ospfcosto(int ospfcosto);
	void set_proveedor_id(QString proveedor_id);

	long long bandwidth();
	QString descripcion();
	unsigned int equipo_id();
	bool estado();
	unsigned int id();
	QString interfaz();
	int interfaz_tipo_id();
	int ospfcosto();
	QString proveedor_id();

	equipoRecord *r_equipo_id();
	interfaz_tipoRecord *r_interfaz_tipo_id();
	proveedorRecord *r_proveedor_id();

	interfazRecord& operator=(const interfazRecord &record);
};

Q_DECLARE_METATYPE(interfazRecord*)

#endif
