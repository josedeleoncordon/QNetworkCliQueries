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
    void setGW(QString GW) { m_gw = GW; }
    void host_connect();
    void host_connect_new();
    void host_disconnect();
    void setUser2(QString user) { m_user2=user; }
    void setPassword2(QString pwd) { m_pwd2=pwd; }
    void setUsersPasswords(QStringList lst);
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

private:
    ConnectionProtocol m_protocol;
    QList<ConnectionProtocol> m_lstConnectionProtocol;
    QList<ConnectionProtocol> m_lstConnectionProtocolgw;
    QString m_ip;
    QString m_user;
    QString m_user2;
    QString m_pwd;
    QString m_pwd2;
    QStringList m_lstUsersPasswords;
    QString m_platform;
    QString m_gw;
    QString m_vendor;
    QString m_host;
    QString m_linuxprompt;
    bool m_hostConnected;
    bool m_termle;
    bool m_enablesent;
    bool m_pwdsent;
    bool m_gwConnected;
    bool m_pwd1sent;
    bool m_probardiferentesprotocolos;
    bool m_connectionRefused;
    QString _errortxt;
    QString m_dataReceived;
    QRegExp m_localprompt;
    // QTimer *m_timerNoResponse;
    QTcpSocket *m_socket;
    QTelnet *m_telnet;
    QSSHSession *m_ssh;

    Terminal *m_terminal;

    QString eliminarCaractaresNoImprimibles(QString txt);
    void m_nextTry();
    void m_nextTry_new();

    void finalizado();

private slots:
    void telnet_host_connected();
    void telnet_host_disconnected();
    void on_telnet_dataReceived();
    void ssh_host_connected();
    void ssh_host_disconnected();
    void on_ssh_dataReceived();
    void m_terminal_ready(bool ready);
    void m_terminal_detaReceived(QString);
    void m_terminal_finished();
    // void m_timeroutNoResponse();

signals:
    void reachable();
    void connected();
    void disconnected();
    void readyRead();
    void working();
};

#endif // QREMOTESHELL_H
