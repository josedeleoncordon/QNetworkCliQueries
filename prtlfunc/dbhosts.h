#ifndef DBHOSTS_H
#define DBHOSTS_H

#include <QString>
#include <QList>

#include "prtlfunc_global.h"

struct Host
{
    QString grupo;
    QString subgrupo;
    QString nombre;
    QString ip;
};

class QPRTLFUNCSHARED_EXPORT DBHosts
{
private:
    static DBHosts* _instance;
    QList<Host> _lstHosts;

public:
    DBHosts();
    ~DBHosts();

    static DBHosts *instance();
};

#endif // DBHOSTS_H
