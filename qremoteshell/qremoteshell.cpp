#include "qremoteshell.h"
#include <QDebug>
#include <QTimer>
#include <QThread>
#include "qremoteshelllogging.h"
#include "qtelnet.h"
#include "qsshsession.h"

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
    m_enablesent = false;
    m_pwdsent = false;
    m_telnet = nullptr;
    m_ssh = nullptr;
    m_sshok=false;
    m_sshmodelibssh=true;
    m_lastlibsshfailed=false;
    m_connectionRefused=false;
    m_consultaIntentos=0;
    setConnectionProtocol( SSHTelnet );
    m_localprompt.setPattern(linuxprompt);
}

QRemoteShell::~QRemoteShell()
{        
    if ( m_telnet )
        delete m_telnet;
    if ( m_ssh )
        delete m_ssh;
    qDebug() << m_ip << "QRemoteShell::~QRemoteShell()";
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
    m_nextTry();
}

void QRemoteShell::m_nextTry()
{
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry() size" <<
        m_lstConnectionProtocol.size();

    emit working();

    m_pwdsent=false;
    m_connectionRefused=false;
    _errortxt.clear();

    bool probar=false;
    if ( !m_lstConnectionProtocol.isEmpty() )
    {
        m_protocol = m_lstConnectionProtocol.takeFirst();
        probar=true;
    }

    if ( probar )
        m_protocolTry();
    else
    {
        qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry() falla la conexion ssh o telnet";
        host_disconnect();
    }
}

void QRemoteShell::m_protocolTry()
{
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_protocolTry()" << m_protocol;
    //alcanzable continamos
    m_consultaIntentos++;
    //conexion al equipo
    switch (m_protocol)
    {
    case Telnet:
    {
        if ( m_telnet  )
        {
            m_telnet->disconnect();
            m_telnet->host_disconnect();
            m_telnet->deleteLater();
        }

        m_telnet = new QTelnet(m_ip,"23",this);
        connect(m_telnet,SIGNAL(connected()),SLOT(telnet_host_connected()));
        connect(m_telnet,SIGNAL(disconnected()),SLOT(telnet_host_disconnected()));
        connect(m_telnet,SIGNAL(readyRead()),SLOT(on_telnet_dataReceived()));
        m_telnet->host_connect();

        break;
    }
    case SSH:
    {
        qCDebug(qremoteshell) << m_ip << "try libssh";
        if ( m_ssh  )
        {
            m_ssh->disconnect();
            m_ssh->host_disconnect();
            m_ssh->deleteLater();
        }
        m_ssh = new QSSHSession(m_ip,"22",m_user,m_pwd);
        connect(m_ssh,SIGNAL(connected()),SLOT(ssh_host_connected()));
        connect(m_ssh,SIGNAL(disconnected()),SLOT(ssh_host_disconnected()));
        connect(m_ssh,SIGNAL(readyRead()),SLOT(on_ssh_dataReceived()));
        m_ssh->connect_to();

        break;
    }
    default: Q_UNREACHABLE();
    }
}

void QRemoteShell::telnet_host_connected()
{
    qCDebug(qremoteshell) << m_ip << Q_FUNC_INFO;
    emit reachable();
}

void QRemoteShell::telnet_host_disconnected()
{
    qCDebug(qremoteshell) << m_ip << Q_FUNC_INFO;
    emit disconnected();
}

void QRemoteShell::on_telnet_dataReceived()
{
    // qCDebug(qremoteshell) << m_ip << Q_FUNC_INFO;
    m_terminal_detaReceived( m_telnet->dataReceived() );
}

void QRemoteShell::ssh_host_connected()
{
    qCDebug(qremoteshell) << m_ip << Q_FUNC_INFO;
    m_sshok=true;
    emit reachable();
    //libssh de una vez autentica. En m_terminal_detaReceived se recibira datos y se podra identificar el hostname
}

