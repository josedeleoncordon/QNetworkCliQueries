#include "consultaequipos.h"

#include <QEventLoop>
#include <QDir>
#include "funciones.h"
#include "properties.h"

ConsultaEquipos::ConsultaEquipos(QObject *parent) : QObject(parent)
{
    m_queriesThread = new QueriesThread(this);
    m_consultaConErrores=false;
    m_consultaAgregarVecinos=false;
    m_consultaAnteriorBorrar=false;
    m_consultaOSPFMismoDominio=false;
    m_consultaSoloEquiposNuevos=false;
    m_interval=1000;
    m_simultaneos=3;
    m_maxParalelos=40;
    m_user = Properties::Instance()->user;
    m_pwd = Properties::Instance()->password;
    m_linuxprompt = Properties::Instance()->linuxpromt;
    connect( m_queriesThread,SIGNAL(newInformation()),SIGNAL(queriesThread_newInformation()) );
    connect( m_queriesThread,SIGNAL(finished(bool)),SLOT(on_queriesThread_finished(bool)));
}

ConsultaEquipos::~ConsultaEquipos()
{
//    qDebug() << "ConsultaEquipos::~ConsultaEquipos()" << 11;
//    if ( _lstQN )
//    {
//        qDebug() << "ConsultaEquipos::~ConsultaEquipos()" << 22 << _lstQN;
//        qDeleteAll(_lstQN->lstQueries); //solo borrar lstQueries de uno (lstQN) ya que lstQ tambien lo comparte
//        delete _lstQN;
//    }
//    if ( _lstQ )
//    {
//        qDebug() << "ConsultaEquipos::~ConsultaEquipos()" << 33;
//        delete _lstQ;
//    }
}

void ConsultaEquipos::setLstQ(LstQueries &lQ)
{
    if ( lQ.isEmpty() )
        return;

    _lstQ = lQ;
    lstQueriesParameters = _lstQ.lstQueriesParameters;
    lstQueries = _lstQ.lstQueries;
    errorMap = _lstQ.errorMap;
    m_lstLabel = _lstQ.label;
    m_lstTipo = _lstQ.tipoconsulta;
    m_grupo = _lstQ.grupo;
    for ( Queries &q : lstQueries )
        lstIPAnteriores.append( q.ip() );
}

void ConsultaEquipos::addOpcionesConsulta(QList<int> lst)
{
	lst.removeAll( QueryOpcion::Null ); //unicamente opciones validas
	opciones.append( lst );
}

void ConsultaEquipos::addParametrosConsulta(const QList<QueriesConfigurationValue> &c)
{
	m_queriesConfiguration.addQueryParameter(c);
}

void ConsultaEquipos::consultarEquiposSync()
{
    disconnect(); //desconectamos todas las señales antes configuradas
    QEventLoop loop;
    connect(this, SIGNAL(finished()), &loop, SLOT(quit()));
    connect(this, SIGNAL(error()), &loop, SLOT(quit()));
    consultarEquipos();
    loop.exec();
}

