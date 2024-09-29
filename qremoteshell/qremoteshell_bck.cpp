#include "qremoteshell.h"
#include <QDebug>
#include <QTcpSocket>
#include <QTimer>
#include <QThread>
#include "terminal/terminal.h"
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
    m_terminal = nullptr;
    m_pwdsent = false;
    m_gwConnected = false;
    m_socket = nullptr;
    m_pwd1sent = false;
    m_telnet = nullptr;
    m_ssh = nullptr;
    m_probardiferentesprotocolos=true;
    m_connectionRefused=false;
    setConnectionProtocol( SSHTelnet );
    m_localprompt.setPattern(linuxprompt);

    // m_timerNoResponse = new QTimer;
    // m_timerNoResponse->setInterval(15000);
    // m_timerNoResponse->setSingleShot(true);
    // connect(m_timerNoResponse,SIGNAL(timeout()),SLOT(m_timeroutNoResponse()));
}

QRemoteShell::~QRemoteShell()
{
    if ( m_terminal )
        delete m_terminal;
    if ( m_telnet )
        delete m_telnet;
    if ( m_ssh )
        delete m_ssh;
    if ( m_socket )
        delete m_socket;
    // delete m_timerNoResponse;
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

void QRemoteShell::setUsersPasswords(QStringList lst)
{
    if ( !lst.isEmpty())
    {
        m_lstUsersPasswords.append(lst);
        m_probardiferentesprotocolos=false;
    }
}

void QRemoteShell::host_connect()
{
    if ( m_gw.isEmpty() )
    {
        qDebug() << "H:" << m_ip << "U:" << m_user << "P:" << m_pwd;

        //conexion directa
        m_terminal = new Terminal( m_ip,m_linuxprompt );
        connect(m_terminal,SIGNAL(ready(bool)),SLOT(m_terminal_ready(bool)));
        connect(m_terminal,SIGNAL(receivedData(QString)),SLOT(m_terminal_detaReceived(QString)));
        connect(m_terminal,SIGNAL(finished()),SLOT(m_terminal_finished()));
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

void QRemoteShell::host_connect_new()
{
    m_nextTry();
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
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry() size" <<
        m_probardiferentesprotocolos <<
        m_lstConnectionProtocol.size() <<
        m_lstUsersPasswords.size();

    emit working();

    m_pwdsent=false;
    bool probar=false;
    if ( m_probardiferentesprotocolos )
    {
        if ( !m_lstConnectionProtocol.isEmpty() )
        {
            m_protocol = m_lstConnectionProtocol.takeFirst();
            probar=true;
        }
    }
    else
    {
        if ( !m_lstUsersPasswords.isEmpty() )
        {
            m_protocol = m_lstConnectionProtocol.first();
            QStringList data = m_lstUsersPasswords.takeFirst().split("\t",QString::SkipEmptyParts);
            if ( data.size() > 1 )
            {
                qCDebug(qremoteshell) << m_ip << "a probar usuario pwd" << data;
                m_user=data.first();
                m_pwd=data.last();
                probar=true;
            }
        }
    }

    if ( probar )
    {
        qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry()" << m_protocol;

//        //verificamos alcance
//        qint16 port=0;
//        switch (m_protocol)
//        {
//        case Telnet: port=23; break;
//        case SSH: port=22; break;
//        }
//        if ( m_socket )
//            delete m_socket;
//        qCDebug(qremoteshell) << m_ip  << "Probando alcance puerto" << port;
//        m_socket = new QTcpSocket(this); //usamos un socket para ver si el equipo es alcanzable
//        m_socket->connectToHost( m_ip, port );
//        m_socket->waitForConnected(5000);
//        if ( m_socket->state() == QAbstractSocket::ConnectedState ||
//             m_socket->error() == QAbstractSocket::ConnectionRefusedError ) //si se rechaza la conexion es alcanzable
//        {
//            m_socket->close(); //cerrar antes de enviar comandos
//            m_socket->waitForDisconnected(5000);
//            qCDebug(qremoteshell) << m_ip  << "QRemoteShell::host_connect() reachable" << m_ip;
//            emit reachable();
//        }
//        else //SocketTimeoutError no alcanzable
//        {
//            //desconectar regresar
//            m_socket->close();
//            qCDebug(qremoteshell) << m_ip  << "equipo no alcanzable" << m_socket->state() <<  m_socket->error();
//            host_disconnect();
//            return;
//        }

        //emit reachable(); se emite en case ssh o en telnet_host_connected() para telnet

        //alcanzable continamos
        if ( !m_gwConnected ) //m_gwConnected va a ser falso la primera conexion, o no si no se usa un gw
        {
            //conexion al equipo o gw

            QString ip = m_gw.isEmpty()?m_ip:m_gw;

            switch (m_protocol)
            {
            case Telnet:
            {
//                m_terminal->sendCommand( "telnet -l "+m_user+" "+ip );
                m_terminal->disconnect();

                if ( m_telnet  )
                {
                    m_telnet->disconnect();
                    m_telnet->host_disconnect();
                    m_telnet->deleteLater();
                    emit disconnected();
                }

                qCDebug(qremoteshell) << m_ip << "$$$$$$$$$$$$$$ TElnet";
                m_telnet = new QTelnet(m_ip,"23",this);
                connect(m_telnet,SIGNAL(connected()),SLOT(telnet_host_connected()));
                connect(m_telnet,SIGNAL(disconnected()),SLOT(telnet_host_disconnected()));
                connect(m_telnet,SIGNAL(readyRead()),SLOT(on_telnet_dataReceived()));
                m_telnet->host_connect();

                break;
            }
            case SSH:
            {
                emit reachable();
                m_terminal->sendCommand( "ssh "+m_user+"@"+ip );

                // m_terminal->disconnect();
                // m_ssh = new QSSHSession(m_ip,"22",m_user,m_pwd);
                // connect(m_ssh,SIGNAL(connected()),SLOT(ssh_host_connected()));
                // connect(m_ssh,SIGNAL(disconnected()),SLOT(ssh_host_disconnected()));
                // connect(m_ssh,SIGNAL(readyRead()),SLOT(on_ssh_dataReceived()));
                // m_ssh->connect_to();

                break;
            }
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

void QRemoteShell::m_nextTry_new()
{
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry() size" <<
        m_probardiferentesprotocolos <<
        m_lstConnectionProtocol.size() <<
        m_lstUsersPasswords.size();

    emit working();

    m_pwdsent=false;
    bool probar=false;
    if ( m_probardiferentesprotocolos )
    {
        if ( !m_lstConnectionProtocol.isEmpty() )
        {
            m_protocol = m_lstConnectionProtocol.takeFirst();
            probar=true;
        }
    }
    else
    {
        if ( !m_lstUsersPasswords.isEmpty() )
        {
            m_protocol = m_lstConnectionProtocol.first();
            QStringList data = m_lstUsersPasswords.takeFirst().split("\t",QString::SkipEmptyParts);
            if ( data.size() > 1 )
            {
                m_user=data.first();
                m_pwd=data.last();
                probar=true;
            }
        }
    }

    if ( probar )
    {
        qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry()" << m_protocol;

        //alcanzable continamos
        if ( !m_gwConnected ) //m_gwConnected va a ser falso la primera conexion, o no si no se usa un gw
        {
            //conexion al equipo o gw

            QString ip = m_gw.isEmpty()?m_ip:m_gw;

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
        else
        {
            //conexion del gw al equipo
            // switch (m_protocol)
            // {
            // case Telnet: m_terminal->sendCommand( "telnet "+m_ip ); break;
            // case SSH: m_terminal->sendCommand( "ssh "+m_ip ); break;
            // default: Q_UNREACHABLE();
            // }
        }
    }
    else
    {
        qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_nextTry() falla la conexion ssh o telnet";
        host_disconnect();
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
    emit reachable();
}

void QRemoteShell::ssh_host_disconnected()
{
    qCDebug(qremoteshell) << m_ip << Q_FUNC_INFO;
    emit disconnected();
}

void QRemoteShell::on_ssh_dataReceived()
{
    // qCDebug(qremoteshell) << m_ip << Q_FUNC_INFO;
    m_terminal_detaReceived( m_ssh->dataReceived() );
}

void QRemoteShell:: m_terminal_detaReceived(QString txt)
{
    m_dataReceived = eliminarCaractaresNoImprimibles(txt);

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

        QStringList data = m_dataReceived.split("\n",QString::SkipEmptyParts);

        if ( data.size() == 0 )
            return;

        QString line = data.last();
        line = line.simplified();

        qDebug() << m_ip << "line" << line;

        QRegExp exp;
        exp.setMinimal(true);

        //enviamos el usuario
        exp.setPattern("(username|login|Username|Login)+:");
        if ( line.contains(exp) )
        {
            //siguiente usuario

            QString user = m_user;
            if ( m_pwdsent )
            {
                //cambiamos de usuario
                //cambia del segundo en adelante intento
                if ( !m_lstUsersPasswords.isEmpty() )
                {
                    QStringList data = m_lstUsersPasswords.takeFirst().split("\t",QString::SkipEmptyParts);
                    if ( data.size() > 1 )
                    {
                        user=data.first();
                        m_pwd=data.last();
                    }
                }
                else
                {
                    //ya no hay mas usuarios
                    qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_terminal_detaReceived: No hay mas usuarios. Se desconecta";
                    host_disconnect();
                    return;
                }
            }

            qCDebug(qremoteshell) << m_ip << "enviando usuario" << user;
            sendCommand( user );
            return;
        }
        //enviamos el password
        exp.setPattern("(Password|password)+:");
        if ( line.contains(exp) )
        {
            if ( !m_pwd.isEmpty() )
            {
                QString pwd=m_pwd;
                m_pwd.clear();
                m_pwdsent=true;
                qCDebug(qremoteshell) << m_ip << "enviando password" << pwd;
                sendCommand( pwd );
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

        //si se rechaza la conexion ---- verificar
        if ( !m_gw.isEmpty() && !m_user2.isEmpty() )
        {
            //se esta conectado al GW y se rechazo el intento de conexion por el protocolo (Telnet SSH). Se intenta con el siguiente
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
                qCDebug(qremoteshell) << m_ip << "Connection refused, a probar con el siguiente protocolo";
                m_nextTry();
                return;
            }
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
            if ( m_probardiferentesprotocolos )
            {
                qCDebug(qremoteshell) << m_ip << "Connection refused, a probar con el siguiente protocolo.";
                m_nextTry();
            }
            else
            {
                qCDebug(qremoteshell) << m_ip << "Connection refused. No probar otros protocolos. Se desconecta";

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
                host_disconnect();
                return;
            }
        }

        //verificamos si ya miramos el promt del equipo para saber que ya nos conectamos
        exp.setPattern("^\\w.+(#|>)\\s*$");
        if ( line.contains(exp) )
        {
            qCDebug(qremoteshell) << m_ip << "Encontro el prompt";

            if ( line.contains(QRegExp("^\\w.+>\\s*$")) )
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
                m_host = exp.cap(0).replace("#","").simplified();
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

        //MikroTik
        exp.setPattern("^\\[.+@(\\S+)\\] \\> *$");
        if ( line.contains(exp) )
        {
            m_host = exp.cap(1);
            m_vendor = "MikroTik";

            finalizado();
            return;
        }

        //local prompt
        if ( line.contains(m_localprompt) ||
             line.contains(QRegExp("^.+#\\s*$")) ) //si es gw
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
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::host_disconnect()";
    if ( m_terminal )
    {
        disconnect(m_terminal,SIGNAL(receivedData(QString)),this,SLOT(m_terminal_detaReceived(QString)));
        m_terminal ? m_terminal->close() : emit disconnected();
    }
    else if ( m_telnet )
    {
        m_telnet->disconnect();
        m_telnet->host_disconnect();
    }
    else if ( m_ssh )
    {
        m_ssh->disconnect();
        m_ssh->host_disconnect();
    }
    m_hostConnected=false;
}

void QRemoteShell::sendData(const QByteArray &data)
{
    if ( m_terminal ) m_terminal->sendData( data );
    if ( m_telnet ) m_telnet->sendData( data );
    if ( m_ssh ) m_ssh->sendData( data );
}

void QRemoteShell::sendCommand(QString cmd)
{
    qCDebug(qremoteshell) << m_ip << "QRemoteShell::sendCommand" << cmd;
    if ( m_telnet ) m_telnet->sendCommand( cmd );
    else if ( m_ssh ) m_ssh->sendCommand( cmd );
    else if ( m_terminal ) m_terminal->sendCommand(cmd);
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

// void QRemoteShell::m_timeroutNoResponse()
// {
//     qCDebug(qremoteshell) << m_ip << "QRemoteShell::m_timeroutNoResponse()";
// //    QByteArray a(1,3);
// //    sendData( a );
//     sendData( QString(m_user+"\n").toUtf8() );
// }

QString QRemoteShell::eliminarCaractaresNoImprimibles(QString txt)
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
