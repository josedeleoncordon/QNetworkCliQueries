#ifndef BDHOSTS_H
#define BDHOSTS_H

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDirIterator>
#include <QDebug>
#include <QString>
#include <QObject>

#include "qnetworkcliqueries_global.h"

struct Host
{
    QString nombre;
    QString interfaz;
    bool interfaceIsShutdown;
    QString ip;
    QString grupo;
    QString vrf;    
};

class QNETWORKCLIQUERIES_EXPORT BDHosts
{
private:    
    QList<Host*> lstHosts;
    QMap<QString,QList<Host*>> mapHostsInterfacesIPs;
    QList<Host*> lstHostsInterfacesIPs;
    static BDHosts* m_instance;

    QStringList extractIPAddresses(QStringList);

public:
    BDHosts();
    ~BDHosts();

    static BDHosts *instance();
    static void reload();

    QString hostIPAddress(QString nombre); //devuelve la IP del equipo segun el nombre
    QString hostName(QString ip,QString grupo=""); //devuelve el nombre del equipo segun la IP
    QString hostCountry(QString nombre); //devuelve el grupo del equipo segun el nombre

    const QList<Host*> hostsAllInterfacesIPs(QString grupo) { return mapHostsInterfacesIPs.value(grupo); }

    QStringList rrFrom(QString);
    QStringList accessFrom(QString);
    QStringList equipmentFrom(QString,QString equipmetclass=""); //equipmetclass: METRO,CORE,DISTRIBUCION,METRO, segun el backup: CORE,METRO,PE_METRO,IPP,TX,SEG,BRAS, separados por coma
    QStringList pesPsFrom(QString);
    QStringList pesFrom(QString); //para este primero hay que sacarlos de los RR, en este caso solo se consulta el archivo previamente generado

    QList<Host*> lstIPs2lstHosts(QStringList lst);
};

#endif
