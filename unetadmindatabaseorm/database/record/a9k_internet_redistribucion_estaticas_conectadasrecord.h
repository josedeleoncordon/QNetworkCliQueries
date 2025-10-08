#ifndef A9K_INTERNET_REDISTRIBUCION_ESTATICAS_CONECTADASRECORD_H
#define A9K_INTERNET_REDISTRIBUCION_ESTATICAS_CONECTADASRECORD_H

#include "qant3_global.h"
#include "recordbase.h"
#include "a9k_equiporecord.h"

class QANT3MVSHARED_EXPORT a9k_internet_redistribucion_estaticas_conectadasRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	a9k_internet_redistribucion_estaticas_conectadasRecord(QObject *qparent=0);
	a9k_internet_redistribucion_estaticas_conectadasRecord(const a9k_internet_redistribucion_estaticas_conectadasRecord &other);
	~a9k_internet_redistribucion_estaticas_conectadasRecord();

	void set_a9k_equipo_id(int a9k_equipo_id);
	void set_date(QDate date);
	void set_descripcion(QString descripcion);
	void set_device_ip(QString device_ip);
	void set_device_name(QString device_name);
	void set_id(int id);
	void set_pais(QString pais);
	void set_routepolicy(QString routepolicy);
	void set_tipo(QString tipo);

	int a9k_equipo_id();
	QDate date();
	QString descripcion();
	QString device_ip();
	QString device_name();
	int id();
	QString pais();
	QString routepolicy();
	QString tipo();

	a9k_equipoRecord *r_a9k_equipo_id();

	a9k_internet_redistribucion_estaticas_conectadasRecord& operator=(const a9k_internet_redistribucion_estaticas_conectadasRecord &record);
};

Q_DECLARE_METATYPE(a9k_internet_redistribucion_estaticas_conectadasRecord*)

#endif
