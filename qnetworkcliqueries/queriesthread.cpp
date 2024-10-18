#include "queriesthread.h"
#include "funciones.h"
#include "properties.h"
#include "qnetworkquerieslogging.h"
#include "queriesthreadcontroller.h"
#include "wcuuids.h"

#include <QEventLoop>

QueriesThread::QueriesThread(QObject *parent) : QObject(parent)
{
    m_interval=1000;
    m_simultaneos=3;
    m_maxparalelos=20;
    m_maxparalelosmismoequipo=1;
    m_lstIPsMismoEquipoCounter=0;
    m_lstIPsCounter=0;
    m_connectionprotocol = QRemoteShell::SSH;
    m_principaluserfirst=true;
    m_soloequiposnuevos=false;
    m_consultarVecinosOSPFMismoDominio=false;
    m_equipmentNeighborsConsultarVecinos=false;
    m_eliminarconsultasdeequiposduplicados=true;
    m_timer = nullptr;
    m_timerActivity = nullptr;
    ThreadWorker = nullptr;
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

void QueriesThread::_emitNewInformation()
{
    emit newInformation();
    qDebug() << Q_FUNC_INFO << m_uuid<< "Equipos en consulta: " + QString::number( equiposEnConsulta().size() )+
                " Equipos consultados: "+ QString::number( equiposConsultados() )+" de "+
                QString::number( m_lstIPsMismoEquipoCounter + m_lstIPsCounter );
//    emit status( m_uuid,"Equipos en consulta: " + QString::number( equiposEnConsulta().size() )+
//                 " - Equipos consultados: "+ QString::number( equiposConsultados() )+" de "+
//                 QString::number( m_lstIPsMismoEquipoCounter + m_lstIPsCounter ) );
    qInfo() << "Equipos en consulta: " + QString::number( equiposEnConsulta().size() )+
                   " Equipos consultados: "+ QString::number( equiposConsultados() )+" de "+
                   QString::number( m_lstIPsMismoEquipoCounter + m_lstIPsCounter );
    // qInfo() << "Equipos en consulta" << m_equiposenconsulta;

    emit status( m_uuid,"Equipos consultados: "+ QString::number( equiposConsultados() )+" de "+
                 QString::number( m_lstIPsMismoEquipoCounter + m_lstIPsCounter ) );
}

void QueriesThread::_emitFinished(bool ok)
{
    QueriesThreadController::Instance()->unregisterQueriesThreadUUID( m_uuid );
    emit status(m_uuid,"");
    emit finished(ok);
}

void QueriesThread::_clear()
{
    m_consultaSimultaneos=0;    
    m_conerrores=0;
    m_sinconexion=0;
    m_equiposExitosos=0;
    m_conexionerrores=0;    
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

void QueriesThread::setLstIP(QStringList lst)
{
    m_lstIP = lst;
    verificarLstIPsQueriesConfiguration();
}

void QueriesThread::setQueriesConfiguration(QueriesConfiguration configuration)
{
    m_queriesconfiguration=configuration;
    verificarLstIPsQueriesConfiguration();
}

void QueriesThread::verificarLstIPsQueriesConfiguration()
{
    if ( m_lstIP.isEmpty() )
        return;

    m_lstIPsMismoEquipo.clear();
    //verificamos los IDs para las configuraciones y agregamos las IPs con el ID
    //172.16.30.253 -> 172.16.30.253_1, 172.16.30.253_2, 172.16.30.253_3
    for ( QueriesConfigurationValue value : m_queriesconfiguration.lstQueryParameters() )
    {
        if ( !value._IDConexion.isEmpty() )
        {
            QRegularExpression exp(QRegularExpression::anchoredPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}"));
            if ( value._IP.contains(exp) )
            {
                m_lstIPsMismoEquipo.append(value._IP+"_"+value._IDConexion);
                m_lstIP.removeOne(value._IP);
            }
        }
    }
    m_lstIPsMismoEquipoCounter = m_lstIPsMismoEquipo.size();
    m_lstIPsCounter = m_lstIP.size();

    qCDebug(queriesthread) << "QueriesThread::verificarLstIPsQueriesConfiguration. m_lstIPs" << m_lstIP <<
                              "m_lstIPsMismoEquipo" << m_lstIPsMismoEquipo;
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
         (m_lstIP.isEmpty() && m_lstIPsMismoEquipo.isEmpty() ) ||
         m_equiposConsultados.size() > 0 )
    {
        qCDebug(queriesthread) << "QueriesThread::iniciar() ed " <<
                                  m_interval <<
                                  m_simultaneos <<
                                  m_maxparalelos <<
                                  m_maxparalelos <<
                                  m_lstIPsMismoEquipo <<
                                  m_equiposConsultados ;

        _emitFinished(false);
        return;
    }

    if ( m_equipmentNeighborsConsultarVecinos )
        QueriesThreadController::Instance()->registerQueriesThreadUUID( m_uuid );
    else
        QueriesThreadController::Instance()->registerQueriesThreadUUID( m_uuid, m_lstIP.size() + m_lstIPsMismoEquipo.size() );        

    connect(WCUUIDs::Instance(),SIGNAL(uuidCanceled(QString)),SLOT(on_uuidCanceled(QString)));

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
    //No usar si se desea emitir señales de status a media consulta

    qDebug() << Q_FUNC_INFO;
    disconnect(); //desconectamos todas las señales antes configuradas
    QEventLoop loop;
    connect(this, SIGNAL(finished(bool)), &loop, SLOT(quit()));
    iniciar();
    loop.exec();
    qDebug() << Q_FUNC_INFO << "saliendo del loop";
}

void QueriesThread::detener()
{
    m_timer->stop();
    m_detener=true;
    m_equipmentNeighborsConsultarVecinos=false;
}

void QueriesThread::cancelar()
{
    //TODO Controlar la eliminacion de qry cuando se usa un worker personalizado

    qDebug() << Q_FUNC_INFO;
    m_timer->stop();
    m_cancelar=true;
    foreach (Queries* qry, m_queriesenconsulta)
    {
        QueriesThreadController::Instance()->consultaFinalizada(m_uuid,qry->ip());
        m_conerrores++;
        m_errorMap.insert( qry->ip(), "Error" );
        qry->disconnect();
        qry->thread()->quit();
        qry->deleteLater();
    }
    m_queriesenconsulta.clear();
    m_equiposenconsulta.clear();

    disconnect(WCUUIDs::Instance(),SIGNAL(uuidCanceled(QString)),this,SLOT(on_uuidCanceled(QString)));

    _emitFinished(true);
    emit canceled(m_uuid);
}

void QueriesThread::on_timerActivity_timeOut()
{
    qDebug() << Q_FUNC_INFO;
    cancelar();
}

void QueriesThread::on_uuidCanceled(QString uuid)
{
    qCDebug(queriesthread) << uuid;
    qDebug() << Q_FUNC_INFO << uuid;
    if ( m_uuid == uuid )
        cancelar();
}

void QueriesThread::on_timer_timeOut()
{
    if ( m_consultaSimultaneos >= m_maxparalelos )
        return;

    qCDebug(queriesthread) << "m_lstIP.size()" << m_lstIP.size()
                           << "m_lstIPsMismoEquipo.size()" << m_lstIPsMismoEquipo.size();

    if ( m_lstIP.isEmpty() && m_lstIPsMismoEquipo.isEmpty() )
    {
        //ya se llego al final
        qCDebug(queriesthread) << "QueriesThread::on_timer_timeOut() ya se llego al final";
        m_timer->stop();
        return;
    }

    bool mismoequipo=false;
    if ( !m_lstIPsMismoEquipo.isEmpty() )
    {
        //tratamos de conectar un equipo con diferentes consultas en direferentes conexiones
        for ( QString ip_idconexion : m_lstIPsMismoEquipo )
            if ( conectarSiguienteMismoEquipo(ip_idconexion) )
            {
                //se puede conectar a otro del mismo equipo
                qCDebug(queriesthread) << ip_idconexion << "siguiente mismo equipo";
                mismoequipo=true;
                m_lstIPsMismoEquipo.removeOne( ip_idconexion );
                siguienteEquipo( ip_idconexion );
                break;
            }
    }
    if ( !mismoequipo )
        if ( !m_lstIP.isEmpty() )
        {
            //continuamos con la consulta normal
            qCDebug(queriesthread) << "QueriesThread::on_timer_timeOut() continuamos con la consulta normal";

            short int c=0;
            short int conectados = 0;
            forever
            {                                
                QString ip = m_lstIP.at(c);
                if ( QueriesThreadController::Instance()->consultarAgregarEquipo(m_uuid,ip ) )
                {
                    m_lstIP.removeAt(c);
                    conectados++;
                    siguienteEquipo( ip );
                }
                else
                    c++;

                if ( c >= m_lstIP.size() || conectados == m_simultaneos )
                    break;
            }
        }
}

//QueriesThreadWorker *QueriesThread::newThreadWorker()
//{
//    return new QueriesThreadWorker;
//}

bool QueriesThread::conectarSiguienteMismoEquipo(QString IP)
{
    QString ip;
    if ( IP.contains("_") )
        ip = IP.split("_").first();
    else
        return false;

    if ( !QueriesThreadController::Instance()->consultarAgregarEquipo(m_uuid, ip ) )
        return false;

    return true;

//    int c=0;
//    for ( QString ipequipo : m_equiposenconsulta )
//        if ( ip == ipequipo )
//            c++;

//    if ( c < m_maxparalelosmismoequipo )
//    {
//        qCDebug(queriesthread) << "QueriesThread::conectarSiguienteMismoEquipo" << ip <<
//                                  "Consultas actuales" << c << "m_maxparalelosmismoequipo" << m_maxparalelosmismoequipo << "TRUE";
//        return true;
//    }
//    else
//    {
//        qCDebug(queriesthread) << "QueriesThread::conectarSiguienteMismoEquipo" << ip <<
//                                  "Consultas actuales" << c << "m_maxparalelosmismoequipo" << m_maxparalelosmismoequipo << "FALSE";
//        return false;
//    }
}

void QueriesThread::siguienteEquipo(QString IP, bool gw)
{
    qCDebug(queriesthread) << IP << "QueriesThread::siguienteEquipo()"
                           << "GW" << gw
                           << "m_consultarVecinosOSPFMismoDominio" << m_consultarVecinosOSPFMismoDominio;

    QString IDConexion;
    QString ip;
    if ( IP.contains("_") )
    {
        ip = IP.split("_").first();
        IDConexion = IP.split("_").last();
    }
    else
        ip = IP;

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
    query->setConexionID(IDConexion);
    query->setConnectionProtocol( m_connectionprotocol );
    if ( !lstRemoteShellUsersPasswords.isEmpty() )
        query->setRemoteShellUsersPasswords( lstRemoteShellUsersPasswords );
    query->setUser2( m_user2 );
    query->setPassword2( m_pwd2 );
    query->setQueriesConfiguration( m_queriesconfiguration );
    query->setOptions(m_opciones);

    m_consultaSimultaneos++;    

    m_equiposenconsulta.append( ip );
    m_queriesenconsulta.append( query );

    _emitNewInformation();

    QThread *thr = new QThread;
    QueriesThreadWorker *worker = (*ThreadWorker)();
    worker->setQueries(query);
    worker->setUUID(m_uuid);

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
    bool eliminar=false; //si es necesario borrar el qry

    if ( m_cancelar )
        return;

    qCDebug(queriesthread) << qry->ip() << "QueriesThread::equipoConsultado 1111"
            << qry->hostName() << "conexionID" << qry->conexionID();
    qCDebug(queriesthread) << "QueriesThread::equipoConsultado 1111"
             << qry->ip() << qry->hostName() << "conexionID" << qry->conexionID();

    QueriesThreadController::Instance()->consultaFinalizada(m_uuid,qry->ip());

    m_timerActivity->stop();
    m_timerActivity->start();

    m_consultaSimultaneos--;

    m_equiposenconsulta.removeOne( qry->ip() );
    m_queriesenconsulta.removeOne( qry );

    if ( qry->gw().isEmpty() )
        m_equiposConsultados.append(qry->ip());
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
            if ( m_eliminarconsultasdeequiposduplicados )
            {
                for (Queries *q : m_lstQueries)
                {
                    if ( qry->hostName() == q->hostName() &&
                        !qry->hostName().isEmpty() &&
                        qry->conexionID() == q->conexionID() )
                    {
                        qCDebug(queriesthread) << qry->ip() <<
                            "QueriesThread::equipoConsultado -- Ya existe un equipo en listado. Se elimina qry";
                        qCDebug(queriesthread) << "QueriesThread::equipoConsultado -- Ya existe un equipo en listado. Se elimina qry";
                        encontrado=true;
                        eliminar=true;
                        break;
                    }
                }
            }
            if ( !encontrado )
            {
                qCDebug(queriesthread) << qry->ip() <<
                                          "QueriesThread::equipoConsultado -- Se agrega a listado de lstQueries";
                qCDebug(queriesthread) << "QueriesThread::equipoConsultado -- Se agrega a listado de lstQueries";
                m_lstIPsConectadosPorGW.append( qry->ip() );
                m_lstQueries.append( qry );
            }
        }
        else
        {
            m_conerrores++;
            qCDebug(queriesthread) << qry->ip() << "QueriesThread::equipoConsultado -- Consulta con errores";
            qCDebug(queriesthread) << "QueriesThread::equipoConsultado -- consulta con errores" << qry->ip();
            m_errorMap.insert( qry->ip(), "Error"+(!qry->errorTxt().isEmpty()?" "+qry->errorTxt():"") );
            eliminar=true;
        }
    }
    else
    {
        if ( qry->isReachable() )
        {
            qCDebug(queriesthread) << qry->ip() << "QueriesThread::equipoConsultado -- Conexion Error";
            qCDebug(queriesthread) << "QueriesThread::equipoConsultado -- Conexion Error" << qry->ip();
            m_errorMap.insert( qry->ip(), "Conexion Error"+(!qry->errorTxt().isEmpty()?" "+qry->errorTxt():"") );
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
                m_errorMap.insert( qry->ip(), "No alcanzable" );
                m_sinconexion++;
                eliminar=true;
            }
        }

