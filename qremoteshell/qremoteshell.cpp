#include "qremoteshell.h"
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include "terminal/terminal.h"
#include "qremoteshelllogging.h"

QRemoteShell::QRemoteShell(QString ip, QString user, QString pwd, QString platform, QString linuxprompt, QObject *parent):
    QObject(parent)
{
    m_ip = ip;
    m_user = user;
    m_pwd = pwd;
    m_platform = platform;
    m_linuxprompt = linuxprompt;
    m_hostConnected = false;       
    m_termle = false;
    m_terminal = nullptr;
    m_pwdsent = false;
    m_gwConnected = false;
    m_socket = nullptr;
    m_user1sent = false;
    m_pwd1sent = false;
    setConnectionProtocol( SSHTelnet );
    m_localprompt.setPattern(linuxprompt);

    m_timerNoResponse = new QTimer;
    m_timerNoResponse->setInterval(10000);
    m_timerNoResponse->setSingleShot(true);
    connect(m_timerNoResponse,SIGNAL(timeout()),SLOT(m_timeroutNoResponse()));
}

QRemoteShell::~QRemoteShell()
{        
    if ( m_terminal )
        delete m_terminal;
    if ( m_socket )
        delete m_socket;
    delete m_timerNoResponse;
    qDebug() << m_ip << "QRemoteShell::~QRemoteShell()"; //no con qCDebug ya que para cerrar el archivo debug del equipo es necesario est en QueriesMessageHandler. No Borrar. No usar con qCDebug ya que podria ser deshabilitado.
}

void QRemoteShell::setConnectionProtocol( ConnectionProtocol cp )
{
    m_lstConnectionProtocol.clear();
    switch (cp)
    {
    case Telnet: m_lstConnectionProtocol << Telnet; break;
    case TelnetSSH: m_lstConnectionProtocol << Telnet << SSH; break;
    case SSH: m_lstConnectionProtocol << SSH; break;
    case SSHTelnet: m_lstConnectionProtocol << SSH << Telnet; break;
    }
    m_lstConnectionProtocolgw = m_lstConnectionProtocol;
}

void QRemoteShell::host_connect()
{
    if ( m_gw.isEmpty() )
    {
        //conexion directa al equipo
        if ( m_socket )
            delete m_socket;

        qCDebug(qremoteshell) << m_ip  << "QRemoteShell::host_connect()" << m_ip;
        m_socket = new QTcpSocket(this); //usamos un socket para ver si el equipo es alcanzable
        m_socket->connectToHost( m_ip, 1 );
        m_socket->waitForConnected(10000);

        if ( m_socket->state() == QAbstractSocket::ConnectedState ||
             m_socket->error() == QAbstractSocket::ConnectionRefusedError ) //si se rechaza la conexion es alcanzable
        {
            qCDebug(qremoteshell) << m_ip  << "QRemoteShell::host_connect() reachable" << m_ip;

            emit reachable();

            m_terminal = new Terminal( m_ip,m_linuxprompt );
            connect(m_terminal,SIGNAL(ready(bool)),SLOT(m_terminal_ready(bool)));
            connect(m_terminal,SIGNAL(receivedData(QString)),SLOT(m_terminal_detaReceived(QString)));
            connect(m_terminal,SIGNAL(finished()),SLOT(m_terminal_finished()));
        }
        else //SocketTimeoutError no alcanzable
        {
            qCDebug(qremoteshell) << m_ip  << "equipo no alcanzable";
            host_disconnect();
        }
        m_socket->close();
    }
    else
    {
        //conexion a traves de un gw
        if ( !m_gwConnected )
        {
            //conexion al gw
            //no se necesita probar con qtcpsocket si es alcanzable ya que no es el equipo objetivo

            qCDebug(qremoteshell) << m_ip << "QRemoteShell::host_connect() conectando al GW" << m_gw;
            m_terminal = new Terminal( m_ip,m_linuxprompt ); //IP es solo para separar el debug
            connect(m_terminal,SIGNAL(ready(bool)),SLOT(m_terminal_ready(bool)));
            connect(m_terminal,SIGNAL(receivedData(QString)),SLOT(m_terminal_detaReceived(QString)));
            connect(m_terminal,SIGNAL(finished()),SLOT(m_terminal_finished()));
        }
        else
            //tratar conexion del gw al equipo
            m_nextTry();
    }
}

