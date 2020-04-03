#include "queriesthread.h"
#include "funciones.h"
#include "properties.h"
#include "qnetworkquerieslogging.h"

#include <QEventLoop>

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
    m_equiposPorGWenConsulta=0;
    m_connectionprotocol = QRemoteShell::SSHTelnet;
    m_detener=false;
    m_cancelar=false;
    m_principaluserfirst=true;
    m_soloequiposnuevos=false;
    m_consultarVecinosOSPFMismoDominio=false;
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

void QueriesThread::iniciar()
{
    if ( m_interval==0 ||
         m_simultaneos==0 ||
         m_maxparalelos==0 ||
         m_opciones==0 ||
         m_lstIP.isEmpty() ||
         m_equiposConsultados > 0 )
    {
        qCDebug(queriesthread) << "QueriesThread::iniciar() emit finished ";

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

void QueriesThread::iniciarSync()
{
    disconnect(); //desconectamos todas las señales antes configuradas
    QEventLoop loop;
    connect(this, SIGNAL(finished(bool)), &loop, SLOT(quit()));
    iniciar();
    loop.exec();
}

void QueriesThread::detener()
{
    m_timer->stop();
    m_detener=true;
    m_equipmentNeighborsConsultarVecinos=false;
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
    paralelos = m_maxparalelos ;

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
        conectarOtroEquipo( m_lstIP.at( m_lstIpPos ) );
    }
}

void QueriesThread::conectarOtroEquipo(QString ip, bool gw)
{
    qCDebug(queriesthread) << ip << "QueriesThread::conectarOtroEquipo()" << "GW" << gw;

    Queries *query = new Queries(ip,m_user,m_password,m_linuxprompt);
    query->setCountry( m_grupo );
    if ( gw )
        query->setGW( m_gw );
    if ( m_consultarVecinosOSPFMismoDominio )
        query->setIpOInterfazMismoDominioOSPFDondeSeViene( m_mapOSPFVecinosInterfazDondeVienen.value( ip ) );
    query->setOptions( m_opciones );
    query->setConnectionProtocol( m_connectionprotocol );
    query->setUser2( m_user2 );
    query->setPassword2( m_pwd2 );

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

    qCDebug(queriesthread) << "Iniciando conexion equipo" << ip;
}

void QueriesThread::equipoConsultado(Queries *qry)
{
    QMutexLocker locker(&m_mutex);

    if ( m_cancelar )
        return;

    m_timerActivity->stop();
    m_timerActivity->start();

    m_consultaSimultaneos--;

    m_equiposenconsulta.removeOne( qry->ip() );
    m_queriesenconsulta.removeOne( qry );

    if ( qry->gw().isEmpty() )
        m_equiposConsultados++;
    else
        m_equiposPorGWenConsulta--;

    if ( qry->isConnected() )
    {
        if ( qry->successful() )
        {
            //para que esten en el log del equipo y el general
            qCDebug(queriesthread) << qry->ip() << "QueriesThread::equipoConsultado Equipo consultado exitosamente"
                     << qry->hostName()
                     << m_equipmentNeighborsConsultarVecinos;
            qCDebug(queriesthread) << "QueriesThread::equipoConsultado Equipo consultado exitosamente"
                     << qry->ip() << qry->hostName()
                     << m_equipmentNeighborsConsultarVecinos;

            m_equiposExitosos++;

            if ( m_equipmentNeighborsConsultarVecinos )
            {
                //agregando vecinos de cdp/llp a la consulta
                for (SEquipmentNeighborsInfo &e : qry->equipmentNeighborsInfo())
                {
                    validarYagregarVecinoAconsulta( qry,
                                                    e.ip,
                                                    qry->ipOinterfazMismoDominioOSPFDondeSeViene(),
                                                    e.interfazestesalida,
                                                    e.interfazotroentrada);
                }

                //agregando los vecinos de ospf que no se agregaron por cdp/llp a la consulta
                for ( SOSPFInfo &o : qry->ospfInfo() )
                {
                    QString ipInterfazSiguiente = qry->interfacesIpAddressesQuery->ipFromInterfaz(o.interfaz);
                    validarYagregarVecinoAconsulta( qry,
                                                    o.id,
                                                    qry->ipOinterfazMismoDominioOSPFDondeSeViene(),
                                                    o.interfaz,
                                                    ipInterfazSiguiente );
                }

                if ( !m_timer->isActive() )
                    m_timer->start();
            }

            //si ya hubiera otra consulta del mismo equipo se elimina porque pudo haberse realizado debido a que
            //se agrego el equipo por equipmentNeighbors cuando aun no existia el mismo, esto ya que hay varios equipos en consulta a la vez
            bool encontrado=false;
            for (Queries *q : m_lstQueries)
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
            {
                m_lstIPsConectadosPorGW.append( qry->ip() );
                m_lstQueries.append( qry );
            }
        }
        else
        {
            m_conerrores++;
            qCDebug(queriesthread) << qry->ip() << "QueriesThread::equipoConsultado -- Consulta con errores";
            qCDebug(queriesthread) << "QueriesThread::equipoConsultado -- consulta con errores" << qry->ip();
            m_errorMap.insert( qry->ip(), "Error" );
            qry->deleteLater();
        }
    }
    else
    {
        if ( qry->isReachable() )
        {
            qCDebug(queriesthread) << qry->ip() << "QueriesThread::equipoConsultado -- Conexion Error";
            qCDebug(queriesthread) << "QueriesThread::equipoConsultado -- Conexion Error" << qry->ip();
            m_errorMap.insert( qry->ip(), "Conexion Error" );
            m_conexionerrores++;
        }
        else
        {
            if ( qry->gw().isEmpty() && !m_gw.isEmpty() )
            {
                qCDebug(queriesthread) << qry->ip()
                                       << "QueriesThread::equipoConsultado -- se agrega a lista de equipos a consultar por GW";
                qCDebug(queriesthread) << "QueriesThread::equipoConsultado -- se agrega a lista de equipos a consultar por GW"
                                       << qry->ip();
                m_lstIPsAintentarPorGW.append(qry->ip());
            }
            else
            {
                qCDebug(queriesthread) << qry->ip() << "QueriesThread::equipoConsultado -- No conexion";
                qCDebug(queriesthread) << "QueriesThread::equipoConsultado -- No conexion" << qry->ip();
                m_errorMap.insert( qry->ip(), "No conexion" );
                m_sinconexion++;
                qry->deleteLater();
            }
        }

//        //si la plataforma y el nombre no estan vacios significa que se pudo consultar la informacion por SNMP
//        //por lo que no lo borramos para que pueda estar disponible a la hora de graficar
//        if ( qry->platform().isEmpty() ||
//             qry->hostName().isEmpty() )
//            qry->deleteLater();
//        else
//            m_lstQueries.append( qry );
    }

    emit newInformation();

    if ( !m_detener )
    {
        //Si se termino de consultar los equipos se finaliza
        if ( m_equiposConsultados >= m_lstIP.size() &&
             m_lstIPsAintentarPorGW.isEmpty() &&
             !m_equiposPorGWenConsulta )
            emit finished(true);
        else
        {
            //conectar al siguiente equipo

            qCDebug(queriesthread) << "probando empezar uno por GW" << m_lstIPsAintentarPorGW << m_equiposPorGWenConsulta;

            if ( !m_lstIPsAintentarPorGW.isEmpty() && m_equiposPorGWenConsulta < 8 )
            {
                QString IP = m_lstIPsAintentarPorGW.takeFirst();
                qCDebug(queriesthread) << IP << "se intenta la conexion por GW";

                //tratamos de conectar a un equipo q fallo por medio del gw
                m_equiposPorGWenConsulta++;
                conectarOtroEquipo( IP, true );
            }
            else
                if ( m_lstIpPos < m_lstIP.size()-1 )
                {
                    //continuamos con la consulta normal
                    m_lstIpPos++;
                    QString ip = m_lstIP.at( m_lstIpPos );
                    qCDebug(queriesthread) << ip << "siguiente conexion normal";
                    conectarOtroEquipo( ip );
                }
        }
    }
    else
    {
        if ( m_consultaSimultaneos == 0 )
            emit finished(true);
    }
}

void QueriesThread::validarYagregarVecinoAconsulta(Queries *qry,
                                                   QString ip,
                                                   QString ipOinterfazDondeSeViene,
                                                   QString interfazEsteEquipoSalida,
                                                   QString interfazSiguienteEquipoEntrada)
{
    //que no este en el listado actual a consultar
    //no pasa si solo equipos nuevos y ya esta en el listado de anteriores
    if ( !m_lstIP.contains(ip) &&
         !( m_soloequiposnuevos && lstIPsConsultaAnterior.contains( ip ) ) )
    {
        //que este en el listado de segmentos de links
        for ( QString segmento : m_lstLinksEnSegmentos )
        {
            bool encontrado=false;
            if ( validarIPperteneceAsegmento( qry->interfacesIpAddressesQuery->ipFromInterfaz( interfazEsteEquipoSalida ),
                                              segmento) )
            {
                encontrado=true;
                break;
            }
            if ( !encontrado )
                return;
        }

        //que este en el listado de segmentos de loopbacks
        for ( QString segmento : m_lstLoopbacksEnSegmentos )
        {
            bool encontrado=false;
            if ( validarIPperteneceAsegmento( ip,segmento) )
            {
                encontrado=true;
                break;
            }
            if ( !encontrado )
                return;
        }

        //ospf area
        if ( !m_consultaOSPFArea.isEmpty() )
        {
            for ( SOSPFInfo &oi : qry->ospfInfo() )
                if ( oi.interfaz == interfazEsteEquipoSalida &&
                     oi.area != m_consultaOSPFArea )
                    return;
        }

        //ospf mismo dominio
        if ( m_consultarVecinosOSPFMismoDominio )
        {
            if ( !continuarPorsiguienteInterfazMismoDominioOSPF( *qry,
                                                                 ipOinterfazDondeSeViene,
                                                                 interfazEsteEquipoSalida ) )
                return;
        }

        //se agrega al listado de equipos a consultar
        qCDebug(queriesthreadNeighbors) << qry->ip()
                                        << "Agregando vecino a la consulta" << ip;
        m_lstIP.append( ip );
        m_mapOSPFVecinosInterfazDondeVienen.insert( ip, interfazSiguienteEquipoEntrada );
    }
    return;
}

QList<Queries> QueriesThread::lstQueries()
{
    QList<Queries> lst;

    for ( Queries *qry : m_lstQueries )
        lst.append(*qry);

    return lst;
}

QMap<QString, QString> updateInfoMapError(QMap<QString, QString> &ant, QMap<QString, QString> &nue,
                                          QStringList &lstIPs)
{
    QMap<QString, QString> salida;

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

    return salida;
}


