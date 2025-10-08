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
}

int QSSHSession::authenticate_console(ssh_session session)
{
    // typedef int (*ssh_userauth_password_t)(ssh_session session,const char *username,const char *password);

    // ssh_userauth_password_t ssh_userauth_password = (ssh_userauth_password_t) dlsym(handle,"ssh_userauth_password");

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
    // typedef int (*ssh_channel_is_eof_t)(ssh_channel channel);
    // typedef int (*ssh_channel_read_nonblocking_t)(ssh_channel channel, void *dest, uint32_t count,
    //                                  int is_stderr);
    // ssh_channel_is_eof_t ssh_channel_is_eof = (ssh_channel_is_eof_t) dlsym(handle,"ssh_channel_is_eof");
    // ssh_channel_read_nonblocking_t ssh_channel_read_nonblocking =
    //     (ssh_channel_read_nonblocking_t) dlsym(handle,"ssh_channel_read_nonblocking");

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
    // typedef ssh_channel (*ssh_channel_new_t)(ssh_session session);
    // typedef int (*ssh_channel_open_session_t)(ssh_channel channel);
    // typedef int (*ssh_channel_request_pty_t)(ssh_channel channel);
    // typedef int (*ssh_channel_request_shell_t)(ssh_channel channel);
    // typedef int (*ssh_channel_change_pty_size_t)(ssh_channel channel,int cols,int rows);
    // typedef socket_t (*ssh_get_fd_t)(ssh_session session);

    // ssh_channel_new_t ssh_channel_new = (ssh_channel_new_t) dlsym(handle,"ssh_channel_new");
    // ssh_channel_open_session_t ssh_channel_open_session = (ssh_channel_open_session_t) dlsym(handle,
    //                                                                                          "ssh_channel_open_session");
    // ssh_channel_request_pty_t ssh_channel_request_pty = (ssh_channel_request_pty_t) dlsym(handle,
    //                                                                                       "ssh_channel_request_pty");
    // ssh_channel_request_shell_t ssh_channel_request_shell = (ssh_channel_request_shell_t) dlsym(handle,
    //                                                                                             "ssh_channel_request_shell");
    // ssh_channel_change_pty_size_t ssh_channel_change_pty_size =
    //     (ssh_channel_change_pty_size_t) dlsym(handle,"ssh_channel_change_pty_size");
    // ssh_get_fd_t ssh_get_fd = (ssh_get_fd_t) dlsym(handle,"ssh_get_fd");

    this->channel = ssh_channel_new(session);
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

    // error_notifier = new QSocketNotifier(socket,
    //                                      QSocketNotifier::Exception,
    //                                      this);

    // connect(error_notifier, SIGNAL(activated(int)),
    //         this, SLOT(networkError(int )));
    // error_notifier->setEnabled(true);

    emit connected();
}

void QSSHSession::host_disconnect()
{
    // typedef int (*ssh_channel_close_t)(ssh_channel channel);
    // typedef void (*ssh_channel_free_t)(ssh_channel channel);
    // typedef int (*ssh_is_connected_t)(ssh_session session);
    // typedef void (*ssh_disconnect_t)(ssh_session session);
    // typedef void (*ssh_free_t)(ssh_session session);

    // ssh_channel_close_t ssh_channel_close = (ssh_channel_close_t) dlsym(handle,"ssh_channel_close");
    // ssh_channel_free_t ssh_channel_free = (ssh_channel_free_t) dlsym(handle,"ssh_channel_free");
    // ssh_is_connected_t ssh_is_connected = (ssh_is_connected_t) dlsym(handle,"ssh_is_connected");
    // ssh_disconnect_t ssh_disconnect = (ssh_disconnect_t) dlsym(handle,"ssh_disconnect");
    // ssh_free_t ssh_free = (ssh_free_t) dlsym(handle,"ssh_free");

    if ( _host_disconnected ) return;

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
    _host_disconnected=true;
    qCDebug(qsshsession) << _host << "SSH session is closing ...";
    emit disconnected();
}

