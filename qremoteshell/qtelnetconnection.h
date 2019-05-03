#ifndef QTELNETCONNECTION_H
#define QTELNETCONNECTION_H

#include <QTimer>

#include "connection.h"

class QTelnet;

class QREMOTESHELLSHARED_EXPORT QTelnetConnection : public Connection
{
    Q_OBJECT
public:
    QTelnetConnection(QString ip, QString port, QString user, QString pwd, QString platform, QObject *parent = 0);
    virtual ~QTelnetConnection();

    void setCountry( QString country );
    void setGTW( QString gtw );

    void host_connect();
    void host_disconnect();

    void sendData(const QByteArray &data);
    void sendCommand(QString str);

private slots:
    void host_connected();
    void host_disconnected();
    void on_m_connection_dataReceived();
    void on_timer_timeOut();

private:
    QString m_gtw;
    QString m_country;
    bool m_connectToGTW;
    QString _ip;
    QTelnet *m_connection;
    QTimer *timer;
};

#endif // QTELNETCONNECTION_H
