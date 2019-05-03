#include "cliconnection.h"

CliConnection::CliConnection(QString ip, QString port, QString user, QString pwd, QString platform, QObject *parent) :
    Connection(ip,port,user,pwd,platform,parent)
{
    process=nullptr;
}

void CliConnection::host_connect()
{
    process = new QProcess(this);
    connect(process,SIGNAL(readyRead()),SLOT(on_process_readyReadStandardOutput()));

    QStringList arguments;
    arguments << m_ip;

    qDebug() << arguments;
    process->start("telnet",arguments);
}

void CliConnection::on_process_readyReadStandardOutput()
{
    QByteArray data = process->readAll();
    qDebug() << "salida";
    qDebug() << data;

//    emit disconnected();
}

void CliConnection::host_disconnect()
{}

void CliConnection::sendData(const QByteArray &data)
{}

void CliConnection::sendCommand(QString str)
{}
