#include "qremoteshell.h"
#include "qtelnetconnection.h"
#include "qsshconnection.h"
#include "cliconnection.h"

QRemoteShell::QRemoteShell(QString ip, QString user, QString pwd, QString platform, QObject *parent):
    QObject(parent)
{
    m_ip = ip;
    m_user = user;
    m_pwd = pwd;
    m_platform = platform;
    m_Connection = nullptr;
    m_hostConnected = false;   
    m_protocol = TelnetSSH;
    m_connectiontype = Lib;
}

QRemoteShell::~QRemoteShell()
{
}

void QRemoteShell::setGW(QString GW)
{
    m_gw = GW;
    if ( !m_gw.isEmpty() )
        m_protocol = Telnet;
}

void QRemoteShell::setLogPath(QString path)
{
    if ( !path.isEmpty() )
    {
        m_logPath = path;
        m_logFile.setFileName( m_logPath );
        m_logFile.open(QIODevice::Append | QIODevice::Text);
        m_out.setDevice(&m_logFile);
    }
}

void QRemoteShell::saveLog(QString txt)
{
    if ( m_logFile.isOpen() )
    {
        m_out << txt;
        m_out.flush();
    }
}

void QRemoteShell::host_connect()
{
    if ( m_connectiontype == Lib )
    {
        //Se usan las librerias de Telnet y Ssh
        switch (m_protocol)
        {
        case Telnet:
        case TelnetSSH:
        {
            qDebug() << "probando con telnet";
            m_Connection = new QTelnetConnection( m_ip,"23",m_user,m_pwd,m_platform,this );
            if ( !m_country.isEmpty() || !m_gw.isEmpty() )
            {
                QTelnetConnection *tc = dynamic_cast<QTelnetConnection*>( m_Connection );

                if ( tc )
                {
                    if ( !m_gw.isEmpty() )
                        tc->setGTW( m_gw );
                    if ( !m_country.isEmpty() )
                        tc->setCountry( m_country );
                }
            }
            break;
        }
        case SSH:
        case SSHTelnet:
        {
            qDebug() << "probando con ssh";
            m_Connection = new QSshConnection( m_ip,"22",m_user,m_pwd,m_platform,this );
            break;
        }
        }
    }
    else
    {
        //Se usa comandos cli con QProcess
        switch (m_protocol)
        {
        case SSH:
        {
            m_Connection = new CliConnection(m_ip,"",m_user,m_pwd,m_platform,this);
            break;
        }
        default:
            break;
        }
    }

    m_Connection->setLogPath( m_logPath );
    connect(m_Connection,SIGNAL(reachable()),SIGNAL(reachable()));
    connect(m_Connection,SIGNAL(connected()),SLOT(m_connected()));
    connect(m_Connection,SIGNAL(disconnected()),SLOT(m_disconnected()));
    connect(m_Connection,SIGNAL(readyRead()),SLOT(m_receiveText()));
    m_Connection->host_connect();
}

void QRemoteShell::host_disconnect()
{    
    if ( m_Connection )
    {
        qDebug() << "QRemoteShell::host_disconnect()";
        m_Connection->disconnect();
        m_Connection->host_disconnect();
        m_Connection->deleteLater();
        m_Connection=nullptr;
    }
}

void QRemoteShell::m_connected()
{
    m_hostConnected=true;
    emit connected();
}

void QRemoteShell::m_disconnected()
{    
    if ( !m_hostConnected )
    {
        //no se ha logrado conexion con el equipo, se prueba con otro tipo de protocolo
        qDebug() << "QRemoteShell::m_disconnected() 1";
        if ( m_protocol == TelnetSSH && dynamic_cast<QTelnetConnection*>(m_Connection) )
        {
            host_disconnect();

            //Conexion de telnet fallo, probamos SSH

            qDebug() << "probando con SSH";

            m_Connection = new QSshConnection( m_ip,"22",m_user,m_pwd,m_platform,this );
            connect(m_Connection,SIGNAL(reachable()),SIGNAL(reachable()));
            connect(m_Connection,SIGNAL(connected()),SLOT(m_connected()));
            connect(m_Connection,SIGNAL(disconnected()),SLOT(m_disconnected()));
            connect(m_Connection,SIGNAL(readyRead()),SLOT(m_receiveText()));
            m_Connection->host_connect();
        }
        else if ( m_protocol == SSHTelnet && dynamic_cast<QSshConnection*>(m_Connection) )
        {
            host_disconnect();

            //Conexion de ssh fallo, probamos telnet

            qDebug() << "Probando con Telnet";

            m_Connection = new QTelnetConnection( m_ip,"23",m_user,m_pwd,m_platform,this );
            connect(m_Connection,SIGNAL(reachable()),SIGNAL(reachable()));
            connect(m_Connection,SIGNAL(connected()),SLOT(m_connected()));
            connect(m_Connection,SIGNAL(disconnected()),SLOT(m_disconnected()));
            connect(m_Connection,SIGNAL(readyRead()),SLOT(m_receiveText()));
            m_Connection->host_connect();
        }
        else
        {
            qDebug() << "QRemoteShell::m_disconnected() 4";
            host_disconnect();
            emit disconnected();
        }
    }
    else
    {
        //host cierra la conexion, ya sea porque se lo solicitamos o el lo realiza
        //no se trata con otro protocolo ya que la primera fue exitosa
        host_disconnect();
        emit disconnected();
    }
}

void QRemoteShell::m_receiveText()
{
    m_dataReceived = m_Connection->dataReceived();

    if ( !m_dataReceived.isEmpty() )
        emit readyRead();
}

void QRemoteShell::sendData(const QByteArray &data)
{
    if ( m_hostConnected )
        m_Connection->sendData( data );
}

void QRemoteShell::sendCommand(QString cmd)
{
    if ( m_hostConnected )
    {
//        qDebug() << "sendCommand" << cmd;
        m_Connection->sendCommand( cmd );
    }
}

void QRemoteShell::disconnectReceiveTextSignalConnections()
{
	disconnect(this,SIGNAL(readyRead()),0,0);
}

QString QRemoteShell::hostName()
{
    return m_Connection->hostName();
}

QString QRemoteShell::brandName()
{
    return m_Connection->brandName();
}

QString QRemoteShell::platform()
{
    return m_Connection->platform();
}
