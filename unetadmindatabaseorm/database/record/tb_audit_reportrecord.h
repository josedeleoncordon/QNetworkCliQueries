#ifndef TB_AUDIT_REPORTRECORD_H
#define TB_AUDIT_REPORTRECORD_H

#include "qant3_global.h"
#include "recordbase.h"

class QANT3MVSHARED_EXPORT TB_Audit_ReportRecord: public QAnt::Dao::RecordBase
{
	Q_OBJECT

protected:

	void initValues();

public:
	TB_Audit_ReportRecord(QObject *qparent=0);
	TB_Audit_ReportRecord(const TB_Audit_ReportRecord &other);
	~TB_Audit_ReportRecord();

	void set_Category(QString Category);
	void set_Config_Severity(QString Config_Severity);
	void set_Country(QString Country);
	void set_Date(QDate Date);
	void set_Description(QString Description);
	void set_Detail(QString Detail);
	void set_Hostname(QString Hostname);
	void set_Network_Severity(QString Network_Severity);
	void set_Parameter(QString Parameter);
	void set_Status(QString Status);
	void set_id(unsigned long long id);

	QString Category();
	QString Config_Severity();
	QString Country();
	QDate Date();
	QString Description();
	QString Detail();
	QString Hostname();
	QString Network_Severity();
	QString Parameter();
	QString Status();
	unsigned long long id();


	TB_Audit_ReportRecord& operator=(const TB_Audit_ReportRecord &record);
};

Q_DECLARE_METATYPE(TB_Audit_ReportRecord*)

#endif
