#ifndef QSSHCONNECTION_H
#define QSSHCONNECTION_H

#include <QSharedPointer>
#include <QTimer>
#include "connection.h"

#include "sshconnection.h"
#include "sshremoteprocess.h"

namespace QSsh {
class SshConnection;
class SshConnectionParameters;
class SshRemoteProcess;
}

class QREMOTESHELLSHARED_EXPORT QSshConnection : public Connection
{
    Q_OBJECT
private:
    QSsh::SshConnection *m_connection;
    QSharedPointer<QSsh::SshRemoteProcess> m_shell;
    QTimer *m_timer_keepAlive;
public:
    explicit QSshConnection(QString ip, QString port, QString user, QString pwd, QString platform, QObject *parent = 0);
    virtual ~QSshConnection();

    void host_connect();
    void host_disconnect();
    void sendData(const QByteArray &data);
    void sendCommand(QString str);

private slots:
    void handleConnected();
    void onConnectionError(QSsh::SshError);
    void handleRemoteStdout();
    void handleRemoteStderr();
    void on_m_timer_keepAlive_time_out();

};

#endif // QSSHCONNECTION_H
