#include "queriesthread.h"
#include "funciones.h"
#include "bdhosts.h"
#include "properties.h"

QueriesThread::QueriesThread(QObject *parent) : QObject(parent)
{
    m_interval=0;
    m_simultaneos=0;
    m_maxparalelos=0;
    m_opciones=0;
    m_consultaSimultaneos=0;
    m_equiposConsultados=0;
    m_conerrores=0;
    m_sinconexion=0;
    m_equiposExitosos=0;
    m_conexionerrores=0;
    m_lstIpPos=-1;
    m_connectionprotocol = QRemoteShell::TelnetSSH;
    m_detener=false;
    m_cancelar=false;
    m_principaluserfirst=true;
    m_equipmentNeighborsConsultarVecinos=false;

    m_user = Properties::Instance()->user;
    m_password = Properties::Instance()->password;
    m_linuxprompt = Properties::Instance()->linuxpromt;
}

QueriesThread::~QueriesThread()
{
    //TODO verificar porq truena al eliminar m_lstIP
//    qDeleteAll(m_lstIP);
}

void QueriesThread::setLstIP(QStringList lst)
{
    m_lstIP = BDHosts::instance()->lstIPs2lstHosts( lst );
}

void QueriesThread::iniciar()
{
    if ( m_interval==0 ||
         m_simultaneos==0 ||
         m_maxparalelos==0 ||
         m_opciones==0 ||
         m_lstIP.isEmpty() ||
         m_equiposConsultados > 0 )
    {
        qDebug() << "QueriesThread::iniciar() emit finished ";

        emit finished(false);
        return;
    }

    m_timer = new QTimer(this);   
    m_timer->setInterval( m_interval );
    connect(m_timer,SIGNAL(timeout()),SLOT(on_timer_timeOut()));

    m_timerActivity = new QTimer(this);
    m_timerActivity->setInterval( 300000 );
    m_timerActivity->setSingleShot(true);
    connect(m_timerActivity,SIGNAL(timeout()),SLOT(on_timerActivity_timeOut()));

    on_timer_timeOut();
    m_timer->start();
    m_timerActivity->start();
}

void QueriesThread::detener()
{
    m_timer->stop();
    m_detener=true;
}

void QueriesThread::cancelar()
{
    m_timer->stop();
    m_cancelar=true;
    foreach (Queries* qry, m_queriesenconsulta)
    {
        m_conerrores++;
        m_errorMap.insert( qry->ip(), "Error" );
        qry->thread()->quit();
        qry->deleteLater();
    }
    m_queriesenconsulta.clear();
    m_equiposenconsulta.clear();

    emit finished(true);
}

void QueriesThread::on_timerActivity_timeOut()
{
    cancelar();
}

void QueriesThread::on_timer_timeOut()
{

    int paralelos;
    paralelos= m_maxparalelos ;

    if ( m_consultaSimultaneos >= paralelos )
        return;

    if ( m_lstIpPos >= m_lstIP.size()-1 )
    {
        //ya se llego al final
        m_timer->stop();
        return;
    }

    int equiposAconectar;
    if ( m_lstIP.size() - ( m_lstIpPos+1 ) > m_simultaneos )
        equiposAconectar = m_simultaneos;
    else
        equiposAconectar = m_lstIP.size() - ( m_lstIpPos+1 );

    int pos = m_lstIpPos+1;
    for ( int c=0; c<equiposAconectar; c++ )
    {
        m_lstIpPos = pos+c;
        conectarOtroEquipo();
    }
}

void QueriesThread::conectarOtroEquipo()
{
    QString ip = m_lstIP.at( m_lstIpPos )->ip;

    qDebug() << "QueriesThread::conectarOtroEquipo()" << ip;

    Queries *query = new Queries(ip,m_user,m_password,m_linuxprompt);
    query->setCountry( m_grupo );
    query->setGW( m_gw );
    query->setOptions( m_opciones );
    query->setConnectionProtocol( m_connectionprotocol );

    //si el listado de equipos a consultar es 1 significa que se configuro para que solo actualizará un equipo
    //no queremos que por equipmentNeighbors se conozcan nuevos vecinos y empezar a consultarlos
    if ( m_equipmentNeighborsConsultarVecinos )
        query->setLstHostsGeneralAconectar( m_lstIP );

    QThread *thr = new QThread(this);
    query->moveToThread( thr );    

    connect(thr,SIGNAL(started()),query,SLOT( start()) );
    connect(query,SIGNAL(finished(Queries*)),SLOT(equipoConsultado(Queries*)),Qt::QueuedConnection);
    connect(query,SIGNAL(finished(Queries*)),thr,SLOT(quit()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));

    m_consultaSimultaneos++;
    emit newInformation();

    m_equiposenconsulta.append( ip );
    m_queriesenconsulta.append( query );

    thr->start();

    qDebug() << "Iniciando conexion equipo" << ip;
}

