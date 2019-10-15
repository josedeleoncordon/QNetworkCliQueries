#ifndef QREMOTESHELL_H
#define QREMOTESHELL_H

#include <QObject>
#include <QFile>
#include <QTextStream>

#include "qremoteshell_global.h"

class QTcpSocket;
class QTimer;
class Terminal;

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

    QRemoteShell( QString ip, QString user, QString pwd, QString platform, QString linuxprompt,QObject *parent=0 );
    ~QRemoteShell();

    void setConnectionProtocol( ConnectionProtocol cp );
    void setGW(QString GW) { m_gw = GW; }
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
    QList<ConnectionProtocol> m_lstConnectionProtocol;
    QList<ConnectionProtocol> m_lstConnectionProtocolgw;
    QString m_ip;
    QString m_user;
    QString m_pwd;
    QString m_platform;
    QString m_gw;
    QString m_vendor;
    QString m_host;
    QString m_linuxprompt;
    bool m_hostConnected;
    bool m_termle;
    bool m_pwdsent;
    bool m_gwConnected;
    QString m_dataReceived;
    QRegExp m_localprompt;
    QTimer *m_timerNoResponse;
    QTcpSocket *m_socket;

    Terminal *m_terminal;

    //debug
    QString m_logPath;
    QFile m_logFile;
    QTextStream m_out;

    void saveLog(QString);
    void m_nextTry();

    void finalizado();

private slots:
    void m_terminal_ready(bool ready);
    void m_terminal_detaReceived(QString);
    void m_terminal_finished();    
    void m_timeroutNoResponse();

signals:
    void reachable();
    void connected();
    void disconnected();
    void readyRead();
};

#endif // QREMOTESHELL_H
