#ifndef INTERFAZ_TIPORECORD_H
#define INTERFAZ_TIPORECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT interfaz_tipoRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	interfaz_tipoRecord(QObject *qparent=0);
	interfaz_tipoRecord(const interfaz_tipoRecord &other);
	~interfaz_tipoRecord();

	void set_id(int id);
	void set_tipo(QString tipo);

	int id();
	QString tipo();


	interfaz_tipoRecord& operator=(const interfaz_tipoRecord &record);
};

Q_DECLARE_METATYPE(interfaz_tipoRecord*)

#endif
