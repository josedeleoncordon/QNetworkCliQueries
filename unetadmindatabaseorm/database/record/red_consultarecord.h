#ifndef RED_CONSULTARECORD_H
#define RED_CONSULTARECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "red_sectorrecord.h"

class QANT3MVSHARED_EXPORT red_consultaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	red_consultaRecord(QObject *qparent=0);
	red_consultaRecord(const red_consultaRecord &other);
	~red_consultaRecord();

	void set_conexion_error(int conexion_error);
	void set_excepcion(int excepcion);
	void set_fecha(QDate fecha);
	void set_id(int id);
	void set_no(int no);
	void set_nohomologado(int nohomologado);
	void set_noservicio(int noservicio);
	void set_ok(int ok);
	void set_principalcaido(int principalcaido);
	void set_red_sector_id(int red_sector_id);
	void set_total(int total);

	int conexion_error();
	int excepcion();
	QDate fecha();
	int id();
	int no();
	int nohomologado();
	int noservicio();
	int ok();
	int principalcaido();
	int red_sector_id();
	int total();

	red_sectorRecord *r_red_sector_id();

	red_consultaRecord& operator=(const red_consultaRecord &record);
};

Q_DECLARE_METATYPE(red_consultaRecord*)

#endif
