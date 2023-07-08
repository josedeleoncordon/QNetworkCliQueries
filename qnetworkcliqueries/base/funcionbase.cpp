#include "funcionbase.h"
#include "queries.h"

#include <QEventLoop>

QueriesConfigurationValue::QueriesConfigurationValue(QString key,
                                                     QString value,
                                                     QString IP,
                                                     QString plataforma,
                                                     QString IDConexion,
                                                     bool appendValue)
{
    _key = key;
    _value = value;
    _IP = IP;
    _Platform = plataforma;
    _appendValue = appendValue;
    _IDConexion = IDConexion;
}

QueriesConfigurationValue::QueriesConfigurationValue(const QueriesConfigurationValue &other)
{
    _key = other._key;
    _value = other._value;
    _IP = other._IP;
    _Platform = other._Platform;
    _appendValue = other._appendValue;
    _IDConexion = other._IDConexion;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const QueriesConfigurationValue& qcv)
{
    //guardar
    out << qcv._IP;
    out << qcv._Platform;
    out << qcv._key;
    out << qcv._value;
    out << qcv._appendValue;
    out << qcv._IDConexion;
    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, QueriesConfigurationValue& qcv)
{
    //abrir
    in >> qcv._IP;
    in >> qcv._Platform;
    in >> qcv._key;
    in >> qcv._value;
    in >> qcv._appendValue;
    in >> qcv._IDConexion;
    return in;
}

QNETWORKCLIQUERIES_EXPORT QDebug operator<<(QDebug dbg, const QueriesConfigurationValue &info)
{
     dbg.space() << info._key << info._value << info._IP << info._Platform << info._appendValue << info._IDConexion;
     return dbg;
}

QueriesConfiguration::QueriesConfiguration()
{}

void QueriesConfiguration::addQueryParameter(const QList<QueriesConfigurationValue>& c)
{
    //buscamos si ya existe, si si lo adjunta al existente, si no, se agrega completo
    for ( QueriesConfigurationValue n : c )
    {
        bool encontrado=false;
        for ( QueriesConfigurationValue &e : m_lstQueryParameters )
        {
            if ( n._IP == e._IP &&
                 n._Platform == e._Platform &&
                 n._key == e._key &&
                 n._IDConexion == e._IDConexion )
            {
                encontrado=true;
                if ( n._appendValue ) e._value.append(","+n._value); else e._value = n._value;
                break;
            }
        }
        if ( !encontrado )
            m_lstQueryParameters.append(n);
    }
}

QVariant QueriesConfiguration::m_find(QString parameter, QString IP, QString platform, QString IDconexion)
{        
    //Por IP, Plataforma especifica, ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if ( v._key == parameter && v._IP == IP && v._Platform == platform && v._IDConexion == IDconexion  )
            return v._value;

    //por IP especifica e ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )     
        if ( v._key == parameter && v._IP == IP && v._Platform.isEmpty() && v._IDConexion == IDconexion  )
            return v._value;

    //plataforma e ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if ( v._key == parameter && v._IP.isEmpty() && v._Platform == platform && v._IDConexion == IDconexion  )
            return v._value;

    //no hay para una ip o plataforma, regresamos el general con ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if ( v._key == parameter && v._IP.isEmpty() && v._Platform.isEmpty() && v._IDConexion == IDconexion )
            return v._value;

    //Por IP, Plataforma especifica, Sin ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if ( v._key == parameter && v._IP == IP && v._Platform == platform && v._IDConexion.isEmpty()  )
            return v._value;

    //por IP especifica sin ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if ( v._key == parameter && v._IP == IP && v._Platform.isEmpty() && v._IDConexion.isEmpty()  )
            return v._value;

    //plataforma sin ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if ( v._key == parameter && v._IP.isEmpty() && v._Platform == platform && v._IDConexion.isEmpty()  )
            return v._value;

    //no hay para una ip o plataforma, regresamos el general sin ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if ( v._key == parameter && v._IP.isEmpty() && v._Platform.isEmpty() && v._IDConexion.isEmpty() )
            return v._value;

    return QVariant();
}

bool QueriesConfiguration::state(QString parameter, QString IP, QString platform, QString IDconexion)
{
    return m_find(parameter,IP,platform,IDconexion).toBool();
}

