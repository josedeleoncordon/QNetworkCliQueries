#include "funcionbase.h"
#include "queries.h"

#include <QEventLoop>

QueriesConfigurationValue::QueriesConfigurationValue(QString key,
                                                     QString value,
                                                     QString IP,
                                                     QString plataforma,
                                                     QString IDConexion,
                                                     QString queryName,
                                                     bool appendValue)
{
    _key = key;
    _value = value;
    _IP = IP;
    _Platform = plataforma;
    _appendValue = appendValue;
    _IDConexion = IDConexion;
    _queryName = queryName;
}

QueriesConfigurationValue::QueriesConfigurationValue(const QueriesConfigurationValue &other)
{
    clone(other);
}

QueriesConfigurationValue& QueriesConfigurationValue::operator =(const QueriesConfigurationValue &other)
{
    clone(other);
    return *this;
}

void QueriesConfigurationValue::clone(const QueriesConfigurationValue &other)
{
    _key = other._key;
    _value = other._value;
    _IP = other._IP;
    _Platform = other._Platform;
    _appendValue = other._appendValue;
    _IDConexion = other._IDConexion;
    _queryName = other._queryName;
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
    out << qcv._queryName;
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
    in >> qcv._queryName;
    return in;
}

QNETWORKCLIQUERIES_EXPORT QDebug operator<<(QDebug dbg, const QueriesConfigurationValue &info)
{
     dbg.space() << info._key << info._value << info._IP << info._Platform
                << info._appendValue << info._IDConexion << info._queryName;
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
                 n._IDConexion == e._IDConexion &&
                 n._queryName == e._queryName
                )
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

QVariant QueriesConfiguration::m_find(QString parameter, QString IP, QString platform,
                                      QString IDconexion, QString queryName,bool platformContains)
{
    qDebug() << IP << "QueriesConfiguration::m_find" << parameter << IP << platform << IDconexion << queryName;

    //Por IP, Plataforma especifica, ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
    {
        qDebug() << IP << "QueriesConfiguration::m_find buscando" << v._key << v._IP << v._Platform << v._IDConexion;
        if ( v._key == parameter && v._IP == IP &&
             ((!platformContains && v._Platform == platform) || (platformContains && platform.contains(v._Platform))) &&
             v._IDConexion == IDconexion && v._queryName == queryName)
        {
            qDebug() << IP << "encontrado 1";
            return v._value;
        }
    }

    //por IP especifica e ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )     
        if (v._key == parameter && v._IP == IP && v._Platform.isEmpty() &&
            v._IDConexion == IDconexion  && v._queryName == queryName)
        {
            // qDebug() << IP << "encontrado 2";
            return v._value;
        }

    //plataforma e ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if ( v._key == parameter && v._IP.isEmpty() &&
             ((!platformContains && v._Platform == platform) || (platformContains && platform.contains(v._Platform))) &&
             v._IDConexion == IDconexion  && v._queryName == queryName )
        {
            // qDebug() << IP << "encontrado 3";
            return v._value;
        }

    //no hay para una ip o plataforma, regresamos el general con ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if (v._key == parameter && v._IP.isEmpty() && v._Platform.isEmpty() &&
            v._IDConexion == IDconexion  && v._queryName == queryName )
        {
            // qDebug() << IP << "encontrado 4";
            return v._value;
        }

    //Por IP, Plataforma especifica, Sin ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if ( v._key == parameter && v._IP == IP &&
             ((!platformContains && v._Platform == platform) || (platformContains && platform.contains(v._Platform))) &&
             v._IDConexion.isEmpty() && v._queryName == queryName )
        {
            // qDebug() << IP << "encontrado 5";
            return v._value;
        }

    //por IP especifica sin ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if (v._key == parameter && v._IP == IP && v._Platform.isEmpty() &&
            v._IDConexion.isEmpty() && v._queryName == queryName )
        {
            // qDebug() << IP << "encontrado 6";
            return v._value;
        }

    //plataforma sin ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
    {
        // qDebug() << "comparando" << v._key << v._value << v._IP << v._Platform << v._IDConexion;
        // qDebug() << "comparando" << platformContains << parameter << platform;
        if ( v._key == parameter && v._IP.isEmpty() &&
             ((!platformContains && v._Platform == platform) || (platformContains && platform.contains(v._Platform))) &&
             v._IDConexion.isEmpty() && v._queryName == queryName )
        {
            // qDebug() << IP << "encontrado 7";
            return v._value;
        }
    }

    //no hay para una ip o plataforma, regresamos el general sin ID
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
        if (v._key == parameter && v._IP.isEmpty() && v._Platform.isEmpty() &&
            v._IDConexion.isEmpty() && v._queryName == queryName )
        {
            // qDebug() << IP << "encontrado 8";
            return v._value;
        }

    return QVariant();
}

bool QueriesConfiguration::state(QString parameter, QString IP, QString platform,
                                 QString IDconexion, QString queryName,bool platformContains)
{
    return m_find(parameter,IP,platform,IDconexion,queryName,platformContains).toBool();
}

QString QueriesConfiguration::value(QString parameter, QString IP, QString platform,
                                    QString IDconexion, QString queryName,bool platformContains)
{
    return m_find(parameter,IP,platform,IDconexion,queryName,platformContains).toString();
}

QStringList QueriesConfiguration::values(QString parameter, QString IP, QString platform,
                                         QString IDconexion, QString queryName, bool platformContains)
{
    return value(parameter,IP,platform,IDconexion,queryName,platformContains).split(",",Qt::SkipEmptyParts);
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
    txt = eliminarCaractaresMenorASCII32( txt );
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
    QRegularExpression exp3("\\[.+@(\\S+)\\] \\> *");

    // qDebug() << m_ip << "verificando promt" << data.last();

    if ((!data.last().simplified().contains(exp,&match) &&
         !data.last().simplified().contains(exp2) &&
         !data.last().simplified().contains(exp3)
         )
        || data.contains("Description: ") )
        return false;

    txt.replace("---- More ----","");
    txt.replace("--More--","");
    txt.replace("more..","");

    //Mikrotik
    txt.replace(QRegularExpression("\\[m\\[\\d+m"),"");
    txt.replace("[m","");

    if (txt.contains("Invalid input detected at '^'") ||
        txt.contains("Error: ") ||
        txt.contains(QRegularExpression("Translating .+domain server")) )
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
