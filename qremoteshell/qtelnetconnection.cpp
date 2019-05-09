#include "qtelnetconnection.h"
#include "qtelnet.h"

QTelnetConnection::QTelnetConnection(QString ip, QString port, QString user, QString pwd, QString platform, QObject *parent):
    Connection(ip,port,user,pwd,platform,parent)
{
    m_connection=nullptr;
    m_connectToGTW=false;
    _ip = ip;

    //para los equipos que no envian texto sin que se les envie algun dato
    //a los dos segundos si no se reciben datos se envia algun dato
    //por ejemplo los ETX
    timer = new QTimer;
    timer->setInterval( 2000 );
    timer->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),SLOT(on_timer_timeOut()));
}

QTelnetConnection::~QTelnetConnection()
{}

void QTelnetConnection::setCountry( QString country )
{
    m_country = country;
}

void QTelnetConnection::setGTW( QString gtw )
{
    m_gtw = gtw;
}

void QTelnetConnection::host_connect()
{
    if ( m_connection )
    {
        m_connection->disconnect();
        m_connection->host_disconnect();
        m_connection->deleteLater();
        emit disconnected();
    }

    m_connection = new QTelnet(_ip,m_port,this);    
    connect(m_connection,SIGNAL(connected()),SLOT(host_connected()));
    connect(m_connection,SIGNAL(disconnected()),SLOT(host_disconnected()));
    connect(m_connection,SIGNAL(readyRead()),SLOT(on_m_connection_dataReceived()));
    m_connection->host_connect();
}

void QTelnetConnection::host_disconnect()
{
    if ( m_connection )
    {
        m_connection->disconnect();
        m_connection->host_disconnect();
        m_connection->deleteLater();
        emit disconnected();
    }
}

void QTelnetConnection::host_connected()
{
    emit reachable();
    timer->start();
}

void QTelnetConnection::host_disconnected()
{
    if ( !m_connected &&
         !m_connectToGTW )
    {
        //si no se ha logrado conectar y la IP no funciono probamos primero por el GW
        if ( m_gtw.isEmpty() )
            emit disconnected();
        else
        {
            m_connectToGTW = true;
            _ip = m_gtw;

            //borramos aca para que no emita disconnected
            m_connection->disconnect();
            m_connection->host_disconnect();
            m_connection->deleteLater();
            m_connection=nullptr;

            host_connect();
        }
    }
    else
    {
        m_connected = false;
        emit disconnected();
    }
}

void QTelnetConnection::on_m_connection_dataReceived()
{
    m_dataReceived = m_connection->dataReceived();    

    timer->stop();
    if ( !m_connected )
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

        //RAD ETX

        //ETX-201-102
        if ( (line.contains("[03;03HUSER NAME:") &&
             line.contains("[21;01HESC - clear; & - exit")) ||
             m_platform == "ETX-201" ||
             m_platform == "ETX-102")
        {
            saveLog( "\nenviando usuario y password su 1234\n" );
            m_brand = "RAD";
            m_platform = "ETX-102/201/202";
            sendCommand( "su" ); //TODO enviar password de una confoguracion
            sendCommand( "1234" );
            return;
        }

        //ETX-1300
        if ( (line.contains("[4;1HUSER NAME:") &&
             line.contains("[1mESC - clear; CTRL+X")) ||
             m_platform == "ETX-1300" )
        {
            saveLog( "\nenviando usuario y password\n" );
            m_brand = "RAD";
            m_platform = "ETX-1300";
            sendCommand( m_user );
            sendCommand( m_pwd );
        }

        if ( line.contains("user>") )
        {
            m_brand = "RAD";
            m_platform = "ETX-1";
            sendCommand( "su" );
            return;
        }
        if ( line.contains("password>") )
        {
            sendCommand( "1234" );
            return;
        }

        exp.setPattern("^.+#\\s*$");
        if ( line.contains(exp) && m_platform == "ETX-1" )
        {
            m_host = exp.cap(0).replace("#","").simplified();
            m_connected=true;
            m_connection->setSendAllData(true);
            emit connected();
            return;
        }

        if ( m_platform.contains("ETX") &&
             m_dataReceived.contains("Main Menu") )
        {
            m_connection->setSendAllData(true);
            saveLog( "\nEquipo conectado ++++\n" );

            //TODO corregir para que saque el nombre de los ETX-1300
            //no urge ya que viene el nombre por SNMP

            //eliminamos caracteres menores al ascii 32
            int c=0;
            forever
            {
                char caracter = m_dataReceived.at( c ).toLatin1();
                if ( caracter < 32 )
                    m_dataReceived.remove(c,1);
                else
                    c++;

                if ( c == m_dataReceived.size() )
                    break;
            }
            m_dataReceived.replace("[01;01H","");

            exp.setPattern("\\[01;\\d+H(.{4,})\\[01;");
            if ( m_dataReceived.contains(exp) )
                m_host = exp.cap(1);

            m_connected=true;
            emit connected();
            return;
        }

        //CISCO ATN

        //enviamos el usuario
        exp.setPattern("(username|login|Username|Login)+:");
        if ( line.contains(exp) )
        {
            saveLog( "\nenviando usuario "+ m_user +"\n" );
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
             m_dataReceived.contains("Connection closed by foreign host") ||
             m_dataReceived.contains("Connection timed out") ||
             m_dataReceived.contains("remote host not responding") ||
             m_dataReceived.contains("Connection refused") )
        {
            host_disconnect();
            return;
        }
        //**************************************************************************

        //verificamos si ya miramos el promt del equipo para saber que ya nos conectamos
        exp.setPattern("^.+#\\s*$");
        if ( line.contains(exp) )
        {
            saveLog( "\nEncontro el prompt\n" );

            //Si nos conectamos a un gtw nos conectamos al equipo deseado
            if ( m_connectToGTW )
            {
                _ip = m_ip;
                sendCommand( "telnet "+_ip );
                m_connectToGTW=false;
                return;
            }
            else
            {
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

                    m_connected=true;
                    m_connection->setSendAllData(true);
                    m_brand="Cisco";

                    emit connected();
                    return;
                }
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
                m_brand = "Huawei";
                m_connected=true;
                m_connection->setSendAllData(true);

                emit connected();
                return;
            }
        }
    }
    else
    {     
        emit readyRead();

        //si estabamos en un GW ya conectados y se cierra la conexion con el otro equipo
        if ( m_dataReceived.contains("closed by foreign host") )
        {
            host_disconnect();
            return;
        }
    }
    return;
}

void QTelnetConnection::on_timer_timeOut()
{
    sendCommand("");
}

void QTelnetConnection::sendData(const QByteArray &data)
{
    if ( m_connection )
        m_connection->sendData( data );
}

void QTelnetConnection::sendCommand(QString str)
{
    if ( m_connection )
        m_connection->sendCommand( str );
}