void QSSHSession::connect_to()
{
    // //Para ssh-dss se asume libssh (ej libssh-0.10.6) fue compilado con -DWITH_DSA=ON
    // //Para compilar libssh con soporte de DSA. Servidores antiguos que requieren ssh-dss
    // //
    // //cmake -DCMAKE_BUILD_TYPE=release -DWITH_EXAMPLES=OFF -DWITH_DSA=ON -DCMAKE_INSTALL_PREFIX=/usr/local ..
    // //make
    // //make install
    // //
    // //Se busca primero en /usr/local/lib64 las lib para que libssh con soporte sea encontrada antes que la oficial
    // //
    // //QMAKE_RPATHDIR += /usr/local/lib64/
    // //QMAKE_RPATHDIR += \$\$ORIGIN
    // //APP_RPATH = $$join(QMAKE_RPATHDIR, ":")
    // //
    // //QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${APP_RPATH}\'

    // //*****************************************************************************************************
    // //Abrimos una version de libssh que no estará enlazada a las librerias y ejecutables.
    // //Qt6 modulo network enlaza a libcurl y libssh en sus ultimas version
    // //abrimos libssh en tiempo de ejecucion para utilizar una version baja vieja, que fue compilada con soporte para DSA
    // qDebug() << "CONCTAR A LIB";
    // handle = dlmopen(LM_ID_NEWLM,"/usr/local/lib64/libssh.so.4.9.6", RTLD_LAZY | RTLD_NODELETE);
    // if (!handle) {
    //     qDebug() << "Cannot open library: " << dlerror();
    //     host_disconnect();
    //     return;
    // }

    // // load the symbol
    // qDebug() << "Loading symbol hello...\n";
    // typedef ssh_threads_callbacks_struct* (*ssh_threads_get_pthread_t)();
    // typedef void (*ssh_threads_set_callbacks_t)(ssh_threads_callbacks_struct*);
    // typedef void (*ssh_init_t)();
    // typedef ssh_session (*ssh_new_t)();
    // typedef int (*ssh_options_set_t)(ssh_session session, enum ssh_options_e type,const void *value);
    // typedef int (*ssh_options_parse_config_t)(ssh_session session, const char *filename);
    // typedef int (*ssh_connect_t)(ssh_session session);
    // typedef char *(*ssh_get_issue_banner_t)(ssh_session session);
    // typedef const char *(*ssh_version_t)(int req_version);

    // // reset errors
    // dlerror();

    // ssh_threads_get_pthread_t ssh_threads_get_pthread = (ssh_threads_get_pthread_t) dlsym(handle,
    //                                                                                       "ssh_threads_get_pthread");
    // ssh_threads_set_callbacks_t ssh_threads_set_callbacks = (ssh_threads_set_callbacks_t) dlsym(handle,
    //                                                                                       "ssh_threads_set_callbacks");
    // ssh_init_t ssh_init = (ssh_init_t) dlsym(handle,"ssh_init");
    // ssh_new_t ssh_new = (ssh_new_t) dlsym(handle,"ssh_new");
    // ssh_options_set_t ssh_options_set = (ssh_options_set_t) dlsym(handle,"ssh_options_set");
    // ssh_options_parse_config_t ssh_options_parse_config = (ssh_options_parse_config_t) dlsym(handle,
    //                                                                                          "ssh_options_parse_config");
    // // ssh_connect_t ssh_connect = (ssh_connect_t) dlsym(handle,"ssh_connect");
    // ssh_get_issue_banner_t ssh_get_issue_banner = (ssh_get_issue_banner_t) dlsym(handle,"ssh_get_issue_banner");
    // ssh_version_t ssh_version = (ssh_version_t) dlsym(handle,"ssh_version");

    // ssh_get_error = (ssh_get_error_t) dlsym(handle,"ssh_get_error");
    // ssh_channel_is_open = (ssh_channel_is_open_t) dlsym(handle,"ssh_channel_is_open");

    // const char *dlsym_error = dlerror();
    // if (dlsym_error) {
    //     qCDebug(qsshsession) << "Error al abrir libssh legacy" << _host << dlsym_error;
    //     host_disconnect();
    //     dlclose(handle);
    //     return;
    // }
    // //*************

    qCDebug(qsshsession) << _host << "ssh is initializing" << ssh_version(0);
    ssh_threads_set_callbacks(ssh_threads_get_pthread());
    ssh_init();
    qCDebug(qsshsession) << _host << "Connecting to host ...";
    session = ssh_new();

    int verbosity = 1;
    // ssh_options_set(session, SSH_OPTIONS_SSH_DIR, "/home/jideleon/.ssh/");
    // ssh_options_set(session, SSH_OPTIONS_USER, "jdeleonc");
    // ssh_options_set(session, SSH_OPTIONS_PASSWORD_AUTH, "JoC!sRael+11");
    ssh_options_set(session, SSH_OPTIONS_HOST, _host.toLocal8Bit());
    ssh_options_set(session, SSH_OPTIONS_PORT, _port.toLocal8Bit());
    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);

    ssh_options_parse_config(session, "~/.ssh/config_legacy"); //con opciones ssh-dss:
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
        qCDebug(qsshsession)<<banner;
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
    // typedef int (*ssh_channel_write_t)(ssh_channel channel, const void *data, uint32_t len);
    // ssh_channel_write_t ssh_channel_write = (ssh_channel_write_t) dlsym(handle,"ssh_channel_write");

    int num = ssh_channel_write(this->channel, data, data.size());
    if (num != data.size()) qCDebug(qsshsession) << _host << "Error ssh_channel_write" << num << data.size();
}