QString QueriesConfiguration::value(QString parameter, QString IP, QString platform, QString IDconexion)
{
    return m_find(parameter,IP,platform,IDconexion).toString();
}

QStringList QueriesConfiguration::values(QString parameter, QString IP, QString platform, QString IDconexion)
{
    return value(parameter,IP,platform,IDconexion).split(",",QString::SkipEmptyParts);
}

void QueriesConfiguration::clear()
{
    m_lstQueryParameters.clear();
    m_lstConfiguration.clear();
}

FuncionBase::FuncionBase()
{
    init();
}

FuncionBase::FuncionBase(QRemoteShell *terminal, int option)
{
    init();
    m_queryoption=option;
    term=terminal;
}

FuncionBase::~FuncionBase()
{}

void FuncionBase::init()
{
    term=nullptr;
    lastCommandFailed=false;
    m_xr64=false;
    m_brand="Cisco";
    m_queryoption=QueryOpcion::Null;
    exp.setMinimal(true);
    exp2.setMinimal(true);
    m_parentQuery=nullptr;
}

void FuncionBase::setPlatform(QString platform)
{
    m_platform=platform;
    m_os=equipmentOSFromPlatform(m_platform);        
    qDebug() << "FuncionBase::setPlatform platform os" << platform << m_os;
}

void FuncionBase::setParentQuery(Queries *qry)
{
    m_parentQuery = qry;
    m_queriesConfiguration = qry->queriesConfiguration();
    m_conexionID = qry->conexionID();
}

void FuncionBase::termSendText(QString str) //Async
{
    txt.clear();
    lastCommandFailed=false;
    m_lastCommand = str;
    term->sendCommand( str );
    emit lastCommand(m_lastCommand);
}

void FuncionBase::termSendTextSync(QString str) //Sync
{
    disconnect();
    QEventLoop loop;
    connect(term, SIGNAL(readyRead()), SLOT(m_on_term_receiveText()));
    connect(this, SIGNAL(m_termReceivedFinished()), &loop, SLOT(quit()));
    termSendText(str);
    loop.exec();
}

void FuncionBase::m_on_term_receiveText()
{
    qDebug() << "FuncionBase::m_on_term_receiveText() sync";

    //cuando es sync
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    emit m_termReceivedFinished();
}

bool FuncionBase::allTextReceived()
{
    emit working();    

    //eliminamos caracteres especiales ansi vt100 al cortar por paginas la informacion
    //en el caso de que no sea un cisco y no se haya configurado algo como terminal lenght 0
    //---- More ----[42D                                          [42D
    txt = eliminarCaractaresNoImprimibles( txt );
    exp.setPattern("\\[42D.+\\[42D");
    txt.replace( exp,"" );

    if ( txt.right(9).contains( "--More--" ) ||
         txt.right(15).contains("---- More ----") ||
         txt.right(7).contains("more.."))
    {
//        qDebug() << "Enviando espacio para continuar";
        QByteArray a(1,32);
        term->sendData( a );
        return false;
    } 

    //hasta que miremos el prompt del equipo sabremos que se finalizo de recibir texto

    QStringList data = txt.split("\n");

    exp.setPattern("^\\S+#\\s*$");
    exp2.setPattern("^<\\S+>");
    if ( (!data.last().simplified().contains(exp) &&
          !data.last().simplified().contains(exp2))
         || data.contains("Description: ") )
        return false;

    txt.replace("---- More ----","");
    txt.replace("--More--","");
    txt.replace("more..","");

    if ( txt.contains("Invalid input detected at '^'") ||
         txt.contains(QRegExp("Translating .+domain server")) )
    {
        lastCommandFailed=true;
//        qDebug() << "**ErrorMenor**" << m_ip << m_name << m_platform << "No se pudo ejecutar comando:" << m_lastCommand;
    }
    else
        lastCommandFailed=false;

//    qDebug() << "true" << m_lastCommand << lastCommandFailed;

    return true;
}

void FuncionBase::finished()
{
//    qDebug() << "FuncionBase::finished()";

    txt.clear();
    term->disconnectReceiveTextSignalConnections();
    emit processFinished();
}
