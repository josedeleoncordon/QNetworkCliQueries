/*
Based on:
softengineer/qsshterm

https://github.com/softengineer/qsshterm
*/

#ifndef QSSHSESSION_H
#define QSSHSESSION_H

#include <QObject>

//include de las cabeceras de la libreria compilada con soporte DSA
#include </usr/local/include/libssh/libssh.h>
#include </usr/local/include/libssh/callbacks.h>

// #include </usr/include/libssh/libssh.h>
// #include </usr/include/libssh/callbacks.h>

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

    enum SSHError
    {
        NONE,
        OPTIONS_ERROR,
        AUTH_DENIED,
        CHANNEL_ERROR,
        SHELL_ERROR
    };
    SSHError error() { return m_error; }
    QString errortxt() { return m_errortxt; }

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

    // //utilizamos dlopen para abrir otra version de libssh en tiempo de ejecucion
    // //se espera que esta otra libssh este compilada con soporte para DSA para conexion con equipos legacy
    // void* handle;
    // typedef const char *(*ssh_get_error_t)(void *error);
    // typedef int (*ssh_channel_is_open_t)(ssh_channel channel);
    // ssh_get_error_t ssh_get_error;
    // ssh_channel_is_open_t ssh_channel_is_open;

    char buffer[1024];
    QTimer  *timer;
    QString _host;
    QString _port;
    QString _user;
    QString _password;
    bool _host_disconnected;

    SSHError m_error;
    QString m_errortxt;
    QString m_dataReceived;

    QSocketNotifier * read_notifier;
    // QSocketNotifier * error_notifier;
    void shell(ssh_session session);
    int authenticate_console(ssh_session session);

};

#endif // QSSHSESSION_H
