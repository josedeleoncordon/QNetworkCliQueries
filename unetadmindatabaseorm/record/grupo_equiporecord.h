#ifndef GRUPO_EQUIPORECORD_H
#define GRUPO_EQUIPORECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "equiporecord.h"
#include "gruporecord.h"

class QANT3MVSHARED_EXPORT grupo_equipoRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	grupo_equipoRecord(QObject *qparent=0);
	grupo_equipoRecord(const grupo_equipoRecord &other);
	~grupo_equipoRecord();

	void set_equipo_id(unsigned int equipo_id);
	void set_grupo_id(int grupo_id);
	void set_id(int id);
	void set_posicion_x(double posicion_x);
	void set_posicion_y(double posicion_y);

	unsigned int equipo_id();
	int grupo_id();
	int id();
	double posicion_x();
	double posicion_y();

	equipoRecord *r_equipo_id();
	grupoRecord *r_grupo_id();

	grupo_equipoRecord& operator=(const grupo_equipoRecord &record);
};

Q_DECLARE_METATYPE(grupo_equipoRecord*)

#endif
