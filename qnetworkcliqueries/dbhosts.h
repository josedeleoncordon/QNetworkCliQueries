#ifndef DBHOSTS_H
#define DBHOSTS_H

#include <QString>
#include <QList>

#include "qnetworkcliqueries_global.h"

struct Host
{
    QString grupo;
    QString subgrupo;
    QString nombre;
    QString ip;
};

class QNETWORKCLIQUERIES_EXPORT DBHosts
{
private:
    static DBHosts* _instance;
    QList<Host> _lstHosts;

    void _load();
    void _save();

public:
    DBHosts();
    ~DBHosts();

    static DBHosts *instance();    
};



#endif // DBHOSTS_H
