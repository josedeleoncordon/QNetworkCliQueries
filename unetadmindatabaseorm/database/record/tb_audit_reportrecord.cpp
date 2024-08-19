#include "tb_audit_reportrecord.h"

TB_Audit_ReportRecord::TB_Audit_ReportRecord(QObject *qparent):
	RecordBase(nullptr,qparent)
{
	TABLENAME="TB_Audit_Report";
}

TB_Audit_ReportRecord::TB_Audit_ReportRecord(const TB_Audit_ReportRecord &other):
	RecordBase(other.RPARENT,other.parent())
{
	TABLENAME="TB_Audit_Report";
	clone(other);
}

TB_Audit_ReportRecord::~TB_Audit_ReportRecord() {}

void TB_Audit_ReportRecord::set_Category(QString Category)
{
	setProperty("Category",Category);
}

void TB_Audit_ReportRecord::set_Config_Severity(QString Config_Severity)
{
	setProperty("Config_Severity",Config_Severity);
}

void TB_Audit_ReportRecord::set_Country(QString Country)
{
	setProperty("Country",Country);
}

void TB_Audit_ReportRecord::set_Date(QDate Date)
{
	setProperty("Date",Date);
}

void TB_Audit_ReportRecord::set_Description(QString Description)
{
	setProperty("Description",Description);
}

void TB_Audit_ReportRecord::set_Detail(QString Detail)
{
	setProperty("Detail",Detail);
}

void TB_Audit_ReportRecord::set_Hostname(QString Hostname)
{
	setProperty("Hostname",Hostname);
}

void TB_Audit_ReportRecord::set_Network_Severity(QString Network_Severity)
{
	setProperty("Network_Severity",Network_Severity);
}

void TB_Audit_ReportRecord::set_Parameter(QString Parameter)
{
	setProperty("Parameter",Parameter);
}

void TB_Audit_ReportRecord::set_Status(QString Status)
{
	setProperty("Status",Status);
}

void TB_Audit_ReportRecord::set_id(unsigned long long id)
{
	setProperty("id",id);
}

QString TB_Audit_ReportRecord::Category()
{
	return property("Category").toString();
}

QString TB_Audit_ReportRecord::Config_Severity()
{
	return property("Config_Severity").toString();
}

QString TB_Audit_ReportRecord::Country()
{
	return property("Country").toString();
}

QDate TB_Audit_ReportRecord::Date()
{
	return property("Date").toDate();
}

QString TB_Audit_ReportRecord::Description()
{
	return property("Description").toString();
}

QString TB_Audit_ReportRecord::Detail()
{
	return property("Detail").toString();
}

QString TB_Audit_ReportRecord::Hostname()
{
	return property("Hostname").toString();
}

QString TB_Audit_ReportRecord::Network_Severity()
{
	return property("Network_Severity").toString();
}

QString TB_Audit_ReportRecord::Parameter()
{
	return property("Parameter").toString();
}

QString TB_Audit_ReportRecord::Status()
{
	return property("Status").toString();
}

unsigned long long TB_Audit_ReportRecord::id()
{
	return property("id").toULongLong();
}

TB_Audit_ReportRecord &TB_Audit_ReportRecord::operator=(const TB_Audit_ReportRecord &other)
{
	clone(other);
	return *this;
}
