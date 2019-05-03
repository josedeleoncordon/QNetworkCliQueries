#include "connection.h"

Connection::Connection(QString ip, QString port, QString user, QString pwd, QString platform, QObject *parent):
    QObject(parent)
{
    m_ip = ip;
    m_port = port;
    m_user = user;
    m_pwd = pwd;
    m_platform = platform;
    m_termle = false;   
    m_connected = false;
}

Connection::~Connection()
{
}

void Connection::setLogPath(QString path)
{
    if ( !path.isEmpty() )
    {
        m_logPath = path;
        m_logFile.setFileName( m_logPath );
        m_logFile.open(QIODevice::Append | QIODevice::Text);
        m_out.setDevice(&m_logFile);
    }
}

void Connection::saveLog(QString txt)
{
    if ( m_logFile.isOpen() )
    {
        m_out << txt;
        m_out.flush();
    }
}
