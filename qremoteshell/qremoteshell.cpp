#include "qremoteshell.h"
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include "terminal/terminal.h"

QRemoteShell::QRemoteShell(QString ip, QString user, QString pwd, QString platform, QObject *parent):
    QObject(parent)
{
    m_ip = ip;
    m_user = user;
    m_pwd = pwd;
    m_platform = platform;
    m_hostConnected = false;       
    m_termle = false;
    m_terminal = nullptr;
    setConnectionProtocol( SSHTelnet );
    m_localprompt.setPattern("\\[.+@.+:.+\\]\\$");
    m_timer = new QTimer;
    m_timer->setInterval(5000);
    m_timer->setSingleShot(true);
    connect(m_timer,SIGNAL(timeout()),SLOT(m_timerout()));
}

QRemoteShell::~QRemoteShell()
{        
    if ( m_terminal )
        delete m_terminal;
    qDebug() << m_ip << "QRemoteShell::~QRemoteShell()";
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
}

void QRemoteShell::host_connect()
{
    qDebug() << m_ip  << "QRemoteShell::host_connect()" << m_ip;
    QTcpSocket *socket = new QTcpSocket; //usamos un socket para ver si el equipo es alcanzable
    socket->connectToHost( m_ip, 1 );
    socket->waitForConnected(5000);    

    if ( socket->state() == QAbstractSocket::ConnectedState ||
         socket->error() == QAbstractSocket::ConnectionRefusedError ) //si se rechaza la conexion es alcanzable
    {
        emit reachable();
        if ( socket->state() == QAbstractSocket::ConnectedState )
            socket->close();

        m_terminal = new Terminal;
        connect(m_terminal,SIGNAL(ready(bool)),SLOT(m_terminal_ready(bool)));
        connect(m_terminal,SIGNAL(receivedData(QString)),SLOT(m_terminal_detaReceived(QString)));
        connect(m_terminal,SIGNAL(finished()),SLOT(m_terminal_finished()));
    }
    else //SocketTimeoutError no alcanzable
    {
        qDebug() << m_ip  << "equipo no alcanzable";
        host_disconnect();
    }
    socket->deleteLater();
}

void QRemoteShell::m_terminal_ready(bool ready)
{
    if ( ready )
        m_nextTry();
    else {
        emit disconnected();
    }
}

void QRemoteShell::m_nextTry()
{    
    if ( !m_lstConnectionProtocol.isEmpty() )
    {
        m_protocol = m_lstConnectionProtocol.takeFirst();

        switch (m_protocol)
        {
        case Telnet: m_terminal->sendCommand( "telnet -l "+m_user+" "+m_ip ); break;
        case SSH: m_terminal->sendCommand( "ssh "+m_user+"@"+m_ip ); break;
        default: Q_UNREACHABLE();
        }
    }
    else
    {
        qDebug() << "QRemoteShell::m_nextTry() falla la conexion ssh o telnet";
        host_disconnect();
    }
}

void QRemoteShell::m_terminal_detaReceived(QString txt)
{    
    m_dataReceived = txt;
    m_timer->stop();

//    qDebug().noquote() << m_ip << txt;

    if ( m_hostConnected )            
        emit readyRead();    
    else
    {
        //no se ha autenticado, se envia usuario y password       

        QStringList data = m_dataReceived.split("\n",QString::SkipEmptyParts);

        if ( data.size() == 0 )
            return;

        QString line = data.last();
        line = line.simplified();

        //saveLog( "+++\n"+ m_dataReceived +"\n---" );
        saveLog( m_dataReceived );

        QRegExp exp;
        exp.setMinimal(true);

        //CISCO ATN

        //enviamos el usuario
        exp.setPattern("(username|login|Username|Login)+:");
        if ( line.contains(exp) )
        {
            sendCommand( m_user );
            return;
        }
        //enviamos el password
        exp.setPattern("(Password|password)+:");
        if ( line.contains(exp) )
        {
            saveLog("\nenviando password\n");
            sendCommand( m_pwd );
            return;
        }

        //si se rechaza la conexion
        //**************************************************************************
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
            m_nextTry();
            return;
        }
        //**************************************************************************

        //verificamos si ya miramos el promt del equipo para saber que ya nos conectamos
        exp.setPattern("^.+#\\s*$");
        if ( line.contains(exp) )
        {
            saveLog( "\nEncontro el prompt\n" );

            if ( !m_termle )
            {
                saveLog( "\nse envia terminal length 0\n" );

                //enviando terminal length 0
                sendCommand( "terminal length 0" );
                m_termle=true;
                return;
            }
            else
            {
                saveLog( "\nconectado .......\n" );
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

                m_hostConnected=true;
                m_vendor="Cisco";

                emit connected();
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
                m_hostConnected=true;

                emit connected();
                return;
            }
        }

        //local prompt
        if ( line.contains(m_localprompt) )
            m_timer->start();
    }
}

void QRemoteShell::host_disconnect()
{            
    qDebug() << m_ip << "QRemoteShell::host_disconnect()" << m_terminal;
    m_hostConnected=false;    

    m_terminal ? m_terminal->close() : emit disconnected();
}

void QRemoteShell::sendData(const QByteArray &data)
{
    if ( m_terminal ) m_terminal->sendData( data );
}

void QRemoteShell::sendCommand(QString cmd)
{    
    qDebug() << m_ip << "QRemoteShell::sendCommand" << cmd;
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
    qDebug() << m_ip << "QRemoteShell::m_terminal_finished()";
    if ( m_terminal )
    {
        delete m_terminal;
        m_terminal = nullptr;
        m_hostConnected=false;
    }
    emit disconnected();
}

void QRemoteShell::m_timerout()
{
    qDebug() << m_ip << "QRemoteShell::m_timerout()";
    host_disconnect();
}
