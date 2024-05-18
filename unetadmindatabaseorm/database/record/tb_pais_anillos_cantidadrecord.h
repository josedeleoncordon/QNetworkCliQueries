#ifndef TB_PAIS_ANILLOS_CANTIDADRECORD_H
#define TB_PAIS_ANILLOS_CANTIDADRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT TB_Pais_anillos_cantidadRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	TB_Pais_anillos_cantidadRecord(QObject *qparent=0);
	TB_Pais_anillos_cantidadRecord(const TB_Pais_anillos_cantidadRecord &other);
	~TB_Pais_anillos_cantidadRecord();

	void set_cantidad(unsigned int cantidad);
	void set_cantidad_buses(int cantidad_buses);
	void set_fecha(QDate fecha);
	void set_id(unsigned long long id);
	void set_pais(QString pais);

	unsigned int cantidad();
	int cantidad_buses();
	QDate fecha();
	unsigned long long id();
	QString pais();


	TB_Pais_anillos_cantidadRecord& operator=(const TB_Pais_anillos_cantidadRecord &record);
};

Q_DECLARE_METATYPE(TB_Pais_anillos_cantidadRecord*)

#endif
