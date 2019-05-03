#include "bdhosts.h"

#include "properties.h"
#include "funciones.h"

BDHosts* BDHosts::m_instance = NULL;

BDHosts::BDHosts()
{
    //abriendo hosts

    QFile file(Properties::Instance()->backupspath+"/bdhosts.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "No se pudo abrir el archivo de hosts:" << Properties::Instance()->backupspath+"/bdhosts.txt";
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    while (!line.isNull()) {

        Host *host = new Host;
        QStringList lst = line.split(" ");
        host->ip = lst.at(0).simplified();
        host->nombre = lst.at(1).simplified();
        if ( lst.size() == 3 )
            host->pais = lst.at(2).simplified();

        lstHosts.append(host);

        line = in.readLine();
    }

    //Abriendo IPs de todas las interfaces de todos los equipos

    QFile file2(Properties::Instance()->backupspath+"/hostsIterfacesL3IPs.txt");
    if (!file2.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "No se pudo abrir el archivo de hosts:" << Properties::Instance()->backupspath+"/hostsIterfacesL3IPs.txt";
        return;
    }

    QTextStream in2(&file2);
    line = in2.readLine();
    while (!line.isNull()) {

        Host *host = new Host;
        QStringList lst = line.split("\t");
        host->pais = lst.at(0).simplified();
        host->nombre = lst.at(1).simplified();
        host->interfaz = lst.at(2).simplified();
        host->vrf = lst.at(3).simplified();
        host->ip = lst.at(4).simplified();
        if ( lst.at(5) == "NoShutdown" )
            host->interfaceIsShutdown = false;
        else
            host->interfaceIsShutdown = true;

        lstHostsInterfacesIPs.append(host);

        line = in2.readLine();
    }

    foreach (QString pais, lstPaises())
    {
        QList<Host*> lst;
        foreach (Host *host, lstHostsInterfacesIPs)
        {
            if ( host->pais == pais )
                lst.append( host );
        }
        mapHostsInterfacesIPs.insert(pais,lst);
    }
}

BDHosts::~BDHosts()
{
    qDeleteAll(lstHosts);

    foreach (QString pais, lstPaises())
        qDeleteAll(mapHostsInterfacesIPs.value(pais));
}

BDHosts *BDHosts::instance()
{
    if ( ! m_instance )
        m_instance = new BDHosts;

    return m_instance;
}

void BDHosts::reload()
{
    if ( m_instance )
        delete m_instance;

    m_instance = new BDHosts;
}

QString BDHosts::hostIPAddress(QString nombre)
{
    foreach (Host *h, lstHosts)
    {
        if ( nombre == h->nombre )
            return h->ip;
    }
    return "";
}

QString BDHosts::hostName(QString ip, QString pais)
{
    QList<Host*> lst;
    if ( pais.isEmpty() )
        lst = lstHostsInterfacesIPs;
    else
        lst = mapHostsInterfacesIPs.value( pais );

    foreach (Host *h, lst)
    {
        if ( ip == h->ip )
            return h->nombre;
    }
    return "";
}

QString BDHosts::hostCountry(QString nombre)
{
    foreach (Host *h, lstHosts)
    {
        if ( nombre == h->nombre )
            return h->pais;
    }
    return "";
}

QStringList BDHosts::extractIPAddresses(QStringList lst)
{
    QStringList salida;
    QRegExp exp;
    exp.setPattern("\\.\\d+\\.\\d+\\.\\d+\\.\\d+");

    foreach (QString host, lst)
    {
        exp.indexIn( host );
        QString txt = exp.cap(0);
        txt = txt.right( txt.size()-1 );
        if ( !txt.isEmpty() )
            salida.append( txt );
    }
    return salida;
}

QStringList BDHosts::rrFrom(QString prefix)
{
    QDir dir( Properties::Instance()->backupspath+"/"+prefix+"_RR" );

    QRegExp exp;
    exp.setPattern("\\.\\d+\\.\\d+\\.\\d+\\.\\d+");

    QStringList lst = dir.entryList(QDir::NoDotAndDotDot|QDir::Files);
    return extractIPAddresses( lst );
}

QStringList BDHosts::accessFrom(QString prefix)
{
    QStringList salida;
    QDir dir;
    dir.setFilter( QDir::NoDotAndDotDot|QDir::Files );

    dir.setPath( Properties::Instance()->backupspath+"/"+prefix+"_METRO" );
    salida.append(  extractIPAddresses(dir.entryList()) );

    dir.setPath( Properties::Instance()->backupspath+"/"+prefix+"_PE_METRO" );
    salida.append(  extractIPAddresses(dir.entryList()) );

    dir.setPath( Properties::Instance()->backupspath+"/"+prefix+"_METRO-PE" );
    salida.append(  extractIPAddresses(dir.entryList()) );

    dir.setPath( Properties::Instance()->backupspath+"/"+prefix+"_ACCESO" );
    salida.append(  extractIPAddresses(dir.entryList()) );

    dir.setPath( Properties::Instance()->backupspath+"/"+prefix+"_DISTRIBUCION" );
    salida.append(  extractIPAddresses(dir.entryList()) );

    dir.setPath( Properties::Instance()->backupspath+"/"+prefix+"_RCT" );
    salida.append(  extractIPAddresses(dir.entryList()) );

    dir.setPath( Properties::Instance()->backupspath+"/"+prefix+"_MWR" );
    salida.append(  extractIPAddresses(dir.entryList()) );

    return salida;
}

QStringList BDHosts::equipmentFrom(QString prefix,QString equipmetclass)
{
    QStringList salida;
    QStringList lst;     
    QStringList lstEquipmetclass = equipmetclass.split(",",QString::SkipEmptyParts);

    QRegExp exp("/"+prefix+"_(?!(SEG|CMTS))");
    QRegExp exp2("/"+prefix+"_.+/.+");
    QDirIterator it( Properties::Instance()->backupspath,
                     QDir::NoDotAndDotDot|QDir::Files,
                     QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString file = it.next();

        if ( file.contains(exp) )
        {
            if ( lstEquipmetclass.isEmpty() )
            {
                if ( file.contains(exp2) )
                    lst.append( file );
            }
            else
                foreach (QString cl, lstEquipmetclass)
                    if ( file.contains( prefix+"_"+cl ) )
                        lst.append( file );
        }
    }

    salida.append( extractIPAddresses(lst) );

    return salida;
}

QStringList BDHosts::pesPsFrom(QString prefix)
{
    QString pes = openFile( Properties::Instance()->backupspath+"/"+prefix+"_PEsPs.txt" );
    return pes.split("\n",QString::SkipEmptyParts);
}

QStringList BDHosts::pesFrom(QString prefix)
{
    QString pes = openFile( Properties::Instance()->backupspath+"/"+prefix+"_PEs.txt" );
    return pes.split("\n",QString::SkipEmptyParts);
}

QList<Host*> BDHosts::lstIPs2lstHosts(QStringList lst)
{
    QList<Host*> lstSalida;
    foreach (QString ip, lst)
    {
        bool encontrado = false;
        foreach (Host *host, lstHostsInterfacesIPs)
        {
            if ( host->ip == ip && !host->interfaceIsShutdown )
            {
                encontrado=true;
                lstSalida.append( host );
                break;
            }
        }
        if ( !encontrado )
        {
            Host* host = new Host;
            host->ip = ip;
            lstSalida.append(host);
        }
    }
    return lstSalida;
}

