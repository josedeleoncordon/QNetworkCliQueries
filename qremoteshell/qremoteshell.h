#ifndef QREMOTESHELL_H
#define QREMOTESHELL_H

#include <QObject>
#include <QFile>
#include <QTextStream>

#include "qremoteshell_global.h"

class QTcpSocket;
class QTimer;
class Terminal;
class QTelnet;
class QSSHSession;

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

    QRemoteShell( QString ip, QString user, QString pwd, QString platform, QString linuxprompt,QObject *parent=nullptr );
    ~QRemoteShell();

    void setConnectionProtocol( ConnectionProtocol cp );
    void host_connect();
    void host_disconnect();
    void sendData(const QByteArray &data);
    void sendCommand(QString cmd);
    void disconnectReceiveTextSignalConnections();
    bool isConnected() { return m_hostConnected; }    
    QString hostName();
    QString brandName();
    QString platform();
    QString& dataReceived() { return m_dataReceived; }
    bool connectionRefused() { return m_connectionRefused; }
    QString errorTxt() { return _errortxt; }
    void setSshModeLibSshEnabled(bool enabled) { m_sshmodelibssh=enabled; }
    bool lastLibSshFailed() { return m_lastlibsshfailed; }

private:
    ConnectionProtocol m_protocol;    
    QList<ConnectionProtocol> m_lstConnectionProtocol;
    QList<ConnectionProtocol> m_lstConnectionProtocolgw;
    QString m_ip;
    QString m_user;
    QString m_pwd;
    QString m_platform;
    QString m_vendor;
    QString m_host;
    QString m_linuxprompt;
    bool m_hostConnected;
    bool m_termle;
    bool m_enablesent;
    bool m_pwdsent;
    bool m_connectionRefused;
    QString _errortxt;
    QString m_dataReceived;
    QRegExp m_localprompt;
    QTelnet *m_telnet;
    QSSHSession *m_ssh;
    bool m_sshok;
    bool m_sshmodelibssh;
    short m_consultaIntentos;
    bool m_lastlibsshfailed;

    Terminal *m_terminal;

    QString eliminarCaractaresNoImprimibles(QString txt);
    void m_nextTry();
    void finalizado();

private slots:
    void m_protocolTry();
    void telnet_host_connected();
    void telnet_host_disconnected();
    void on_telnet_dataReceived();
    void ssh_host_connected();
    void ssh_host_disconnected();
    void on_ssh_dataReceived();
    void m_terminal_ready(bool ready);
    void m_terminal_detaReceived(QString);
    void m_terminal_finished();    

signals:
    void reachable();
    void connected();
    void disconnected();
    void readyRead();
    void working();
};

#endif // QREMOTESHELL_H
