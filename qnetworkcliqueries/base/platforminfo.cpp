#include "platforminfo.h"
#include "properties.h"

#include "funciones.h"

PlatformInfo::PlatformInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
}

PlatformInfo::PlatformInfo(const PlatformInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
}

void PlatformInfo::getPlatformInfo()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));

    if ( m_brand == "Cisco" )
    {        
        //Como en esta funcion estamos consultando la plataforma no hay manera de saber que version de comando mandar
        //la unica manera de por el nombre, si es un XR tendra la informacion de procesador, etc
        //asi mandamos un show version brief y no tener que perder tiempo con el show version
        //si es un NCS6000 fallara, no podemos diferenciar un CRS a un NCS6000 por el nombre.
        if (m_name.contains(QRegularExpression("RP(_|/)0.+(_|:)")))
            termSendText( "show version brief" );
        else
            termSendText( "show version" );
    }
    else if ( m_brand == "Huawei" )
        termSendText("display version");
    else if ( m_brand == "MikroTik" )
    {
        m_platform="MikroTik";
        term->disconnectReceiveTextSignalConnections();
        finished();
    }
    else
    {
        qDebug() << "PlatformInfo::getPlatformInfo()" << m_brand << "no soportado";
        finished();
    }
}

void PlatformInfo::on_term_receiveText()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    if ( m_brand == "Cisco" )
    {
        //si el equipo es un NCS6000 fallara el comando, le mandamos un show version
        if (m_name.contains(QRegularExpression("RP(_|/)0.+(_|:)")) &&
                lastCommandFailed)
        {
            m_xr64 = true;
            termSendText("show version");
            return;
        }

        exp.setPattern("^(c|C)isco .+ processor .+ memory");
        exp2.setPattern(" *(c|C)isco .+ (c|C)hassis");
        exp3.setPattern("(c|C)isco .+ processor$");
        exp4.setPattern("(c|C)isco \\S+ \\(.+");
        foreach (QString line, lines)
        {
            line = line.simplified();

            if ( ! line.contains(exp,&match) && !line.contains(exp2) && !line.contains(exp3) && !line.contains(exp4) )
                continue;                        

            QStringList data = line.split(" ",Qt::SkipEmptyParts);
            m_platform = data.at(1).simplified();

            qDebug() << m_ip << Q_FUNC_INFO << m_platform;

            break;
        }

        exp.setPattern("^(.+) Chassis");
        foreach (QString line, lines)
        {
            line = line.simplified();
            if ( line.contains(exp,&match) )
            {
                m_model = match.captured(1);
                break;
            }
        }

        //Raisecom. Tiene el mismo prompt de Cisco
        exp.setPattern("Product (Version|Name|name): (.+)$");
        foreach (QString line, lines)
        {
            line = line.simplified();

            if ( line.contains(exp,&match) )
            {
                m_platform = match.captured(2);
                if ( m_platform.contains("RAX") || m_platform.contains("ISCOM") )
                    m_brand="RAISECOM";
                break;
            }
        }

    }
    else if ( m_brand == "Huawei" )
    {
        exp.setPattern("software, Version \\d+(\\.\\d+)* \\((.+)\\)");
        foreach (QString line, lines)
        {
            line = line.simplified();
            if ( line.contains(exp,&match) )
            {
                m_platform = match.captured( 2 );
                break;
            }
        }
    }    

    //si es xr verificamos el location a utilizar para comandos como la consulta de tablas de mac
    if ( equipmentOSFromPlatform(m_platform) == "IOS XR" )
    {
        term->disconnectReceiveTextSignalConnections();
        connect(term,SIGNAL(readyRead()),SLOT(on_term_on_term_receiveText_xr_location()));
        termSendText("show platform");
    }
    else
    {
        qDebug() << m_ip << Q_FUNC_INFO << m_platform << equipmentOSFromPlatform( m_platform ) << m_os;
        if ( m_brand == "Cisco" )
        {
            term->disconnectReceiveTextSignalConnections();
            connect(term,SIGNAL(readyRead()),SLOT(on_term_on_term_receiveTextSnmpLocation()));
            termSendText("show snmp location");
        }
        else
        {
            qDebug() << m_ip << "CLI SNMP Location no soportado" << m_brand;
            finished();
        }
    }
}

void PlatformInfo::on_term_on_term_receiveText_xr_location()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    QRegularExpression exp("^\\d/\\d/CPU\\d ");
    foreach (QString line, lines)
    {        
        if ( line.contains( exp,&match ) )
        {            
            m_xr_location = line.split(" ").at(0);
            break;
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_on_term_receiveTextSnmpLocation()));
    termSendText("sh running-config snmp-server location location");
}

void PlatformInfo::on_term_on_term_receiveTextSnmpLocation()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n",Qt::SkipEmptyParts);

    if ( equipmentOSFromPlatform(m_platform) == "IOS XR" )
    {
        for ( QString line : lines )
        {
            if ( line.contains("sh run") )
                continue;

            if ( line.contains("snmp-server location") )
            {
                m_location = line.replace("snmp-server location","").simplified();
                break;
            }
        }
    }
    else
    {
        lines.removeLast();
        if ( !lines.last().contains("sh snmp") )
            m_location = lines.last().simplified();
    }

    finished();
}
