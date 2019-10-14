#include "bdhostsupdater.h"
#include "bdhosts.h"
#include "properties.h"
#include "queries.h"
#include "queriesthread.h"
#include "funciones.h"

BDHostsUpdater::BDHostsUpdater(QObject *parent) : QObject(parent)
{}

void BDHostsUpdater::updateDB( QString path )
{
    m_path = path;

    m_lstGrupos = lstGrupos();
    lstGruposPos=-1;
    queriesThread=nullptr;

    consultarPEs_otroGrupo();
}

void BDHostsUpdater::consultarPEs_otroGrupo()
{
    //consultar los RR para sacar las IPs de los PEs
    if ( lstGruposPos < m_lstGrupos.size()-1 )
    {
        lstGruposPos++;
        grupo = m_lstGrupos.at( lstGruposPos );
        queriesThread = new QueriesThread(this);
        connect( queriesThread,SIGNAL(finished(bool)),SLOT(on_actualizarPEs_finished(bool)));

        queriesThread->setGrupo( grupo );
        queriesThread->setUser( Properties::Instance()->user );
        queriesThread->setPassword( Properties::Instance()->password );
        queriesThread->setLstIP( BDHosts::instance()->rrFrom( grupo ) );
        queriesThread->setConnectionProtocol( QRemoteShell::SSHTelnet );
        queriesThread->setInterval( 1000 );
        queriesThread->setSimultaneos( 3 );
        queriesThread->setMaxParalelos( 10 );

        int opciones = Queries::Connect |
                Queries::Platform |
                Queries::BGPNeig |
                Queries::Exit;

        queriesThread->setOpciones( opciones );

        QueriesConfiguration::instance()->mapQueries.insert("BGPNeig_Type","VPNV4");

        queriesThread->iniciar();
    }
    else
        //generamos ahora todos los equipos L3, Ps y PEs
        on_actualizarPEsPs_finished();
}

void BDHostsUpdater::on_actualizarPEs_finished(bool ok)
{
    //se termina la consulta, se guardan las IPs de los PEs de ese grupo
    if ( ok )
    {
        QList<Queries*> lstRRs = queriesThread->lstQueries();        
        QStringList lstPEIPs;

        foreach (Queries *qry, lstRRs)
        {
            QStringList lstNuevos = qry->bgpNeighborsInfo();
            foreach (QString ip, lstNuevos )
            {
                if ( ! lstPEIPs.contains(ip) )
                    lstPEIPs.append( ip );
            }
        }

        saveFile( lstPEIPs.join("\n"), m_path+"/"+grupo+"_PEs.txt" );
        lstPEIPsGeneral.append( lstPEIPs );
        lstPEIPs.clear();
    }
    consultarPEs_otroGrupo();
}

void BDHostsUpdater::on_actualizarPEsPs_finished()
{
    //generar los P y PEs

    QStringList lstPEsPsIPs;
    foreach (QString grupo, m_lstGrupos)
    {
        QDirIterator it( m_path , QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString filetxt = it.next();
            filetxt.remove( m_path );

            if ( ! filetxt.contains( grupo+"_" ) )
                continue;

            QRegExp exp;
            exp.setMinimal(true);

            QFile file( m_path+"/"+filetxt );
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                continue;

            QTextStream in(&file);
            while (!in.atEnd())
            {
                QString line = in.readLine();
                line = line.simplified();

                exp.setPattern("^mpls (ip|ldp)+$");
                if ( line.contains(exp) )
                {
                    exp.setPattern(".+\\.(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})\\..+");
                    if ( filetxt.contains(exp) )
                    {
                        lstPEsPsIPs.append( exp.cap(1) );
                        break;
                    }
                }
            }
        }

        QString salida;
        foreach (QString ip, lstPEsPsIPs)
            salida.append(ip+"\n");

        lstPEsPsIPs.clear();
        saveFile( salida, m_path+"/"+grupo+"_PEsPs.txt" );
    }

    on_actualizarETCHosts_finished();
}



