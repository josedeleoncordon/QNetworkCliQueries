/*
Based on:
softengineer/qsshterm

https://github.com/softengineer/qsshterm
*/

#ifndef QSSHSESSION_H
#define QSSHSESSION_H

#include <QObject>
#include </usr/include/libssh/libssh.h>
#include </usr/include/libssh/callbacks.h>

class QTimer;
class QSocketNotifier;

class QSSHSession : public QObject {
    Q_OBJECT

public:

    QSSHSession(QString host, QString port, QString user, QString pass, QObject *parent = 0);
    ~QSSHSession();
    void deinit();
    QString& dataReceived() { return m_dataReceived; }
    void sendCommand(QString str);
    void sendData(const QByteArray &command);

    /**
    * try to connect host again
    */

public slots:
    void connect_to();
    void host_disconnect();

signals:
    void connected();
    void disconnected();
    void readyRead();

private slots:
    void select_loop();

private :
    ssh_session session;
    ssh_channel channel;
    char buffer[1024];
    QTimer    *timer;
    QString _host;
    QString _port;
    QString _user;
    QString _password;

    QString m_dataReceived;


    QSocketNotifier * read_notifier;
    // QSocketNotifier * error_notifier;
    void shell(ssh_session session);
    int authenticate_console(ssh_session session);

};

#endif // QSSHSESSION_H