void QRemoteShell::m_terminal_ready(bool ready)
{
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_terminal_ready" << ready;
    if ( ready )
        m_nextTry();
    else
        emit disconnected();
}

void QRemoteShell::m_nextTry()
{    
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry() size" << m_lstConnectionProtocol.size();

    emit working();

    if ( !m_lstConnectionProtocol.isEmpty() )
    {
        m_protocol = m_lstConnectionProtocol.takeFirst();

        qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry()" << m_protocol;

        if ( !m_gwConnected ) //m_gwConnected va a ser falso la primera conexion, o no si no se usa un gw
        {
            //conexion al equipo o gw

            QString ip = m_gw.isEmpty()?m_ip:m_gw;

            switch (m_protocol)
            {
            case Telnet: m_terminal->sendCommand( "telnet -l "+m_user+" "+ip ); break;
            case SSH: m_terminal->sendCommand( "ssh "+m_user+"@"+ip ); break;
            default: Q_UNREACHABLE();
            }
        }
        else
        {
            //conexion del gw al equipo
            switch (m_protocol)
            {
            case Telnet: m_terminal->sendCommand( "telnet "+m_ip ); break;
            case SSH: m_terminal->sendCommand( "ssh "+m_ip ); break;
            default: Q_UNREACHABLE();
            }
        }
    }
    else
    {
        qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry() falla la conexion ssh o telnet";
        host_disconnect();
    }
}

void QRemoteShell::m_terminal_detaReceived(QString txt)
{    
    m_dataReceived = txt;

    qCDebug(terminalReceived).noquote() << m_ip << txt; //debug de todo el texto recibido en la terminal

    if ( m_hostConnected )            
        emit readyRead();    
    else
    {
        //no se ha autenticado, se envia usuario y password       

        m_timerNoResponse->stop();        

        QStringList data = m_dataReceived.split("\n",QString::SkipEmptyParts);

        if ( data.size() == 0 )
            return;

        QString line = data.last();
        line = line.simplified();

        QRegExp exp;
        exp.setMinimal(true);

        //CISCO ATN

        //enviamos el usuario
        exp.setPattern("(username|login|Username|Login)+:");
        if ( line.contains(exp) )
        {
            QString user;
            if ( m_user2.isEmpty() )
                user = m_user;
            else
            {
                if ( !m_user1sent )
                {
                    user = m_user;
                    m_user1sent=true;
                }
                else
                    user = m_user2;
            }

            qCDebug(qremoteshell) << m_ip << "enviando usuario" << user;
            sendCommand( user );
            return;
        }
        //enviamos el password
        exp.setPattern("(Password|password)+:");
        if ( line.contains(exp) )
        {
            if ( !m_pwdsent )
            {
                QString pwd;
                if ( m_pwd2.isEmpty() )
                {
                    pwd = m_pwd;
                    m_pwdsent=true;
                }
                else
                {
                    if ( !m_pwd1sent )
                    {
                        pwd = m_pwd;
                        m_pwd1sent=true;
                    }
                    else
                    {
                        pwd = m_pwd2;
                        m_pwdsent=true;
                    }
                }

                qCDebug(qremoteshell) << m_ip << "enviando password" << pwd;
                sendCommand( pwd );
            }
            else
            {
                qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_terminal_detaReceived: se pide nuevamente el pwd";
                host_disconnect();
            }
            return;
        }

        //si se rechaza la conexion
        if ( !m_gw.isEmpty() && !m_user2.isEmpty() )
        {
            //se esta conectado al GW y se rechazo el intento de conexion por el protocolo (Telnet SSH). Se intenta con el siguiente
            if ( m_dataReceived.contains("Authorization failed") ||
                 m_dataReceived.contains("Connection closed by") ||
                 m_dataReceived.contains("Connection timed out") ||
                 m_dataReceived.contains("remote host not responding") ||
                 m_dataReceived.contains("Connection refused") ||
                 m_dataReceived.contains("Invalid key length") ||
                 m_dataReceived.contains("Connection reset by peer") ||
                 m_dataReceived.contains("Disconnected from") ||
                 m_dataReceived.contains("no matching host key type found") )
            {
                qCDebug(qremoteshell) << m_ip << "Connection refused, a probar con el siguiente protocolo";
                m_nextTry();
                return;
            }
        }

        //verificamos si ya miramos el promt del equipo para saber que ya nos conectamos
        exp.setPattern("^.+#\\s*$");
        if ( line.contains(exp) )
        {
            qCDebug(qremoteshell) << m_ip << "Encontro el prompt";

            if ( !m_termle )
            {
                qCDebug(qremoteshell) << m_ip << "se envia terminal length 0";

                //enviando terminal length 0
                sendCommand( "terminal length 0" );
                m_termle=true;
                return;
            }
            else
            {
                qCDebug(qremoteshell) << m_ip << "conectado .......";
                m_host = exp.cap(0).replace("#","").simplified();

                //se han encontrado casos de caracteres no imprimibles al principio del nombre del equipo
                //se eliminan
                int c=0;
                forever
                {
                    char caracter = m_host.at( c ).toLatin1();
                    if ( caracter < 32 && caracter != 13 && caracter != 10 )
                        m_host.remove(c,1);
                    else
                        c++;

                    if ( c == m_host.size() )
                        break;
                }

                m_vendor="Cisco";

                finalizado();
                return;
            }
        }

        //HUAWEI ATN
        exp.setPattern("^<.+> *$");
        if ( line.contains(exp) )
        {
            if ( !m_termle )
            {
                //enviando terminal length 0
                sendCommand( "screen-length 0 temporary" );
                m_termle=true;
                return;
            }
            else
            {
                m_host = exp.cap(0);
                m_host.replace("<","");
                m_host.replace(">","");
                m_host = m_host.simplified();
                m_vendor = "Huawei";

                finalizado();
                return;
            }
        }

        //local prompt
        if ( line.contains(m_localprompt) ||
             line.contains(QRegExp("^.+#\\s*$")) ) //si es gw
        {
            qCDebug(qremoteshell) << "---------- localprompt ----------------";
            m_nextTry();
        }

//        qCDebug(qremoteshell) << m_ip << "m_timerNoResponse start";
        m_timerNoResponse->start();
    }
}

