#include "funcionbase.h"
#include "queries.h"

#include <QEventLoop>

QueriesConfigurationValue::QueriesConfigurationValue(QString key, QVariant value, QString IP)
{
    _key = key;
    _value = value;
    _IP = IP;
}

QueriesConfigurationValue::QueriesConfigurationValue(const QueriesConfigurationValue &other)
{
    _key = other._key;
    _value = other._value;
    _IP = other._IP;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const QueriesConfigurationValue& qcv)
{
    //guardar
    out << qcv._IP;
    out << qcv._key;
    out << qcv._value;
    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, QueriesConfigurationValue& qcv)
{
    //abrir
    in >> qcv._IP;
    in >> qcv._key;
    in >> qcv._value;
    return in;
}

QNETWORKCLIQUERIES_EXPORT QDebug operator<<(QDebug dbg, const QueriesConfigurationValue &info)
{
     dbg.space() << info._key << info._value << info._IP;
     return dbg;
}

QueriesConfiguration* QueriesConfiguration::m_instance = nullptr;

QueriesConfiguration::QueriesConfiguration()
{}

QueriesConfiguration *QueriesConfiguration::instance()
{
    if ( ! m_instance )
        m_instance = new QueriesConfiguration;

    return m_instance;
}

void QueriesConfiguration::addQueryParameter(const QList<QueriesConfigurationValue>& c)
{
    m_lstQueryParameters.append(c);
}

void QueriesConfiguration::addConfiguration(const QList<QueriesConfigurationValue>& c)
{
    m_lstConfiguration.append(c);
}

QVariant QueriesConfiguration::m_find(QString parameter, QString IP)
{
    //por IP especifica
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
    {
        if ( v._key == parameter && v._IP == IP  )
            return v._value;
    }

    //no hay para un equipo especifico, regresamos el general
    for ( QueriesConfigurationValue v : m_lstQueryParameters )
    {
        if ( v._key == parameter && v._IP == QString("*") )
            return v._value;
    }

    return QVariant();
}

bool QueriesConfiguration::state(QString parameter, QString IP)
{
    return m_find(parameter,IP).toBool();
}

QString QueriesConfiguration::value(QString parameter, QString IP)
{
    return m_find(parameter,IP).toString();
}

QStringList QueriesConfiguration::values(QString parameter, QString IP)
{
    return value(parameter,IP).split(",",QString::SkipEmptyParts);
}

QString QueriesConfiguration::configuration(QString platform, QString os, QString IP)
{
    //por IP especifica
    for ( QueriesConfigurationValue v : m_lstConfiguration )
    {
        if ( (v._key == platform || v._key == os ) && v._IP == IP  )
            return v._value.toString();
    }

    //no hay para un equipo especifico, regresamos el general
    for ( QueriesConfigurationValue v : m_lstConfiguration )
    {
        if ( (v._key == platform || v._key == os ) && v._IP == QString("*") )
            return v._value.toString();
    }

    return "";
}

FuncionBase::FuncionBase()
{
    init();
}

FuncionBase::FuncionBase(QRemoteShell *terminal,QObject *parent)/*:
    QObject(parent) */
{
    init();
    term=terminal;
}

FuncionBase::~FuncionBase()
{}

void FuncionBase::init()
{
    term=nullptr;
    lastCommandFailed=false;
    m_brand="Cisco";
    exp.setMinimal(true);
    exp2.setMinimal(true);
}

void FuncionBase::setPlatform(QString platform)
{
    m_platform=platform;
    m_os=equipmentOSFromPlatform(m_platform);
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
    if ( (!data.last().contains(exp) && !data.last().contains(exp2)) || data.contains("Description: ") )
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
