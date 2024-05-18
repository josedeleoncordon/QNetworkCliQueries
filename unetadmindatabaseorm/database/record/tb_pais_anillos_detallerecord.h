#ifndef TB_PAIS_ANILLOS_DETALLERECORD_H
#define TB_PAIS_ANILLOS_DETALLERECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT TB_Pais_anillos_detalleRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	TB_Pais_anillos_detalleRecord(QObject *qparent=0);
	TB_Pais_anillos_detalleRecord(const TB_Pais_anillos_detalleRecord &other);
	~TB_Pais_anillos_detalleRecord();

	void set_anillotxt(QString anillotxt);
	void set_fecha(QDate fecha);
	void set_id(unsigned long long id);
	void set_pais(QString pais);

	QString anillotxt();
	QDate fecha();
	unsigned long long id();
	QString pais();


	TB_Pais_anillos_detalleRecord& operator=(const TB_Pais_anillos_detalleRecord &record);
};

Q_DECLARE_METATYPE(TB_Pais_anillos_detalleRecord*)

#endif