void QRemoteShell::ssh_host_disconnected()
{
    qCDebug(qremoteshell) << m_ip << Q_FUNC_INFO;
    emit working();
    if ( m_sshok )
        emit disconnected();
    else
    {
        qCDebug(qremoteshell) << m_ip << "SSH Error" << m_ssh->error() << m_ssh->errortxt();
        _errortxt = m_ssh->errortxt();

        if (!_errortxt.contains("Timeout") &&
            !_errortxt.contains("La red es inaccesible") &&
            !_errortxt.contains("Network is unreachable"))
            emit reachable();

        switch ( m_ssh->error() )
        {
        case QSSHSession::OPTIONS_ERROR:
        {
            if ( _errortxt.contains("kex error") )
            {
                emit disconnected();
                return;
            }

            if ( m_consultaIntentos < 2 )
            {
                qCDebug(qremoteshell) << m_ip << "LibSSH Disconnected. Se intenta nuevamente";
                QTimer::singleShot( 2000,[this]() { m_protocolTry(); } );
            }
            else
                emit disconnected();

            break;
        }
        case QSSHSession::AUTH_DENIED:
        case QSSHSession::CHANNEL_ERROR:
        case QSSHSession::SHELL_ERROR:
        {
            m_nextTry();
            break;
        }
        default: break;
        };
    }
}

void QRemoteShell::on_ssh_dataReceived()
{
    // qCDebug(qremoteshell) << m_ip << Q_FUNC_INFO;
    m_terminal_detaReceived( m_ssh->dataReceived() );
}

void QRemoteShell:: m_terminal_detaReceived(QString txt)
{
    m_dataReceived = eliminarCaractaresMenorASCII32(txt);

    qCDebug(terminalReceived).noquote() << m_ip << m_dataReceived; //debug de todo el texto recibido en la terminal

    if ( m_hostConnected )
    {
        // qDebug() << Q_FUNC_INFO << "emit readyRead()";
        emit readyRead();
    }
    else
    {
        //no se ha autenticado, se envia usuario y password

        // m_timerNoResponse->stop();

        emit working();

        QStringList data = m_dataReceived.split("\n",Qt::SkipEmptyParts);

        if ( data.size() == 0 )
            return;        

        QString line = data.last();
        line = line.simplified();

        qDebug() << m_ip << "line" << line;

        QRegularExpression exp;

        //enviamos el usuario
        exp.setPattern("(username|login|Username|Login)+:");
        if ( line.contains(exp,&match) )
        {
            qCDebug(qremoteshell) << m_ip << "enviando usuario" << m_user;
            sendCommand( m_user );
            return;
        }
        //enviamos el password
        exp.setPattern("(Password|password)+:");
        if ( line.contains(exp,&match) )
        {
            if ( !m_pwdsent )
            {
                m_pwdsent=true;
                qCDebug(qremoteshell) << m_ip << "enviando password" << m_pwd;
                sendCommand( m_pwd );
            }
            else if ( m_enablesent )
            {
                qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_terminal_detaReceived: Enviando Password de enable";
                sendCommand( "raisecom" ); //TODO implementar confiuracion para este password
            }
            else
            {
                qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_terminal_detaReceived: se pide nuevamente el pwd. Se desconecta";
                host_disconnect();
            }
            return;
        }

        //si se rechaza la conexion
        if (m_dataReceived.contains("Authorization failed") ||
            m_dataReceived.contains("authentication failed") ||
            m_dataReceived.contains("Connection closed by") ||
            m_dataReceived.contains("Connection timed out") ||
            m_dataReceived.contains("remote host not responding") ||
            m_dataReceived.contains("Connection refused") ||
            m_dataReceived.contains("Invalid key length") ||
            m_dataReceived.contains("Connection reset by peer") ||
            m_dataReceived.contains("Disconnected from") ||
            m_dataReceived.contains("no matching host key type found") ||
            m_dataReceived.contains("Login invalid")
            )
        {
            qCDebug(qremoteshell) << m_ip << "Connection refused. a probar con el siguiente protocolo.";

            if (m_dataReceived.contains("authentication failed")) _errortxt="authentication failed";
            if (m_dataReceived.contains("Connection closed by")) _errortxt="Connection closed by";
            if (m_dataReceived.contains("Connection timed out")) _errortxt="Connection timed out";
            if (m_dataReceived.contains("remote host not responding")) _errortxt="remote host not responding";
            if (m_dataReceived.contains("Connection refused")) _errortxt="Connection refused";
            if (m_dataReceived.contains("Invalid key length")) _errortxt="Invalid key length";
            if (m_dataReceived.contains("Connection reset by peer")) _errortxt="Connection reset by peer";
            if (m_dataReceived.contains("Disconnected from")) _errortxt="Disconnected from";
            if (m_dataReceived.contains("no matching host key type found")) _errortxt="no matching host key type found";
            if (m_dataReceived.contains("Login invalid")) _errortxt="Login invalid";

            m_connectionRefused=true;
            m_nextTry();
            return;
        }

        //verificamos si ya miramos el promt del equipo para saber que ya nos conectamos
        exp.setPattern("^\\w.+(#|>)\\s*$");
        if ( line.contains(exp,&match) )
        {
            qCDebug(qremoteshell) << m_ip << "Encontro el prompt";

            if ( line.contains(QRegularExpression("^\\w.+>\\s*$")) )
            {
                if ( !m_enablesent )
                {
                    qCDebug(qremoteshell) << m_ip << "se envia enable";
                    sendCommand( "enable" );
                    m_enablesent=true;
                }
                return;
            }
            else if ( !m_termle )
            {
                qCDebug(qremoteshell) << m_ip << "se envia terminal length 0";

                //enviando terminal length 0
                sendCommand( "terminal length 0" );
                // sendCommand( "sh int des" );
                m_termle=true;
                return;
            }
            else
            {
                qCDebug(qremoteshell) << m_ip << "conectado .......";
                m_host = match.captured(0).replace("#","").simplified();
                qDebug() << "HOSTNAME" << m_host;

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

        //HUAWEI
        exp.setPattern("^<.+> *$");
        if ( line.contains(exp,&match) )
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
                m_host = match.captured(0);
                m_host.replace("<","");
                m_host.replace(">","");
                m_host = m_host.simplified();
                m_vendor = "Huawei";

                finalizado();
                return;
            }
        }

        //MikroTik
        exp.setPattern("^\\[.+@(\\S+)\\] \\> *$");
        if ( line.contains(exp,&match) )
        {
            m_host = match.captured(1);
            m_vendor = "MikroTik";

            finalizado();
            return;
        }

        //local prompt
        if ( line.contains(m_localprompt) ||
             line.contains(QRegularExpression("^.+#\\s*$")) ) //si es gw
        {
            qCDebug(qremoteshell) << m_ip << "---------- localprompt ----------------";
            m_nextTry();
        }

//        qCDebug(qremoteshell) << m_ip << "m_timerNoResponse start";
        // m_timerNoResponse->start();
    }
}