void BDHostsUpdater::on_actualizarETCHosts_finished()
{
    //generamos el nuevo archivos de hosts

    QList<Host> lstHosts;

    QDirIterator it( m_path , QDir::Files, QDirIterator::Subdirectories );
    while (it.hasNext())
    {
        QString d = it.next();
        QString completo = d;
        QDir dir( d );
        QString file = dir.dirName();
        QString country = d.replace(m_path,"").split("_").at(0);
        country.replace("/","");

        QRegExp exp("(.+)\\.(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})\\.txt");
        exp.setMinimal(true);

        QRegExp exp2("RP_0.+\\_");
        QRegExp exp3("hostname (.+)$");
        if ( file.contains(exp) )
        {
            Host h;
            h.nombre = exp.cap(1);
            h.ip = exp.cap(2);
            h.grupo = country;

            foreach (Host hh, lstHosts)
            {
                if ( hh.ip == h.ip )
                {
                    qDebug() << "Equipo duplicado" << hh.nombre << h.nombre;
                    break;
                }
            }

            lstHosts.append( h );

            //************************
            //extraemos el hostname del backup ya que hay equipos que tienen difernte nombre de archivo y hostname
            QString file = openFile( completo );
            foreach (QString line, file.split("\n"))
            {
                if ( line.contains(exp3) )
                {
                    QString hostnuevo = exp3.cap(1);
                    if ( hostnuevo != h.nombre )
                    {
                        Host h2;
                        h2.nombre = hostnuevo;
                        h2.ip = h.ip;
                        h2.grupo = h.grupo;

                        bool encontrado=false;
                        foreach (Host hh, lstHosts)
                        {
                            if ( hh.nombre == h2.nombre )
                            {
                                encontrado=true;
                                break;
                            }
                        }
                        if ( !encontrado )
                            lstHosts.append( h2 );
                    }
                    break;
                }
            }
            //*************

            if ( h.nombre.contains(exp2) )
            {
                Host oh;
                oh.nombre = simplicateName( h.nombre );
                oh.ip = h.ip;
                oh.grupo = country;

                bool encontrado=false;
                foreach (Host hh, lstHosts)
                {
                    if ( hh.nombre == oh.nombre )
                    {
                        encontrado=true;
                        break;
                    }
                }
                if ( !encontrado )
                    lstHosts.append( oh );
            }
        }
    }

    //agregando los hosts que se tenian, en /etc/hosts, al final

    QFile file("/etc/hosts");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "No se pudo abrir el archivo de hosts /etc/hosts";
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();
    QList<Host*> lstEtcHosts;
    while (!line.isNull())
    {
        QStringList lst = line.split(" ");

        if (  line.contains("#") ||
              line.contains("127.0.0") ||
              line.contains("::") ||
              lst.size() < 2 )
        {
            line = in.readLine();
            continue;
        }

        Host *host = new Host;
        host->ip = lst.at(0).simplified();
        host->nombre = lst.at(1).simplified();
        if ( lst.size() == 3 )
            host->grupo = lst.at(2).simplified();

        lstEtcHosts.append(host);

        line = in.readLine();
    }

    foreach (Host *h, lstEtcHosts )
    {
        bool encontrado=false;
        foreach (Host nh, lstHosts)
        {
            if ( nh.nombre == h->nombre )
            {
                encontrado=true;
                break;
            }
        }
        if ( encontrado )
            continue;

        Host nh;
        nh.nombre = h->nombre;
        nh.ip = h->ip;

        lstHosts.append(nh);
    }

    QString salidaEtc="127.0.0.1 localhost\n";
    QString salidaBD;

    foreach (Host h, lstHosts)
    {
        salidaEtc.append( h.ip +" "+ h.nombre+ "\n" );
        salidaBD.append( h.ip +" "+ h.nombre+" "+ h.grupo + "\n" );
    }

    saveFile(salidaEtc,m_path+"/hosts");
    saveFile(salidaBD,m_path+"/bdhosts.txt");

    on_actualizarHostALLIPs_finished();
}

