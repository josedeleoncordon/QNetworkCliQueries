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