void QRemoteShell::finalizado()
{
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::finalizado() conectado";
    m_hostConnected=true;
    emit connected();
}

void QRemoteShell::host_disconnect()
{
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::host_disconnect()";
    if ( m_telnet )
    {
        m_telnet->disconnect();
        m_telnet->host_disconnect();
    }
    if ( m_ssh )
    {
        m_ssh->disconnect();
        m_ssh->host_disconnect();
    }
    m_hostConnected=false;
    emit disconnected();
}

void QRemoteShell::sendData(const QByteArray &data)
{
    if ( m_telnet ) m_telnet->sendData( data );
    else if ( m_ssh ) m_ssh->sendData( data );
}

void QRemoteShell::sendCommand(QString cmd)
{    
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::sendCommand" << cmd;
    if ( m_telnet ) m_telnet->sendCommand( cmd );
    else if ( m_ssh ) m_ssh->sendCommand( cmd );
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

QString QRemoteShell::eliminarCaractaresMenorASCII32(QString txt)
{
    //eliminamos caracteres menores al ascii 32

    if ( txt.isEmpty() )
        return txt;

    int c=0;
    forever
    {
        char caracter = txt.at( c ).toLatin1();
        if ( caracter < 32 && caracter != 13 && caracter != 10 && caracter != 9 )
            txt.remove(c,1);
        else
            c++;

        if ( c == txt.size() )
            break;
    }
    return txt;
}
