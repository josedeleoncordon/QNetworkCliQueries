#include "qsshsession.h"

#include <QDebug>
#include <QTimer>
#include <QSocketNotifier>
#include <qremoteshelllogging.h>
#include <dlfcn.h>

QSSHSession::QSSHSession(QString host, QString port, QString user, QString pass, QObject *parent )
    : QObject{parent}, session(0), channel(0)
{
    _host = host;
    _port = port;
    _user = user;
    _password = pass;
    m_error=NONE;
    _host_disconnected = false;

    read_notifier = nullptr;
    timer = nullptr;
}

QSSHSession::~QSSHSession()
{
    qCDebug(qsshsession) << _host << "QSSHSession::~QSSHSession()";
    if ( read_notifier )
    {
        qCDebug(qsshsession) << _host << "Del";
        delete read_notifier;
        qCDebug(qsshsession) << _host << "SI";
    }
    if (timer)
        delete timer;
}

int QSSHSession::authenticate_console(ssh_session session)
{
    int rc;

    //Try to authenticate with password
    rc = ssh_userauth_password(session, _user.toLocal8Bit(), _password.toLocal8Bit());
    qDebug() <<  _host << "rc" << rc;
    if (rc == SSH_AUTH_DENIED) {
        m_errortxt = ssh_get_error(session);
        qCDebug(qsshsession) << _host << "Error al autenticar"  << m_errortxt;
        m_error = AUTH_DENIED;
        host_disconnect();
        return rc;
    }
    return rc;
}

