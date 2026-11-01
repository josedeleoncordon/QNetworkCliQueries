#include "sshmanager.h"
#include <QDebug>
#include <QMutexLocker>

SshManager *SshManager::m_instance = nullptr;

SshManager* SshManager::instance()
{
    if (!m_instance)
        m_instance = new SshManager;
    return m_instance;
}

SshManager::SshManager()
{
    idsCount=0;
}

SshManager::~SshManager()
{
    qDebug() << "SshManager::~SshManager()";
    clear();
}

void SshManager::clear()
{
    for (auto &ctx : channels) {
        if (ctx.channel) {
            ssh_channel_close(ctx.channel);
            ssh_channel_free(ctx.channel);
        }
    }

    if (session_gw) {
        ssh_disconnect(session_gw);
        ssh_free(session_gw);
    }

    delete keepAlive;
    delete notifier;
    gw_enabled=false;
}

bool SshManager::initConnection(QString host, QString port, QString user, QString pass)
{
    _host=host;
    _port=port;
    _user=user;
    _pass=pass;
    return initConnection();
}

bool SshManager::initConnection()
{
    if (gw_enabled)
        return false;

    ssh_init();

    session_gw = ssh_new();

    int verbosity = 1;
    ssh_options_set(session_gw, SSH_OPTIONS_HOST, _host.toLocal8Bit());
    ssh_options_set(session_gw, SSH_OPTIONS_PORT, _port.toLocal8Bit());
    ssh_options_set(session_gw, SSH_OPTIONS_USER, _user.toLocal8Bit());
    ssh_options_set(session_gw, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    ssh_options_parse_config(session_gw, "~/.ssh/config");

    if (ssh_connect(session_gw) != SSH_OK)
    {
        qDebug() << "Error:" << ssh_get_error(session_gw);
        emit error(-1, ssh_get_error(session_gw));
        return false;
    }

    int rc = ssh_userauth_password(session_gw,
                                   _user.toLocal8Bit(),
                                   _pass.toLocal8Bit());

    if (rc != SSH_AUTH_SUCCESS)
    {
        qDebug() << "Error:" << ssh_get_error(session_gw);
        emit error(-1, ssh_get_error(session_gw));
        return false;
    }

    keepAlive = new QTimer;
    connect(keepAlive, &QTimer::timeout,
            this, &SshManager::sendKeepAlive);
    keepAlive->start(180000);

    qDebug() << "SshManager::initConnection OK";
    gw_enabled = true;
    setupNotifier();
    return true;
}

void SshManager::sendKeepAlive()
{
    if (!session_gw)
        return;

    const char *data = "";
    int rc = ssh_send_ignore(session_gw,data);
}

void SshManager::setupNotifier()
{
    qDebug() << "SshManager::setupNotifier()";
    int fd = ssh_get_fd(session_gw);

    notifier = new QSocketNotifier(fd, QSocketNotifier::Read);
    connect(notifier, &QSocketNotifier::activated,
            this, &SshManager::onSocketReadable,Qt::QueuedConnection);
}

// bool SshManager::isSessionAlive()
// {
//     qDebug() << "SshManager::isSessionAlive()";
//     if (!session_gw)
//     {
//         qDebug() << "111";
//         return false;
//     }

//     if (!ssh_is_connected(session_gw))
//     {
//         qDebug() << "222";
//         return false;
//     }

//     ssh_channel ch = ssh_channel_new(session_gw);

//     if (!ch)
//     {
//         qDebug() << "333";
//         return false;
//     }

//     int rc = ssh_channel_open_session(ch);

//     if (rc != SSH_OK)
//     {
//         qDebug() << "444";
//         ssh_channel_free(ch);
//         return false;
//     }

//     ssh_channel_close(ch);
//     ssh_channel_free(ch);

//     return true;
// }

int SshManager::createClientID(QString ip)
{
    QMutexLocker locker(&m_mutex1);
    int id = ++idsCount;
    qDebug() << ip << "SshManager::createClientID()" << id;
    qDebug() << "SshManager::createClientID()" << ip << id;
    return id;
}

void SshManager::createShell(int id, QString ip)
{
    // QMutexLocker locker(&m_mutex2);

    if (!gw_enabled)
        return;

    // if (!isSessionAlive())
    // {
    //     qDebug() << "Reconectando con Jump Server";
    //     clear();
    //     if (!initConnection())
    //     {
    //         qDebug() << "No se pudo reconectar con el Jump Server";
    //         return;
    //     }
    // }

    ssh_channel channel = ssh_channel_new(session_gw);

    if (ssh_channel_open_session(channel) != SSH_OK) {
        qDebug() << ip << "SshManager::createShell error 1" << ssh_get_error(session_gw)
                 << "cantidad de canales" << channels.size();
        qDebug() << "SshManager::createShell error 1" << ip << ssh_get_error(session_gw)
                 << "cantidad de canales" << channels.size();
        qInfo() << "SshManager::createShell error 1" << ip << ssh_get_error(session_gw)
                 << "cantidad de canales" << channels.size();
        emit error(id, ssh_get_error(session_gw));
        return;
    }

    ssh_channel_request_pty(channel);

    if (ssh_channel_request_shell(channel) != SSH_OK) {
        qDebug() << ip << "SshManager::createShell error 2" << ssh_get_error(session_gw)
                 << "cantidad de canales" << channels.size();
        qDebug() << "SshManager::createShell error 2" << ip << ssh_get_error(session_gw)
                 << "cantidad de canales" << channels.size();
        qInfo() << "SshManager::createShell error 2" << ip << ssh_get_error(session_gw)
                 << "cantidad de canales" << channels.size();
        emit error(id, ssh_get_error(session_gw));
        return;
    }

    channels[id].channel = channel;

    qDebug() << ip << "SshManager::createShell idSShManagerClient" << id << "OK";
    qDebug() << "SshManager::createShell idSShManagerClient" << id << ip << "OK";
    emit connected(id);
}

void SshManager::sendCommand(int id, QString cmd, QString ip)
{
    // QMutexLocker locker(&m_mutex3);

    if (!channels.contains(id))
    {
        qDebug() << ip << "SshManager::sendCommand error: sin canal para enviar comando" << id << cmd;
        return;
    }

    auto &ctx = channels[id];

    QByteArray data = (cmd + "\n").toLocal8Bit();
    ssh_channel_write(ctx.channel, data.data(), data.size());
}

void SshManager::closeChannel(int id, QString ip)
{    
    // QMutexLocker locker(&m_mutex4);
    qDebug() << ip << "SshManager::closeChannel" << id;
    qDebug() << "SshManager::closeChannel" << id;

    if (!channels.contains(id))
        return;

    auto &ctx = channels[id];

    if (ctx.channel) {
        qDebug() << ip << "SshManager::closeChannel Eliminado" << id;
        qDebug() << "SshManager::closeChannel Eliminado" << id;
        ssh_channel_close(ctx.channel);
        ssh_channel_free(ctx.channel);
    }

    channels.remove(id);
    if (channels.isEmpty())
    {
        idsCount=0;
        qDebug() << "SshManager IDCLIENTES reinicio a 0";
    }
    qDebug() << "SshManager::closeChannel cantidad de canales" << channels.size();
    emit disconnected(id);
}

void SshManager::onSocketReadable()
{
    for (auto it = channels.begin(); it != channels.end(); ++it)
    {
        ssh_channel ch = it->channel;
        if (!ch) continue;

        QByteArray data;

        int nbytes;
        do {
            nbytes = ssh_channel_read_nonblocking(ch, buffer, sizeof(buffer), 0);
            if (nbytes > 0)
                data.append(buffer, nbytes);
        } while (nbytes > 0);

        if (!data.isEmpty())
        {
            // qDebug() << "emit readyRead" << it.key();
            emit readyRead(it.key(), data);
        }
    }
}
