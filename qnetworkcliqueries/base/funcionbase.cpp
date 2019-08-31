#include "funcionbase.h"
#include "queries.h"

InfoBase::InfoBase(const InfoBase &other)
{
    datetime = other.datetime;
    operativo = other.operativo;
    queryParent = other.queryParent;
}

QueriesConfiguration* QueriesConfiguration::m_instance = nullptr;

QueriesConfiguration::QueriesConfiguration()
{
    opciones=0;
}

QueriesConfiguration *QueriesConfiguration::instance()
{
    if ( ! m_instance )
        m_instance = new QueriesConfiguration;

    return m_instance;
}

QStringList QueriesConfiguration::mapQueriesToList(QString value)
{
    return m_instance->mapQueries.value(value).split(",",QString::SkipEmptyParts);
}

FuncionBase::FuncionBase(QRemoteShell *terminal,QObject *parent):
    QObject(parent)
{
    term=terminal;
    lastCommandFailed=false;
    m_brand="Cisco";
    exp.setMinimal(true);
    exp2.setMinimal(true);

    m_queriesParent=dynamic_cast<Queries*>( parent );
}

FuncionBase::~FuncionBase()
{}

void FuncionBase::setPlatform(QString platform)
{
    m_platform=platform;
    m_os=equipmentOSFromPlatform(m_platform);
}

void FuncionBase::termSendText(QString str)
{
    txt.clear();
    lastCommandFailed=false;
    m_lastCommand = str;
    term->sendCommand( str );
    emit lastCommand(m_lastCommand);
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
        qDebug() << "Enviando espacio para continuar";
        QByteArray a(1,32);
        term->sendData( a );
        return false;
    } 

    //hasta que miremos el prompt del equipo sabremos que se finalizo de recibir texto

    QStringList data = txt.split("\n");
    exp.setPattern("^.+#\\s*$");
    exp2.setPattern("^<.+>");
    if ( !data.last().contains(exp) && !data.last().contains(exp2) )
        return false;

    txt.replace("---- More ----","");
    txt.replace("--More--","");
    txt.replace("more..","");

    if ( txt.contains("Invalid input detected at '^'") ||
         txt.contains(QRegExp("Translating .+domain server")) )
    {
        lastCommandFailed=true;
        qDebug() << "**ErrorMenor**" << m_ip << m_name << m_platform << "No se pudo ejecutar comando:" << m_lastCommand;
    }
    else
        lastCommandFailed=false;

    qDebug() << "true" << m_lastCommand << lastCommandFailed;

    return true;
}

void FuncionBase::finished()
{
    qDebug() << "FuncionBase::finished()";

    txt.clear();
    term->disconnectReceiveTextSignalConnections();
    emit processFinished();
}
