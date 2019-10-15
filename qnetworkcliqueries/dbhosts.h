#ifndef DBHOSTS_H
#define DBHOSTS_H

#include <QString>
#include <QMap>

#include "qnetworkcliqueries_global.h"

enum dbhoststype
{
    BackupGrupoSubgrupo,
    ListadoGrupoSubgrupo
};

struct Host
{
    QString grupo;
    QString subgrupo;    
    QString ip;
    QString nombre;
};

class QNETWORKCLIQUERIES_EXPORT DBHosts
{
private:
    static DBHosts* _instance;
    QList<Host*> _lstHost;

    void _load();
    void _save();

public:
    DBHosts();
    ~DBHosts();

    static DBHosts *instance();    
};



#endif // DBHOSTS_H