void BDHostsUpdater::on_actualizarHostALLIPs_finished()
{
    //generamos el archivo de todas las direcciones IPs usadas en los equipos

    QString salida;
    QStringList salidavlans;

    QStringList lstIPsGlobal;

    QRegExp expHostname("^ ?(sysname|hostname)+ (.+)$");
    QRegExp expInterface("^interface ((\\w|\\.|\\/)+)$");
    QRegExp expIP("^ ip(v|\\d)* address (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) \\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
    QRegExp expFileIP(".+\\.(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})\\.txt");

    QDirIterator it( m_path , QDir::Files, QDirIterator::Subdirectories );
    while (it.hasNext())
    {
        QString fileTxt = it.next();
        QString host;

        QString filetxtIpAddress;
        if ( fileTxt.contains(expFileIP) )
            filetxtIpAddress=expFileIP.cap(1);

        QString country = fileTxt.replace(m_path,"").split("_").at(0);
        country.replace("/","");

        QFile file(m_path+"/"+fileTxt);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "No se pudo abrir el archivo " << fileTxt;
            return;
        }

        QTextStream in(&file);
        QString line = in.readLine();

        QString interface;
        QString vrf;
        bool shutdown;

        while (!line.isNull())
        {
            //recorremos todas las lineas del archivo de backup

            if ( line.contains(expHostname) )
            {
                //la configuracion de hostname siempre aparece antes que las interfaces
                host = expHostname.cap(2);
                line = in.readLine();
                continue;
            }

            if ( line.contains(expInterface) )
            {
                interface = expInterface.cap(1);
                shutdown = false;
                vrf.clear();
                line = in.readLine();
                continue;
            }

            if ( !interface.isEmpty() )
            {
                //ya se encontro interfaz

                if ( line.contains("vrf ") )
                {
                    vrf = line.split(" ",QString::SkipEmptyParts).last().simplified();
                    line = in.readLine();
                    continue;
                }

                if ( line.contains("shutdown") )
                {
                    shutdown=true;
                    line = in.readLine();
                    continue;
                }
            }

            if ( line.contains(expIP) &&
                 !interface.isEmpty() )
            {
                //encontramos una interfaz L3
                QString ip = expIP.cap(2);
                salida.append(country+"\t"+host+"\t"+interface+"\t"+
                              (vrf.isEmpty()?"NoVRF":vrf)+"\t"+ip+"\t"+ ( shutdown?"Shutdown":"NoShutdown" ) +"\n");

                if ( vrf.isEmpty() && !shutdown )
                    lstIPsGlobal.append( ip );

                //Se guardan las vlans q se usan de gestion
                QString grupo = fileTxt.split("/",QString::SkipEmptyParts).at(0);
                if ( (grupo.contains("METRO") ||
                      grupo.contains("ACCESO")) &&
                     !grupo.contains("RC"))
                {
                    if ( filetxtIpAddress == ip &&
                         (interface.contains("Vlan") ||
                          interface.contains("BDI")) )
                    {
                        interface.replace("Vlanif","");
                        interface.replace("Vlan","");
                        interface.replace("BDI","");
                        if ( !salidavlans.contains( country+"\t"+interface ) &&
                             interface!="1")
                            salidavlans.append(country+"\t"+interface);
                    }
                }

                interface.clear();
            }

            line = in.readLine();
        }
    }
    saveFile(salida,m_path+"/hostsIterfacesL3IPs.txt");

    //creamos un archivo con las IPs de los PEs conocidos en los RR y que no estan en el BCK
    QStringList lstPEsNO;
    foreach (QString ip, lstPEIPsGeneral)
    {
        if ( !lstIPsGlobal.contains(ip) )
            lstPEsNO.append( ip );
    }
    saveFile( lstPEsNO.join("\n"), m_path+"/nuevos_PEs.txt" );

    emit finished();
}
