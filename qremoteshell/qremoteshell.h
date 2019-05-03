#ifndef QREMOTESHELL_H
#define QREMOTESHELL_H

#include <qobject.h>
#include <QFile>
#include <QTextStream>

#include "qremoteshell_global.h"

class Connection;

class QREMOTESHELLSHARED_EXPORT QRemoteShell : public QObject
{
    Q_OBJECT

public:

    enum ConnectionProtocol
    {        
        Telnet,
        SSH,
        TelnetSSH,
        SSHTelnet
    };

    enum ConnectionType
    {
        Lib,
        Cli
    };

    QRemoteShell( QString ip, QString user, QString pwd, QString platform, QObject *parent=0 );
    ~QRemoteShell();

    void setConnectionProtocol( ConnectionProtocol cp ) { m_protocol = cp; }
    void setConnectionType( ConnectionType ct) { m_connectiontype = ct; }
    void setCountry( QString country ) { m_country = country; }
    void setGW(QString GW);
    void host_connect();
    void host_disconnect();
    void sendData(const QByteArray &data);
    void sendCommand(QString cmd);
    void disconnectReceiveTextSignalConnections();
    bool isConnected() { return m_hostConnected; }
    QString hostName();
    QString brandName();
    QString platform();
    void setLogPath(QString path);
    QString& dataReceived() { return m_dataReceived; }

private:
    ConnectionProtocol m_protocol;
    ConnectionType m_connectiontype;
    QString m_ip;
    QString m_user;
    QString m_pwd;
    QString m_country;
    QString m_platform;
    QString m_gw;
    Connection *m_Connection;
    bool m_hostConnected;
    QString m_dataReceived;

    //debug
    QString m_logPath;
    QFile m_logFile;
    QTextStream m_out;

    void saveLog(QString);

private slots:
    void m_connected();
    void m_disconnected();
    void m_receiveText();

signals:
    void reachable();
    void connected();
    void disconnected();
    void readyRead();
};

#endif // QREMOTESHELL_H