void ConsultaEquipos::consultarEquipos()
{
    if ( !m_lstQpath.isEmpty() )
        m_lstQfilepath = buscarArchivoConsultasActual(m_lstQpath,m_lstLabel,m_grupo);

    //consulta anterior
    QStringList lstAnteriores;
    if (!m_lstQfilepath.isEmpty())
    {
        _lstQ = openLstQueries( m_lstQfilepath );
        m_lstLabel = _lstQ.label;
        m_lstTipo = _lstQ.tipoconsulta;
        m_grupo = _lstQ.grupo;
        lstQueriesParameters = _lstQ.lstQueriesParameters;
        lstQueries = _lstQ.lstQueries;
        errorMap = _lstQ.errorMap;
        for ( Queries &q : lstQueries )
            lstAnteriores.append( q.ip() );
    }

    //opciones de consulta
	if ( opciones.isEmpty() )
        opciones = _lstQ.opcionesConsulta; //si no se configuro opciones se usa la de la consulta anterior
	if ( opciones.isEmpty() )
    {
        _error = "Opciones de consulta no configuradas";
        emit error();
        return;
    }

    //Equipos a consultar
    if ( !m_consultaConErrores )
    {
        //consulta normal

        if ( !m_lstIPsPath.isEmpty() )
            lstIP.append( openFile( m_lstIPsPath ).split("\n",Qt::SkipEmptyParts) );
        if ( !m_ip.isEmpty() )
            lstIP.append(m_ip);
        if ( lstIP.isEmpty() ) //si ninguna de las dos anteriores se configuro usamos la de la consulta anterior
            lstIP = lstAnteriores;
    }
    else
    {
        //consulta de equipos anteriores con errores

        if ( _lstQ.isEmpty() )
        {
            _error = "No se configuro archivo de consultas anteriores";
            emit error();
            return;
        }
        if ( !m_ip.isEmpty() )
        {
            _error = "Se especifico IP de consulta junto con equipos con errores anteriores";
            emit error();
            return;
        }

        QMapIterator<QString, QString> i(errorMap);
        while (i.hasNext())
        {
            i.next();
            if ( i.value() == "No conexion" ||
                 i.value() == "Conexion Error" ||
                 i.value() == "Error" )
                lstIP.append( i.key() );
        }
    }

    if ( lstIP.isEmpty() )
    {
        qDebug() << "Sin listado de equipos a consultar";
        _error = "Sin listado de equipos a consultar";
        emit error();
        return;
    }

    qDebug() << "consultaequipos: IPs a consultar" << lstIP;

    m_queriesThread->setConnectionProtocol( m_connectionprotocol );
    m_queriesThread->setGrupo(m_grupo);
    m_queriesThread->setLstIP( lstIP );
    m_queriesThread->setGW( m_gw );
    m_queriesThread->setUser( m_user );
    m_queriesThread->setPassword( m_pwd );
    m_queriesThread->setUser2( m_user2 );
    m_queriesThread->setPassword2( m_pwd2 );
    m_queriesThread->setLinuxPrompt( m_linuxprompt );
	m_queriesThread->setOptions( opciones );
    m_queriesThread->setEquipmentNeighborsConsultarVecinos( m_consultaAgregarVecinos );
    m_queriesThread->setEquipmentNeighborsOSPFMismoDominio( m_consultaOSPFMismoDominio );
	m_queriesThread->setEquipmentNeighborsRegExpOSPFArea( m_consultaOSPFArea );
    m_queriesThread->setConsultaAgregarVecinosLinksEnSegmentos( m_lstLinksEnSegmentos );
    m_queriesThread->setConsultaAgregarVecinosLoopbacksEnSegmentos( m_lstLoopbacksEnSegmentos );
    m_queriesThread->setInterval( m_interval );
    m_queriesThread->setSimultaneos( m_simultaneos );
    m_queriesThread->setMaxParalelos( m_maxParalelos );
	m_queriesThread->setQueriesConfiguration( m_queriesConfiguration );

    //paramentros de consulta
	m_queriesConfiguration.addQueryParameter(lstQueriesParameters);

    //iniciando consulta
    m_queriesThread->iniciar();
}

QString ConsultaEquipos::newInformationHumanReadable()
{
    return m_grupo + ": Equipo "+QString::number(m_queriesThread->equiposConsultados())+
            " de "+QString::number(m_queriesThread->equiposTotal())+
            "\n\tExitosos: "+QString::number(m_queriesThread->equiposExitosos())+
            "\n\tConsulta con errores: "+QString::number(m_queriesThread->equiposConErrores())+
            "\n\tConexion con errores: "+QString::number(m_queriesThread->equiposConexionConErrores())+
            "\n\tSin conexion: "+QString::number(m_queriesThread->equiposSinConexion());
}

void ConsultaEquipos::on_queriesThread_finished(bool ok)
{
    if ( !ok )
    {
        m_queriesThread->deleteLater();        
        _error = "QueriesThread error";
        emit error();
        return;
    }

    QMap<QString, QString> errormapaintegrado;
    QMap<QString, QString> errormap_nuevo = m_queriesThread->errorMap();
    lstQueriesConsultados = m_queriesThread->lstQueries();
    if ( !_lstQ.isEmpty() )
    {
        //actualizando consultar anterior
        Queries::updateInfoQueries( lstQueries, lstQueriesConsultados, lstIP );
        QMap<QString, QString> errormap_nuevo = m_queriesThread->errorMap();
        errormapaintegrado = updateInfoMapError( errorMap,errormap_nuevo,lstIP );
    }
    else
    {
        //consulta nueva
        errormapaintegrado = errormap_nuevo;
        lstQueries = lstQueriesConsultados;
    }

    qDebug() << "lstQueries size" << lstQueries.size() << m_grupo << m_lstLabel << m_lstQpath;

    _lstQN.grupo = m_grupo;
    _lstQN.label = m_lstLabel;
    _lstQN.opcionesConsulta = opciones;
    _lstQN.lstQueries = lstQueries;
    _lstQN.errorMap = errormapaintegrado;
	_lstQN.lstQueriesParameters = m_queriesConfiguration.lstQueryParameters();
    _lstQN.tipoconsulta = m_lstTipo;
    _lstQN.lstIPsAconsultadas = lstIP;
    _lstQN.dateTime = QDateTime::currentDateTime();

    //eliminando consulta anterior si fue configurado
    if ( m_consultaAnteriorBorrar )
    {
        QFile file( m_lstQfilepath );
        file.remove();
    }

    //guardando consulta
    if ( !m_lstQpath.isEmpty() )
        saveLstQueries( _lstQN, m_lstQpath+"/"+ nombreArchivoLstQueries(_lstQN) );

    emit finished();        
}

