#ifndef CLICONNECTION_H
#define CLICONNECTION_H

#include <QObject>
#include <QProcess>
#include "connection.h"

class QREMOTESHELLSHARED_EXPORT CliConnection : public Connection
{
    Q_OBJECT
private:
    QProcess *process;

public:
    explicit CliConnection(QString ip, QString port, QString user, QString pwd, QString platform, QObject *parent = nullptr);

    void host_connect();
    void host_disconnect();
    void sendData(const QByteArray &data);
    void sendCommand(QString str);

public slots:
    void on_process_readyReadStandardOutput();

};

#endif // CLICONNECTION_H
