#include "qsshconnection.h"
//#include <argumentscollector.h>

QSshConnection::QSshConnection(QString ip, QString port, QString user, QString pwd, QString platform, QObject *parent):
    Connection(ip,port,user,pwd,platform,parent)
{
    m_connection=NULL;
//    m_timer_keepAlive = new QTimer;
//    m_timer_keepAlive->setInterval( 8000 );
//    connect(m_timer_keepAlive,SIGNAL(timeout()),SLOT(on_m_timer_keepAlive_time_out()));
}

QSshConnection::~QSshConnection()
{}

void QSshConnection::host_connect()
{
    //usando QSsh-master_segundo

    QSsh::SshConnectionParameters params;
    params.setHost( m_ip );
    params.setUserName( m_user );
    params.setPassword( m_pwd );
    params.setPort(22);
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypePassword;
    params.timeout = 300;    

    m_connection = new QSsh::SshConnection(params,this);
    connect(m_connection, SIGNAL(reachable()), SIGNAL(reachable()));
    connect(m_connection, SIGNAL(connected()), SLOT(handleConnected()));
    connect(m_connection, SIGNAL(error(QSsh::SshError)), SLOT(onConnectionError(QSsh::SshError)));

    m_connection->connectToHost();
}

void QSshConnection::handleConnected()
{
    m_shell = m_connection->createRemoteShell();
    connect(m_shell.data(), SIGNAL(readyReadStandardOutput()), SLOT(handleRemoteStdout()));
    connect(m_shell.data(), SIGNAL(readyReadStandardError()), SLOT(handleRemoteStderr()));
    connect(m_shell.data(), SIGNAL(closed(int)), SIGNAL(disconnected()));
    m_shell->start();
}

void QSshConnection::onConnectionError(QSsh::SshError err)
{    
    qDebug() << "QSshConnection::onConnectionError(QSsh::SshError err)" << err << m_connection->errorString();
    host_disconnect();
}

void QSshConnection::handleRemoteStdout()
{    
    m_dataReceived = m_shell->readAllStandardOutput().data();

    emit readyRead();

    if ( !m_connected )
    {
        //SSh de una vez autentica, solo queda enviar terminal length 0
        QStringList data = m_dataReceived.split("\n",QString::SkipEmptyParts);

        if ( data.size() == 0 )
            return;

        QString line = data.last();
        line = line.simplified();

        QRegExp exp;
        exp.setMinimal(true);

        //verificamos si ya miramos el promt del cisco para saber que ya nos conectamos
        exp.setPattern("^.+#\\s*$");
        if ( line.contains(exp) )
        {
            if ( !m_termle )
            {
                //enviando terminal length 0
//                sendCommand( "" );
                sendCommand( "terminal length 0" );
                m_termle=true;
                return;
            }
            else
            {
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

                m_connected=true;
                m_brand="Cisco";

                emit connected();
//                m_timer_keepAlive->start();
                return;
            }
        }

        //verificamos si ya miramos el prompt de huawei
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
                m_brand = "Huawei";
                m_connected=true;
                emit connected();
                return;
            }
        }
    }
}

void QSshConnection::handleRemoteStderr()
{
    m_dataReceived = m_shell->readAllStandardOutput().data();
    emit readyRead();
}

void QSshConnection::host_disconnect()
{
    qDebug() << "QSshConnection::host_disconnect()";
    if ( m_connection )
    {
        m_connection->disconnect();
        m_connection->closeAllChannels();
        m_connection->deleteLater();
        m_connection = NULL;
        emit disconnected();
    }
}

void QSshConnection::sendData(const QByteArray &data)
{
    qDebug() << "QSshConnection::sendData" << data;
    if ( m_shell )
        m_shell->write(data);
}

void QSshConnection::sendCommand(QString str)
{
    qDebug() << "QSshConnection::sendCommand" << str;
    str.append("\n");
    if ( m_shell )
        m_shell->write(str.toStdString().c_str());
}

void QSshConnection::on_m_timer_keepAlive_time_out()
{
}