void QRemoteShell::finalizado()
{
    if ( m_gw.isEmpty() )
    {
        qCDebug(qremoteshell) << m_ip << "QRemoteShell::finalizado() conectado";
        m_hostConnected=true;
        emit connected();
    }
    else
    {
        if ( !m_gwConnected )
        {
            //conectado al GW, a conectarse al equipo final

            qCDebug(qremoteshell) << m_ip << "QRemoteShell::finalizado() conectado al gw, a empezar conexion con el equipo";

            m_gwConnected=true;
            m_pwdsent=false;
            m_termle=false;
            m_user1sent=false;
            m_pwd1sent=false;
            m_lstConnectionProtocol = m_lstConnectionProtocolgw;
            host_connect();
        }
        else
        {
            qCDebug(qremoteshell) << m_ip << "QRemoteShell::finalizado() conectado al equipo a traves del gw";

            m_hostConnected=true;
            emit connected();
        }
    }
}

void QRemoteShell::host_disconnect()
{            
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::host_disconnect()" << m_terminal;
    m_hostConnected=false;    
    disconnect(m_terminal,SIGNAL(receivedData(QString)),this,SLOT(m_terminal_detaReceived(QString)));

    m_terminal ? m_terminal->close() : emit disconnected();
}

void QRemoteShell::sendData(const QByteArray &data)
{
    if ( m_terminal ) m_terminal->sendData( data );
}

void QRemoteShell::sendCommand(QString cmd)
{    
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::sendCommand" << cmd;
    if ( m_terminal ) m_terminal->sendCommand(cmd);
}

void QRemoteShell::disconnectReceiveTextSignalConnections()
{
    disconnect(this,SIGNAL(readyRead()),0,0);
}

QString QRemoteShell::hostName()
{
    return m_host;
}

QString QRemoteShell::brandName()
{
    return m_vendor;
}

QString QRemoteShell::platform()
{
    return m_platform;
}

void QRemoteShell::m_terminal_finished()
{
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_terminal_finished()";
    if ( m_terminal )
    {
        delete m_terminal;
        m_terminal = nullptr;
        m_hostConnected=false;
    }
    emit disconnected();
}

void QRemoteShell::m_timeroutNoResponse()
{
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_timeroutNoResponse()";
    QByteArray a(1,3);
    sendData( a );
}
