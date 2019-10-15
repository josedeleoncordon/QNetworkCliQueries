#include "qremoteshell.h"
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include "terminal/terminal.h"

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
    m_lstConnectionProtocolgw = m_lstConnectionProtocol;
}

void QRemoteShell::host_connect()
{
    if ( m_gw.isEmpty() )
    {
        //conexion directa al equipo
        if ( m_socket )
            delete m_socket;

        qDebug() << m_ip  << "QRemoteShell::host_connect()" << m_ip;
        m_socket = new QTcpSocket(this); //usamos un socket para ver si el equipo es alcanzable
        m_socket->connectToHost( m_ip, 1 );
        m_socket->waitForConnected(5000);

        if ( m_socket->state() == QAbstractSocket::ConnectedState ||
             m_socket->error() == QAbstractSocket::ConnectionRefusedError ) //si se rechaza la conexion es alcanzable
        {
            emit reachable();

            m_terminal = new Terminal( m_ip,m_linuxprompt );
            connect(m_terminal,SIGNAL(ready(bool)),SLOT(m_terminal_ready(bool)));
            connect(m_terminal,SIGNAL(receivedData(QString)),SLOT(m_terminal_detaReceived(QString)));
            connect(m_terminal,SIGNAL(finished()),SLOT(m_terminal_finished()));
        }
        else //SocketTimeoutError no alcanzable
        {
            qDebug() << m_ip  << "equipo no alcanzable";
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

            QString mie = m_gw;
            m_gw = m_ip;
            m_ip = mie;

            qDebug() << "QRemoteShell::host_connect() conectando al GW" << m_gw;
            m_terminal = new Terminal( m_ip,m_linuxprompt );
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
    if ( ready )
        m_nextTry();
    else {
        emit disconnected();
    }
}

void QRemoteShell::m_nextTry()
{    
    qDebug() << "QRemoteShell::m_nextTry()" << m_lstConnectionProtocol.size();

    if ( !m_lstConnectionProtocol.isEmpty() )
    {
        m_protocol = m_lstConnectionProtocol.takeFirst();

        qDebug() << "QRemoteShell::m_nextTry()" << m_protocol;

        if ( !m_gwConnected ) //m_gwConnected va a ser falso la primera conexion, o no si no se usa un gw
        {
            //conexion al equipo o gw
            qDebug() << "aaaaa";
            switch (m_protocol)
            {
            case Telnet: m_terminal->sendCommand( "telnet -l "+m_user+" "+m_ip ); break;
            case SSH: m_terminal->sendCommand( "ssh "+m_user+"@"+m_ip ); break;
            default: Q_UNREACHABLE();
            }
        }
        else
        {
            //conexion del gw al equipo
            qDebug() << "bbbbb";
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
        qDebug() << "QRemoteShell::m_nextTry() falla la conexion ssh o telnet";
        host_disconnect();
    }
}

void QRemoteShell::m_terminal_detaReceived(QString txt)
{    
    m_dataReceived = txt;   

//    qDebug().noquote() << m_ip << txt;

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
            if ( !m_pwdsent )
            {
                m_pwdsent=true;
                saveLog("\nenviando password\n");
                sendCommand( m_pwd );
            }
            else
            {
                qDebug() << m_ip << "QRemoteShell::m_terminal_detaReceived: se pide nuevamente el pwd";
                host_disconnect();
            }
            return;
        }

        //si se rechaza la conexion
        //**************************************************************************
//        if ( m_dataReceived.contains("Authorization failed") ||
//             m_dataReceived.contains("Connection closed by") ||
//             m_dataReceived.contains("Connection timed out") ||
//             m_dataReceived.contains("remote host not responding") ||
//             m_dataReceived.contains("Connection refused") ||
//             m_dataReceived.contains("Invalid key length") ||
//             m_dataReceived.contains("Connection reset by peer") ||
//             m_dataReceived.contains("Disconnected from") ||
//             m_dataReceived.contains("no matching host key type found") )
//        {
//            m_nextTry();
//            return;
//        }
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
            qDebug() << "---------- localprompt ----------------";
            m_nextTry();
        }

        qDebug() << "m_timerNoResponse start";
        m_timerNoResponse->start();
    }
}

void QRemoteShell::finalizado()
{
    if ( m_gw.isEmpty() )
    {
        qDebug() << "QRemoteShell::finalizado() conectado";
        m_hostConnected=true;
        emit connected();
    }
    else
    {
        if ( !m_gwConnected )
        {
            //conectado al GW, a conectarse al equipo fin

            qDebug() << "QRemoteShell::finalizado() conectado al gw, a empezar conexion con el equipo";

            QString mie = m_gw;
            m_gw = m_ip;
            m_ip = mie;

            m_gwConnected=true;
            m_pwdsent=false;
            m_lstConnectionProtocol = m_lstConnectionProtocolgw;
            host_connect();
        }
        else
        {
            qDebug() << "QRemoteShell::finalizado() conectado al equipo a traves del gw";

            m_hostConnected=true;
            emit connected();
        }
    }
}

void QRemoteShell::host_disconnect()
{            
    qDebug() << m_ip << "QRemoteShell::host_disconnect()" << m_terminal;
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

void QRemoteShell::m_timeroutNoResponse()
{
    qDebug() << m_ip << "QRemoteShell::m_timeroutNoResponse()";
    QByteArray a(1,3);
    sendData( a );
}
