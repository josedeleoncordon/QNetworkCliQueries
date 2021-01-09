#ifndef PCP_PROVEEDORRECORD_H
#define PCP_PROVEEDORRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT pcp_proveedorRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	pcp_proveedorRecord(QObject *qparent=0);
	pcp_proveedorRecord(const pcp_proveedorRecord &other);
	~pcp_proveedorRecord();

	void set_id(unsigned int id);
	void set_logo_path(QString logo_path);
	void set_proveedor(QString proveedor);

	unsigned int id();
	QString logo_path();
	QString proveedor();


	pcp_proveedorRecord& operator=(const pcp_proveedorRecord &record);
};

Q_DECLARE_METATYPE(pcp_proveedorRecord*)

#endif