//        //si la plataforma y el nombre no estan vacios significa que se pudo consultar la informacion por SNMP
//        //por lo que no lo borramos para que pueda estar disponible a la hora de graficar
//        if ( qry->platform().isEmpty() ||
//             qry->hostName().isEmpty() )
//            eliminar=true;
//        else
//            m_lstQueries.append( qry );
    }        

    _emitNewInformation();

    if ( !m_uuid.isEmpty() )
    {
        if ( !WCUUIDs::Instance()->isAlive( m_uuid ) && !m_detener )
        {
            qCDebug(queriesthread) << "QueriesThread uuid no sigue vivo. Se cancela";
            cancelar();
            return;
        }
    }

    qCDebug(queriesthread) << qry->ip() << "Qthread *****";

    if ( !m_detener )
    {
        //Si se termino de consultar los equipos se finaliza

        // qCDebug(queriesthread) << "Analizando si se termina: m_equiposConsultados" << m_equiposConsultados
        //                        << "m_lstIPsAintentarPorGW" << m_lstIPsAintentarPorGW
        //                        << "m_equiposPorGWenConsulta" << m_equiposPorGWenConsulta
        //                        << "m_lstIP.size()" << m_lstIP.size()
        //                        << "m_equiposenconsulta" << m_equiposenconsulta;

        if ( m_lstIP.isEmpty() &&
             m_lstIPsAintentarPorGW.isEmpty() &&
             m_lstIPsMismoEquipo.isEmpty() &&
             !m_equiposPorGWenConsulta &&
             m_equiposenconsulta.isEmpty() )
        {
            qCDebug(queriesthread) << "queriesthread se termino de consultar los equipos se finaliza" <<
                                      m_equiposConsultados <<
                                      m_lstIP.size() <<
                                      m_lstIPsAintentarPorGW <<
                                      m_lstIPsMismoEquipo <<
                                      m_equiposPorGWenConsulta <<
                                      m_equiposenconsulta;

            //Error MAP. Eliminar los errores de equipos que en segundas consultas fueron exitosas
            QMapIterator<QString,QString> imap(m_errorMap);
            QStringList lstEliminar;
            while ( imap.hasNext() )
            {
                imap.next();
                for ( Queries *qry : m_lstQueries )
                {
                    if ( imap.key() == qry->ip() )
                    {
                        lstEliminar.append( imap.key() );
                        break;
                    }
                }
            }
            for ( QString ip : lstEliminar )
                m_errorMap.remove(ip);

            _emitFinished(true);
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
            {
                bool mismoequipo=false;
                if ( !m_lstIPsMismoEquipo.isEmpty() )
                {
                    //tratamos de conectar un equipo con diferentes consultas en direferentes conexiones
                    QString ip_idconexion = m_lstIPsMismoEquipo.first();
                    if ( conectarSiguienteMismoEquipo(ip_idconexion) )
                    {
                        qCDebug(queriesthread) << ip_idconexion << "siguiente mismo equipo";
                        mismoequipo=true;
                        m_lstIPsMismoEquipo.takeFirst();
                        siguienteEquipo( ip_idconexion );
                    }
                }
                if ( !mismoequipo )
                    if ( !m_lstIP.isEmpty() )
                    {
                        //continuamos con la consulta normal
                        QString ip = m_lstIP.at(0);
                        if ( QueriesThreadController::Instance()->consultarAgregarEquipo( m_uuid,ip ) )
                        {
                            m_lstIP.removeAt(0);
                            qCDebug(queriesthread) << ip << "siguiente conexion normal";
                            siguienteEquipo( ip );
                        }
                    }
            }
        }
    }
    else
    {
        if ( m_consultaSimultaneos == 0 )
        {
            qCDebug(queriesthread) << "equipoConsultado m_consultaSimultaneos==0";
            _emitFinished(true);
        }
    }

    qDebug() << qry->ip() << "QueriesThread::equipoConsultado FIN";//no con qCDebug ya que para cerrar el archivo debug del equipo es necesario est en QueriesMessageHandler. No Borrar. No usar con qCDebug ya que podria ser deshabilitado.

    if ( eliminar )
        qry->deleteLater();
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
    if ( !m_equiposConsultados.contains(ip) && !m_lstIP.contains(ip) &&
         !( m_soloequiposnuevos && lstIPsConsultaAnterior.contains( ip ) ) )
    {
        //que este en el listado de segmentos de links
        if ( !m_lstLinksEnSegmentos.isEmpty() )
        {
            bool encontrado=false;
            for ( QString segmento : m_lstLinksEnSegmentos )
            {
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
            QRegularExpression exp(QRegularExpression::anchoredPattern(m_consultaOSPFArea));
            for ( SOSPFInfo &oi : qry->ospfInfo() )
            {
                QRegularExpressionMatch match = exp.match(oi.area);
                if (oi.interfaz == interfazEsteEquipoSalida &&
                    !match.hasMatch())
                    return;
            }
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
        m_lstIPsCounter++;
        m_mapOSPFVecinosInterfazDondeVienen.insert( ip, interfazSiguienteEquipoEntrada );

        qCDebug(queriesthreadNeighbors) << qry->ip()
                                        << "Se agrego al listado. Queda asi" << m_lstIP;
    }
}

QList<Queries> QueriesThread::lstQueries()
{
    QList<Queries> lst;

    for ( Queries *qry : m_lstQueries )
    {
        qDebug() << "QueriesThread::lstQueries()" << qry->ip() << "listo para clonar";
        lst.append(*qry);
    }

    return lst;
}

QList<Queries*> QueriesThread::lstQueriesPointers()
{
    return m_lstQueries;
}

Queries QueriesThread::queriesFromIP(QString ip)
{
    Queries qrysalida;
    for ( Queries *qry : m_lstQueries )
        if ( qry->ip() == ip )
        {
            qrysalida = *qry;
            return qrysalida;
        }

    return qrysalida;
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