void QueriesThread::equipoConsultado(Queries *qry)
{
    QMutexLocker locker(&m_mutex);

    if ( m_cancelar )
        return;

    m_timerActivity->stop();
    m_timerActivity->start();

    m_consultaSimultaneos--;
    m_equiposConsultados++;

    m_equiposenconsulta.removeOne( qry->ip() );
    m_queriesenconsulta.removeOne( qry );

    if ( qry->isConnected() )
    {
        if ( qry->successful() )
        {
            qDebug() << "Equipo consultado exitosamente" << qry->ip() << qry->hostName();

            m_equiposExitosos++;

//            //se agregan los vecinos de equipmentNeighbors a la consulta
//            //la verificación de nombre y plataforma se realiza en el hilo correspondiente
//            foreach (SEquipmentNeighborsInfo *equipmentNeighbors, qry->equipmentNeighborsNuevosEquipos())
//            {
//                bool encontrado=false;
//                QString equipmentNeighborsnombre = simplicateName( equipmentNeighbors->nombreequipo );
//                foreach (Host *host, m_lstIP)
//                {
//                    if ( host->nombre == equipmentNeighborsnombre )
//                    {
//                        encontrado=true;
//                        break;
//                    }
//                }
//                if ( !encontrado )
//                {
//                    Host *host = new Host;
//                    host->ip = equipmentNeighbors->ip;
//                    host->nombre = equipmentNeighborsnombre;
//                    m_lstIP.append( host );
//                    lstIPsAgregadosPorVecinos.append( host->ip );

//                    qDebug() << "equipmentNeighbors Equipo agregado" << equipmentNeighbors->nombreequipo << equipmentNeighbors->ip;
//                }
//            }

            for ( SOSPFInfo *o : qry->ospfInfo() )
            {
                bool encontrado=false;
                foreach (Host *host, m_lstIP)
                {
                    if ( host->ip == o->id )
                    {
                        encontrado=true;
                        break;
                    }
                }
                if ( !encontrado )
                {
                    Host *host = new Host;
                    host->ip = o->id;
                    m_lstIP.append( host );
                    lstIPsAgregadosPorVecinos.append( o->id );
                    if ( !m_timer->isActive() )
                        m_timer->start();
                }
            }
            //***********************************************************************************************

            //si ya hubiera otra consulta del mismo equipo se elimina porque puedo haberse realizado debido a que
            //se agrego el equipo por equipmentNeighbors cuando aun no existia el mismo, esto ya que hay varios equipos en consulta a la vez
            bool encontrado=false;
            foreach (Queries *q, lstQueries())
            {
                if ( qry->hostName() == q->hostName() &&
                     !qry->hostName().isEmpty() )
                {                    
                    encontrado=true;
                    qry->deleteLater();
                    break;
                }
            }
            if ( !encontrado )
                m_lstQueries.append( qry );
        }
        else
        {
            m_conerrores++;
            m_errorMap.insert( qry->ip(), "Error" );
            qry->deleteLater();
        }
    }
    else
    {
        if ( qry->isReachable() )
        {
            if ( qry->isReachable() )
            {
                m_errorMap.insert( qry->ip(), "Conexion Error" );
                m_conexionerrores++;
            }
            else
            {
                m_errorMap.insert( qry->ip(), "No conexion" );
                m_sinconexion++;
            }
        }

        //si la plataforma y el nombre no estan vacios significa que se pudo consultar la informacion por SNMP
        //por lo que no lo borramos para que pueda estar disponible a la hora de graficar
        if ( qry->platform().isEmpty() ||
             qry->hostName().isEmpty() )
            qry->deleteLater();
        else
            m_lstQueries.append( qry );
    }

    emit newInformation();

    if ( !m_detener )
    {
        //Si se termino de consultar los equipos se finaliza
        if ( m_equiposConsultados == m_lstIP.size() )
            emit finished(true);
        else
        {
            //conectar al siguiente equipo
            if ( m_lstIpPos < m_lstIP.size()-1 )
            {
                m_lstIpPos++;
                conectarOtroEquipo();
            }
        }
    }
    else
    {
        if ( m_consultaSimultaneos == 0 )
            emit finished(true);
    }
}

QMap<QString, QString> updateInfoMapError(QMap<QString, QString> &ant, QMap<QString, QString> &nue,
                                          QStringList &lstIPs, short modo)
{
    QMap<QString, QString> salida;

    switch (modo)
    {
    case 0: //debug, nunca tendria que venir este
    case 1: //todos los equipos la primera vez, no hay que actualizar
    case 2: //se actualizan todos los equipos, se manda completo el nuevo
    case 6: //consulta por primera vez de un listado de IPs
    case 10: //actualizar formato de archivo, nunca tendria que venir este
        return nue;
    case 3: //se actualizan todos los equipos pendientes
    case 4: //actualizar un equipo o listado de IPs en archivo
    case 5: //actualizar todos los equipos con errores
    {      
        QMapIterator<QString, QString> iant(ant);
        while (iant.hasNext())
        {
            iant.next();
            if ( !lstIPs.contains( iant.key() ) )
                salida.insert( iant.key(),iant.value() );
            else
            {
                QMapIterator<QString, QString> inue(nue);
                while (inue.hasNext())
                {
                    inue.next();
                    if ( iant.key() == inue.key() )
                    {
                        salida.insert( inue.key(),inue.value() );
                        break;
                    }
                }
            }
        }
        break;
    }
    }

    return salida;
}
