#ifndef TB_ADDRESSING_CONSULTARECORD_H
#define TB_ADDRESSING_CONSULTARECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT TB_Addressing_consultaRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	TB_Addressing_consultaRecord(QObject *qparent=0);
	TB_Addressing_consultaRecord(const TB_Addressing_consultaRecord &other);
	~TB_Addressing_consultaRecord();

	void set_Fecha(QDate Fecha);
	void set_Hostname(QString Hostname);
	void set_IP(QString IP);
	void set_Interface(QString Interface);
	void set_VRF(QString VRF);
	void set_id(long long id);

	QDate Fecha();
	QString Hostname();
	QString IP();
	QString Interface();
	QString VRF();
	long long id();


	TB_Addressing_consultaRecord& operator=(const TB_Addressing_consultaRecord &record);
};

Q_DECLARE_METATYPE(TB_Addressing_consultaRecord*)

#endif
