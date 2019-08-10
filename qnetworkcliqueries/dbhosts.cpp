#include "dbhosts.h"
#include <QSettings>

DBHosts* DBHosts::_instance = nullptr;

DBHosts::DBHosts()
{
    //abriendo hosts
}

DBHosts::~DBHosts()
{}

DBHosts *DBHosts::instance()
{
    if ( ! _instance )
        _instance = new DBHosts;

    return _instance;
}

void DBHosts::_load()
{
    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

//    user = settings.value("ConnectionUser").toString();
//    password = settings.value("ConnectionPassword").toString();
//    queriespath = settings.value("QueriesPath").toString();
//    mnemonicspath = settings.value("MnemonicsPath").toString();
//    backupspath = settings.value("BackupsPath").toString();
//    logspath = settings.value("LogsPath").toString();
}

void DBHosts::_save()
{
    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

//    settings.beginGroup("Connection");
//    settings.setValue("ConnectionUser",user);
//    settings.setValue("ConnectionPassword",password);
//    settings.endGroup();
}
