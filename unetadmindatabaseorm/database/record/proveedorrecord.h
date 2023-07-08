#ifndef PROVEEDORRECORD_H
#define PROVEEDORRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT proveedorRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	proveedorRecord(QObject *qparent=0);
	proveedorRecord(const proveedorRecord &other);
	~proveedorRecord();

	void set_id(QString id);
	void set_proveedor(QString proveedor);

	QString id();
	QString proveedor();


	proveedorRecord& operator=(const proveedorRecord &record);
};

Q_DECLARE_METATYPE(proveedorRecord*)

#endif
