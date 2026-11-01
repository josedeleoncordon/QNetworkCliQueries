#ifndef SSHMANAGER_H
#define SSHMANAGER_H

//Conexion ssh a un Jump Server y a partir de este abrir multiples canales/shells para conectarnos a diferentes
//equipos con una sola autenticacion

#include <QObject>
#include <QMap>
#include <QSocketNotifier>
#include <QMutex>
#include <QTimer>

//include de las cabeceras de la libreria compilada con soporte DSA
#include </usr/local/include/libssh/libssh.h>
#include </usr/local/include/libssh/callbacks.h>

class SshManager : public QObject
{
    Q_OBJECT
public:
    static SshManager* instance();
    bool initConnection(QString host, QString port, QString user, QString pass);
    ~SshManager();

    bool gwEnabled() { return  gw_enabled;}
    int createClientID(QString ip="");

signals:

    void connected(int id);
    void readyRead(int id, QByteArray data);
    void disconnected(int id);
    void error(int id, QString err);

public slots:

    void createShell(int id,QString ip="");
    void sendCommand(int id, QString cmd, QString ip="");
    void closeChannel(int id,QString ip="");

private slots:
    void sendKeepAlive();
    void onSocketReadable();

private:
    explicit SshManager();

    static SshManager *m_instance;

    ssh_session session_gw = nullptr;
    bool gw_enabled = false;

    struct ChannelCtx {
        ssh_channel channel = nullptr;
    };

    QMap<int, ChannelCtx> channels;
    QSocketNotifier *notifier = nullptr;

    char buffer[1024];

    QMutex m_mutex1;
    // QMutex m_mutex2;
    // QMutex m_mutex3;
    // QMutex m_mutex4;
    int idsCount;

    QTimer *keepAlive;

    QString _host;
    QString _port;
    QString _user;
    QString _pass;

    bool initConnection();
    void setupNotifier();
    // bool isSessionAlive();
    void clear();
};

#endif // SSHMANAGER_H
