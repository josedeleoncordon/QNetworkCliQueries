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
    m_connectionprotocol = QRemoteShell::SSHTelnet;
    m_principaluserfirst=true;
    m_soloequiposnuevos=false;
    m_consultarVecinosOSPFMismoDominio=false;
    m_equipmentNeighborsConsultarVecinos=false;
    m_timer = nullptr;
    m_timerActivity = nullptr;
    _clear();

    m_user = Properties::Instance()->user;
    m_password = Properties::Instance()->password;
    m_linuxprompt = Properties::Instance()->linuxpromt;
}

QueriesThread::~QueriesThread()
{
    //TODO verificar porq truena al eliminar m_lstIP
//    qDeleteAll(m_lstIP);
}

void QueriesThread::_clear()
{
    m_consultaSimultaneos=0;
    m_equiposConsultados=0;
    m_conerrores=0;
    m_sinconexion=0;
    m_equiposExitosos=0;
    m_conexionerrores=0;
    m_lstIpPos=-1;
    m_equiposPorGWenConsulta=0;
    m_detener=false;
    m_cancelar=false;
    m_queriesenconsulta.clear();
    m_equiposenconsulta.clear();
    m_errorMap.clear();
    m_mapOSPFVecinosInterfazDondeVienen.clear();
    m_lstIPsAintentarPorGW.clear();
    ThreadWorker = &newQueriesThreadWorker;
    if ( m_timer )
    {
        delete m_timer;
        m_timer = nullptr;
    }
    if ( m_timerActivity )
    {
        delete m_timerActivity;
        m_timerActivity = nullptr;
    }
}

void QueriesThread::setOptions(QList<int> opciones)
{
    opciones.removeAll( QueryOpcion::Null ); //unicamente opciones validas
    m_opciones = opciones;
    _clear();
}

void QueriesThread::setNewThreadWorker(QueriesThreadWorker*(*threadworker)(void))
{
    ThreadWorker = threadworker;
}

void QueriesThread::iniciar()
{
    if ( m_interval==0 ||
         m_simultaneos==0 ||
         m_maxparalelos==0 ||
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
        siguienteEquipo( m_lstIP.at( m_lstIpPos ) );
    }
}

//QueriesThreadWorker *QueriesThread::newThreadWorker()
//{
//    return new QueriesThreadWorker;
//}