void QSSHSession::select_loop()
{
    if (timer)
    {
        qDebug() << "deteniendo timer al recibir text";
        timer->stop();
        delete timer;
        timer = nullptr;
    }

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

void QSSHSession::m_sendfirstrn()
{
    qDebug() << "QSSHSession::m_sendfirstrn() por no recibir texto del equipo";
    sendCommand("");
}

void QSSHSession::shell(ssh_session session)
{
    channel = ssh_channel_new(session);
    // ssh_channel_accept_x11(channel, 10);
    if(ssh_channel_open_session(channel)){
        m_errortxt = ssh_get_error(session);
        qCDebug(qsshsession) << _host << "error opening channel" << ssh_get_error(session) << m_errortxt;
        m_error = CHANNEL_ERROR;
        host_disconnect();
        return;
    }

    ssh_channel_request_pty(channel);
    if(ssh_channel_request_shell(channel)){
        m_errortxt = ssh_get_error(session);
        qCDebug(qsshsession) << _host << "Requesting shell :" << ssh_get_error(session) << m_errortxt;
        m_error = SHELL_ERROR;
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

    //Si el equipo al conectarnos no envia texto, enviamos un simbolo de enter para que nos envie mensajes
    //Ej equipos Nokia
    timer = new QTimer;
    timer->setInterval(1500);
    timer->setSingleShot(true);
    connect(timer,SIGNAL(timeout()),SLOT(m_sendfirstrn()));
    timer->start();

    emit connected();
}

void QSSHSession::host_disconnect()
{
    if ( _host_disconnected ) return;

    if (read_notifier)
        read_notifier->setEnabled(false);
    if (ssh_channel_is_open(channel))
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
    }
    if (ssh_is_connected(session))
    {
        ssh_disconnect(session);
        ssh_free(session);    
    }
    //ssh_finalize();
    _host_disconnected=true;
    qCDebug(qsshsession) << _host << "SSH session is closing ...";
    emit disconnected();
}

void QSSHSession::connect_to()
{
    qDebug() << "QSSHSession::connect_to()";

    //Conexion normal hacia equipo ó
    //primera conexion hacia GW de salto
    qCDebug(qsshsession) << _host << "ssh is initializing" << ssh_version(0);
    ssh_threads_set_callbacks(ssh_threads_get_pthread());
    ssh_init();
    qCDebug(qsshsession) << _host << "Connecting to host ..." << _host << _port;
    session = ssh_new();

    int verbosity = 1;
    // ssh_options_set(session, SSH_OPTIONS_SSH_DIR, "/home/jideleon/.ssh/");
    ssh_options_set(session, SSH_OPTIONS_USER, _user.toLocal8Bit());
    ssh_options_set(session, SSH_OPTIONS_HOST, _host.toLocal8Bit());
    ssh_options_set(session, SSH_OPTIONS_PORT, _port.toLocal8Bit());
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
    // ssh_options_parse_config(session, "~/.ssh/config");
    ssh_options_parse_config(session, "~/.ssh/config_legacy"); //con soporte ssh-dss:

    //Host *
    //        Port 22
    //        StrictHostKeyChecking no
    //        UserKnownHostsFile=/dev/null
    //	Ciphers aes128-ctr,aes192-ctr,aes256-ctr,aes128-cbc,3des-cbc,aes256-cbc
    //        KexAlgorithms +diffie-hellman-group1-sha1,diffie-hellman-group-exchange-sha1,diffie-hellman-group14-sha1,diffie-hellman-group16-sha512,diffie-hellman-group14-sha256,diffie-hellman-group-exchange-sha256,diffie-hellman-group1-sha1,diffie-hellman-group-exchange-sha1,diffie-hellman-group14-sha1
    //	HostkeyAlgorithms ssh-dss,ssh-rsa,rsa-sha2-512,rsa-sha2-256,ecdsa-sha2-nistp256,ssh-ed25519
    //	PubkeyAcceptedKeyTypes +ssh-dss,ssh-rsa

    // ssh_options_set(session, SSH_OPTIONS_PUBLICKEY_ACCEPTED_TYPES, "ssh-dss,ssh-rsa");
    // ssh_options_set(session, SSH_OPTIONS_HOSTKEYS, "ssh-dss,ssh-rsa,rsa-sha2-512,rsa-sha2-256,ecdsa-sha2-nistp256,ssh-ed25519");
    // ssh_options_set(session, SSH_OPTIONS_KEY_EXCHANGE, "+diffie-hellman-group1-sha1,diffie-hellman-group-exchange-sha1,diffie-hellman-group14-sha1,diffie-hellman-group16-sha512,diffie-hellman-group14-sha256,diffie-hellman-group-exchange-sha256,diffie-hellman-group1-sha1,diffie-hellman-group-exchange-sha1,diffie-hellman-group14-sha1");
    // ssh_options_set(session, SSH_OPTIONS_CIPHERS_C_S, "aes128-ctr,aes192-ctr,aes256-ctr,aes128-cbc,3des-cbc,aes256-cbc");
    // ssh_options_set(session, SSH_OPTIONS_HMAC_C_S, "hmac-sha2-256-etm@openssh.com,hmac-sha1-etm@openssh.com,umac-128-etm@openssh.com,hmac-sha2-512-etm@openssh.com,hmac-sha2-256,hmac-sha1,umac-128@openssh.com,hmac-sha2-512,hmac-sha1-96");

    qDebug() << "1111";

    int rc = ssh_connect(session);

    if (rc != SSH_OK) {
        m_errortxt = ssh_get_error(session);
        m_error = OPTIONS_ERROR;
        qCDebug(qsshsession) << "Connection to host failed!" << _host << rc << ssh_get_error(session) << ssh_version(0);
        host_disconnect();
        return;
    }

    char * banner=ssh_get_issue_banner(session);
    if(banner) {
        qCDebug(qsshsession) << "banner" << banner;
        free(banner);
    }

    authenticate_console(session); // <-- si falla autentiacacion este manda a finalizar y emitir señal
    if ( m_error == NONE )
        shell(session);
}

void QSSHSession::sendCommand(QString str)
{
    sendData(QString(str+"\n").toLocal8Bit());
}

void QSSHSession::sendData(const QByteArray &data)
{
    int num = ssh_channel_write(this->channel, data, data.size());
    if (num != data.size()) qCDebug(qsshsession) << _host << "Error ssh_channel_write" << num << data.size() << data;
}
