#ifndef SERVICIOS_HOMOLOGADOSRECORD_H
#define SERVICIOS_HOMOLOGADOSRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT servicios_homologadosRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	servicios_homologadosRecord(QObject *qparent=0);
	servicios_homologadosRecord(const servicios_homologadosRecord &other);
	~servicios_homologadosRecord();

	void set_diagrama_completo(int diagrama_completo);
	void set_estandard_count(int estandard_count);
	void set_fecha(QDate fecha);
	void set_id(long long id);
	void set_pais(QString pais);
	void set_total(int total);

	int diagrama_completo();
	int estandard_count();
	QDate fecha();
	long long id();
	QString pais();
	int total();


	servicios_homologadosRecord& operator=(const servicios_homologadosRecord &record);
};

Q_DECLARE_METATYPE(servicios_homologadosRecord*)

#endif