void QueriesThread::siguienteEquipo(QString ip, bool gw)
{
    qCDebug(queriesthread) << ip << "QueriesThread::siguienteEquipo()"
                           << "GW" << gw
                           << "m_consultarVecinosOSPFMismoDominio" << m_consultarVecinosOSPFMismoDominio;

    Queries *query = new Queries(ip,m_user,m_password,m_linuxprompt);    
    query->setCountry( m_grupo );
    if ( gw )
        query->setGW( m_gw );
    if ( m_consultarVecinosOSPFMismoDominio )
    {
        qCDebug(queriesthreadNeighbors) << ip << "Nuevo Query, agregando ip o interfaz donde viene:"
                                        << m_mapOSPFVecinosInterfazDondeVienen.value( ip );
        query->setIpOInterfazMismoDominioOSPFDondeSeViene( m_mapOSPFVecinosInterfazDondeVienen.value( ip ) );
    }
    query->setConnectionProtocol( m_connectionprotocol );
    query->setUser2( m_user2 );
    query->setPassword2( m_pwd2 );
    query->setQueriesConfiguration( m_queriesconfiguration );
    query->setOptions(m_opciones);

    m_consultaSimultaneos++;
    emit newInformation();

    m_equiposenconsulta.append( ip );
    m_queriesenconsulta.append( query );

    QThread *thr = new QThread;
    QueriesThreadWorker *worker = (*ThreadWorker)();
    worker->setQueries(query);

    worker->moveToThread( thr );
    query->moveToThread( thr );

    connect(thr,SIGNAL(started()),worker,SLOT( start()) );
    connect(worker,SIGNAL(finished(Queries*)),SLOT(equipoConsultado(Queries*)),Qt::QueuedConnection);
    connect(worker,SIGNAL(finished(Queries*)),thr,SLOT(quit()));
    connect(thr,SIGNAL(finished()),thr,SLOT(deleteLater()));
    connect(thr,SIGNAL(finished()),worker,SLOT(deleteLater()));

    thr->start();
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

        qCDebug(queriesthread) << "Analizando si se termina: m_equiposConsultados" << m_equiposConsultados
                               << "m_lstIPsAintentarPorGW" << m_lstIPsAintentarPorGW
                               << "m_equiposPorGWenConsulta" << m_equiposPorGWenConsulta
                               << "m_lstIpPos" << m_lstIpPos
                               << "m_lstIP.size()" << m_lstIP.size()
                               << "m_equiposenconsulta" << m_equiposenconsulta;
                                  ;

        if ( m_equiposConsultados >= m_lstIP.size() &&
             m_lstIPsAintentarPorGW.isEmpty() &&
             !m_equiposPorGWenConsulta )
        {
            qCDebug(queriesthread) << "se termino de consultar los equipos se finaliza";
            emit finished(true);
        }
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
                siguienteEquipo( IP, true );
            }
            else
                if ( m_lstIpPos < m_lstIP.size()-1 )
                {
                    //continuamos con la consulta normal
                    m_lstIpPos++;
                    QString ip = m_lstIP.at( m_lstIpPos );
                    qCDebug(queriesthread) << ip << "siguiente conexion normal";
                    siguienteEquipo( ip );
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
    qCDebug(queriesthreadNeighbors) << qry->ip() << "QueriesThread::validarYagregarVecinoAconsulta"
                                    << "ip" << ip
                                    << "ipOinterfazDondeSeViene" << ipOinterfazDondeSeViene
                                    << "interfazEsteEquipoSalida" << interfazEsteEquipoSalida
                                    << "interfazSiguienteEquipoEntrada" << interfazSiguienteEquipoEntrada
                                    << "ospf area" << m_consultaOSPFArea
                                    << "m_consultarVecinosOSPFMismoDominio" << m_consultarVecinosOSPFMismoDominio
                                    << "m_soloequiposnuevos" << m_soloequiposnuevos
                                    << "\n";

    //que no este en el listado actual a consultar
    //no pasa si solo equipos nuevos y ya esta en el listado de anteriores
    if ( !m_lstIP.contains(ip) &&
         !( m_soloequiposnuevos && lstIPsConsultaAnterior.contains( ip ) ) )
    {
        //que este en el listado de segmentos de links
        if ( !m_lstLinksEnSegmentos.isEmpty() )
        {
            bool encontrado=false;
            for ( QString segmento : m_lstLinksEnSegmentos )
            {
                bool encontrado=false;
                if ( validarIPperteneceAsegmento( qry->interfacesIpAddressesQuery->ipFromInterfaz( interfazEsteEquipoSalida ),
                                                  segmento) )
                {
                    encontrado=true;
                    break;
                }
            }
            if ( !encontrado )
                return;
        }

        //que este en el listado de segmentos de loopbacks
        if ( !m_lstLoopbacksEnSegmentos.isEmpty() )
        {
            bool encontrado=false;
            for ( QString segmento : m_lstLoopbacksEnSegmentos )
            {
                if ( validarIPperteneceAsegmento( ip,segmento) )
                {
                    encontrado=true;
                    break;
                }
            }
            if ( !encontrado )
                return;
        }

        //ospf area
        if ( !m_consultaOSPFArea.isEmpty() )
        {
            QRegExp exp(m_consultaOSPFArea);
            for ( SOSPFInfo &oi : qry->ospfInfo() )
                if ( oi.interfaz == interfazEsteEquipoSalida &&
                     !exp.exactMatch(oi.area) )
                    return;
        }

        //ospf mismo dominio        
        if ( m_consultarVecinosOSPFMismoDominio )
        {
            if ( interfazSiguienteEquipoEntrada.isEmpty() )
            {
                qCDebug(queriesthreadNeighbors) << qry->ip()
                                                << "Activada consulta OSPF mismo dominio pero no se tiene interfazSiguienteEquipoEntrada. Si viene de OSPF hay que agregar a la consulta las IPs de las interfaces";
                return;
            }

            if ( ipOinterfazDondeSeViene.isEmpty() && !m_consultarVecinosOSPFEquipoRaizProceso.isEmpty() )
            {                
                //equipo raiz y se establecio proceso de ospf. Solo las interfaces que pertenecen a ese proceso se recoreran

                qCDebug(queriesthreadNeighbors) << qry->ip()
                                                << "m_consultarVecinosOSPFMismoDominio. Se establecio Proceso inicial y es primer equipo";

                QString interfazHaciaSiguienteEquipo = interfaceToPortChannelInterface(qry->portChannelInfo(),
                                                                                       interfazEsteEquipoSalida,
                                                                                       qry->platform());
                for ( SOSPFInfo &oi : qry->ospfInfo() )
                {
                    if ( oi.interfaz == interfazHaciaSiguienteEquipo )
                        if ( oi.process != m_consultarVecinosOSPFEquipoRaizProceso )
                            return;
                }
            }
            else
            {
                qCDebug(queriesthreadNeighbors) << qry->ip()
                                                << "m_consultarVecinosOSPFMismoDominio. Verificando si interfaces entrada y salida pertenecen al mismo proceso" << ipOinterfazDondeSeViene << interfazEsteEquipoSalida;

                if ( !continuarPorsiguienteInterfazMismoDominioOSPF( *qry,
                                                                 ipOinterfazDondeSeViene,
                                                                 interfazEsteEquipoSalida ) )
                return;
            }
        }

        //se agrega al listado de equipos a consultar
        qCDebug(queriesthreadNeighbors) << qry->ip()
                                        << "Agregando vecino a la consulta" << ip << "interfaz siguiente equipo entrada"
                                        << interfazSiguienteEquipoEntrada;
        m_lstIP.append( ip );
        m_mapOSPFVecinosInterfazDondeVienen.insert( ip, interfazSiguienteEquipoEntrada );

        qCDebug(queriesthreadNeighbors) << qry->ip()
                                        << "Se agrego al listado. Queda asi" << m_lstIP;
    }
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


