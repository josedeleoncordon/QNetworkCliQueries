#include "qsshsession.h"

#include <QDebug>
#include <QTimer>
#include <QSocketNotifier>
#include <qremoteshelllogging.h>

QSSHSession::QSSHSession(QString host, QString port, QString user, QString pass, QObject *parent )
    : QObject{parent}, session(0), channel(0)
{
    _host = host;
    _port = port;
    _user = user;
    _password = pass;

    read_notifier = nullptr;
}

QSSHSession::~QSSHSession()
{
    qCDebug(qsshsession) << _host << "QSSHSession::~QSSHSession()";
    if ( read_notifier ) delete read_notifier;
}

int QSSHSession::authenticate_console(ssh_session session){
    int rc;

    //Try to authenticate with password
    rc = ssh_userauth_password(session, _user.toLocal8Bit(), _password.toLocal8Bit());
    if (rc == SSH_AUTH_ERROR) {
        qCDebug(qsshsession) << _host << "Error al autenticar";
        host_disconnect();
        return rc;
    }
    return rc;
}

void QSSHSession::select_loop(){
    if (ssh_channel_is_open(channel))
    {
        int nbytes = 0;
        m_dataReceived.clear();
        // char buffer[1024];
        do
        {
            nbytes = ssh_channel_read_nonblocking(channel, buffer, sizeof(buffer), 0);
            if (nbytes > 0) {
                // qCDebug(qsshsession) << "nbytes" << nbytes << "buffer --" << buffer << "--";
                m_dataReceived.append( QByteArray( buffer,nbytes ) );
            }
        }
        while(nbytes > 0 && ssh_channel_is_open(channel) && !ssh_channel_is_eof(channel));
        // qCDebug(qsshsession) << "saliendo de select_loop()" << _host;
        if ( !m_dataReceived.isEmpty() )
            emit readyRead();
    }
    else
        host_disconnect();
}

void QSSHSession::shell(ssh_session session){
    this->channel = ssh_channel_new(session);
    // ssh_channel_accept_x11(channel, 10);
    if(ssh_channel_open_session(channel)){
        qCDebug(qsshsession) << _host << "error opening channel" << ssh_get_error(session);
        host_disconnect();
        return;
    }

    ssh_channel_request_pty(channel);
    if(ssh_channel_request_shell(channel)){
        qCDebug(qsshsession) << _host << "Requesting shell :" << ssh_get_error(session);
        host_disconnect();
        return;
    }

    ssh_channel_change_pty_size(channel,80, 24);

    socket_t socket = ssh_get_fd(session);

    read_notifier = new QSocketNotifier(socket,
                                        QSocketNotifier::Read,
                                        this);
    read_notifier->setEnabled(true);
    connect(read_notifier, SIGNAL(activated(int)),
            this, SLOT(select_loop()));

    // error_notifier = new QSocketNotifier(socket,
    //                                      QSocketNotifier::Exception,
    //                                      this);

    // connect(error_notifier, SIGNAL(activated(int)),
    //         this, SLOT(networkError(int )));
    // error_notifier->setEnabled(true);

    emit connected();
}

void QSSHSession::host_disconnect() {
    if (read_notifier)
        read_notifier->setEnabled(false);
    if (ssh_channel_is_open(channel))
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
    }
    if (ssh_is_connected(session)) {
        ssh_disconnect(session);
        ssh_free(session);
    }
    //ssh_finalize();
    qCDebug(qsshsession) << _host << "SSH session is closing ...";
    emit disconnected();
}

void QSSHSession::connect_to() {
    qCDebug(qsshsession) << _host << "ssh is initializing";
    ssh_threads_set_callbacks(ssh_threads_get_pthread());
    ssh_init();
    qCDebug(qsshsession) << _host << "Connecting to host ...";
    session = ssh_new();
    int verbosity = 1;
    ssh_options_set(session, SSH_OPTIONS_HOST, _host.toLocal8Bit());
    ssh_options_set(session, SSH_OPTIONS_PORT, _port.toLocal8Bit());
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    // ssh_options_set(session, SSH_OPTIONS_PUBLICKEY_ACCEPTED_TYPES, "+ssh-dss,rsa-sha2-256,rsa-sha2-512,ecdh-sha2-nistp256,ssh-rsa");
    // ssh_options_set(session, SSH_OPTIONS_HOSTKEYS, "+ssh-dss,ssh-rsa,ecdh-sha2-nistp256,ssh-ed25519,ecdsa-sha2-nistp521,ecdsa-sha2-nistp384,ecdsa-sha2-nistp256,rsa-sha2-512,rsa-sha2-256,ssh-rsa,ssh-dss");
    // ssh_options_set(session, SSH_OPTIONS_KEY_EXCHANGE, "diffie-hellman-group1-sha1,diffie-hellman-group-exchange-sha1,diffie-hellman-group14-sha1,curve25519-sha256,curve25519-sha256@libssh.org,ecdh-sha2-nistp256,ecdh-sha2-nistp384,ecdh-sha2-nistp521,diffie-hellman-group18-sha512,diffie-hellman-group16-sha512,diffie-hellman-group-exchange-sha256,diffie-hellman-group14-sha256,diffie-hellman-group14-sha1,diffie-hellman-group1-sha1");
    int rc = ssh_connect(session);

    if (rc != SSH_OK) {
        qCDebug(qsshsession) << _host << "Connection to host failed!";
        host_disconnect();
        return;
    }

    char * banner=ssh_get_issue_banner(session);
    if(banner) {
        qCDebug(qsshsession)<<banner;
        free(banner);
    }

    authenticate_console(session);
    shell(session);
}

void QSSHSession::sendCommand(QString str)
{
    sendData(QString(str+"\n").toLocal8Bit());
}

void QSSHSession::sendData(const QByteArray &data)
{
    int num = ssh_channel_write(this->channel, data, data.size());
    if (num != data.size()) qCDebug(qsshsession) << _host << "Error ssh_channel_write" << num << data.size();
}
