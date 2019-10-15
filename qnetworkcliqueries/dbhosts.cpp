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

    QStringList lst;

    settings.beginGroup("Hosts");
    lst = settings.value("Hosts").toStringList();
    settings.endGroup();

    for (QString s : lst)
    {
        QStringList data = s.split(",");
        Host *h = new Host;
        h->grupo = data.at(0);
        h->subgrupo = data.at(1);
        h->ip = data.at(2);
        h->nombre = data.at(3);
        _lstHost.append(h);
    }
}

void DBHosts::_save()
{
    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

    QStringList lst;
    for ( Host *h : _lstHost )
        lst.append( h->grupo+","+h->subgrupo+","+h->ip+","+h->nombre );

    settings.beginGroup("Hosts");
    settings.setValue("Hosts",lst);
    settings.endGroup();
}
