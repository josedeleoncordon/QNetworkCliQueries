#include "config.h"
#include "queries.h"

Config::Config(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
    m_error=false;
    m_guardado=false;
    m_lstComandosPos=-1;
    m_parentQuery=nullptr;
}

Config::~Config() {}

void Config::configApply()
{
    QString platformcontains = m_queriesConfiguration.value("ConfigPlatformType","","","");
    qDebug() << "Config::configApply() platformcontains" << platformcontains;
    if ( platformcontains == "Contains" )
        m_lstComandos =
                m_queriesConfiguration.value("ConfigTemplate",m_ip,m_platform,m_conexionID,true).split("\n",Qt::SkipEmptyParts);
    else
        m_lstComandos = m_queriesConfiguration.value("ConfigTemplate",m_ip,m_os,m_conexionID).split("\n",Qt::SkipEmptyParts);

    qDebug() << "Config::configApply() lstComandos" << m_lstComandos;

    if ( !m_parentQuery )
    {
        for ( QString comando : m_lstComandos )
            if ( comando.contains("@")  )
            {
                m_lstComandosConErrores.append("Comandos con @ propiedades de padre, padre no fue establecido");
                m_error=true;
                finished();
                return;
            }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_finished()));
    siguienteComando();

//    //entrando al modo de configuracion
//    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_configMode()));
//    if ( m_brand == "Cisco" )
//        termSendText("configure terminal");
//    else if ( m_brand == "Huawei" )
//        termSendText("system-view");
}

void Config::on_term_receiveText_configMode()
{
    txt.append( term->dataReceived() );

    exp.setPattern("^.+\\(config(\\-\\w+)*\\)#\\s*$");
    exp2.setPattern("^\\[.+\\]");
    exp3.setPattern("^Password:$");

    QString line = txt.split("\n").last();
    line = line.simplified();

    if ( line.contains(exp,&match) || line.contains(exp2) || line.contains(exp3) )
    {
        //modo de configuracion
        term->disconnectReceiveTextSignalConnections();
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_finished()));

        siguienteComando();
    }
    else if ( allTextReceived() )
    {
        //no se pudo entrar al modo de configuracion
        m_error=true;
        finished();
    }
}

void Config::siguienteComando()
{
    txt.clear();
    if ( m_lstComandosPos < m_lstComandos.size()-1 )
    {
        m_lstComandosPos++;

        //se envia el comando de configuracion
        //se reemplaza las variables por el valor
//        QStringList comando;
//        foreach (QString word, m_lstComandos.at(m_lstComandosPos).split(" ",Qt::SkipEmptyParts))
//        {
//            if (word.contains("@"))
//            {
//                word.replace("@","");
//                comando.append( m_parentQuery->property(word.toLocal8Bit()).toString() );
//            }
//            else
//                comando.append(word);
//        }
//        QString txt = comando.join(" ");
        QString txt = m_lstComandos.at(m_lstComandosPos);

        qDebug() << Q_FUNC_INFO << "enviando" << txt;
        termSendText( txt );
    }
    else
    {
        //se termino de configurar
//        term->disconnectReceiveTextSignalConnections();
//        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_exited()));
//        if ( m_brand == "Huawei" )
//            termSendText("quit");
//        else
//            termSendText("exit");
        qDebug() << "Config::siguienteComando() se finaliza";
        finished();
    }
}

void Config::on_term_receiveText_finished()
{
    emit working();

    txt.append( term->dataReceived() );    
    QString line = txt.split("\n").last();

    qDebug() << Q_FUNC_INFO << "recibido" << txt;

    line = line.simplified();

    exp.setPattern("^.+\\(config(\\-\\w+)*\\)#\\s*$");
    exp2.setPattern("^\\[.+\\]");
    exp3.setPattern("^\\S+#\\s*$");

    if ( line.contains(exp,&match) || line.contains(exp2) || line.contains(exp3) )
    {
        //si verifica si se salio error al ejecutar el comando
        if ( txt.contains("Unrecognized command") ||
             txt.contains("Invalid input detected") ||
             txt.contains("Error input in the position market by") )
        {
            m_error=true;            
            m_lstComandosConErrores.append( m_lastCommand );
             qDebug() << Q_FUNC_INFO << "m_lstComandosConErrores.size" << m_lstComandosConErrores.size() << m_lastCommand;
        }

        siguienteComando();
    }
}

void Config::on_term_receiveText_exited()
{
    txt.append( term->dataReceived() );
    QString line = txt.split("\n").last();
    line = line.simplified();

    exp.setPattern("^.+\\(config(\\-\\w+)*\\)#\\s*$");
    exp2.setPattern("^\\[.+\\]");

    if ( txt.contains("Uncommitted changes found, commit them befo") )
    {
        //Cisco XR, al salir pregunta si se aplica los cambios
        termSendText("no");   //cambiar a yes
        m_guardado=true;
    }
    else if ( line.contains(exp,&match) || line.contains(exp2) )
    {
        //Se esta aun en el modo de configuracion, nos salimos
        if ( m_brand == "Cisco" )
            termSendText("exit");
        else if ( m_brand == "Huawei" )
            termSendText("quit");
    }
    else if ( line.contains("Are you sure to continue?[Y/N]") )
    {
        //se esta grabando en un equipo Huawei        
        termSendText("N");  //cambiar a Y
        m_guardado=true;
    }
    else if ( line.contains("Destination filename [startup-config]") )
    {
        //se esta grabando en un equipo Cisco
        termSendText("\n");
        m_guardado=true;
    }
    else if ( allTextReceived() )
    {
        finished();

//        //se salio del modo de configuracion
//        if ( !m_guardado )
//        {
//            //se graba la configuración
//            if ( m_brand == "Cisco" )
//                termSendText("copy running-config startup-config");
//            else if ( m_brand == "Huawei" )
//                termSendText("save");
//        }
//        else
//            //se salvo la configuración, se termina
//            finished();
    }
}

QDebug operator<<(QDebug dbg, const Config &info)
{
    dbg.nospace() << "Config:\n";
    dbg.space() << "Errores en la configuración:" << info.m_error;
    foreach (QString command, info.m_lstComandosConErrores)
        dbg.space() << "\t" << command;
    dbg.nospace() << "\n";
    return dbg.maybeSpace();
}
