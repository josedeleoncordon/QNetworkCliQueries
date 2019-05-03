#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QDebug>
#include <QRegExp>
#include <QFile>

#include "qremoteshell_global.h"

class QREMOTESHELLSHARED_EXPORT Connection : public QObject
{
    Q_OBJECT

protected:
    QString m_ip;
    QString m_port;
    QString m_user;
    QString m_pwd;
    QString m_host;
    QString m_platform;
    QString m_brand;
    QRegExp exp;

    bool m_termle;    
    bool m_connected;

    //debug
    QString m_logPath;
    QFile m_logFile;
    QTextStream m_out;
    void saveLog(QString);

    QString m_dataReceived;

public:
    explicit Connection(QString ip, QString port, QString user, QString pwd, QString platform, QObject *parent=0);
    virtual ~Connection();

    virtual void host_connect() = 0;
    virtual void host_disconnect() = 0;
    virtual void sendData(const QByteArray &data) = 0;
    virtual void sendCommand(QString str) = 0;

    QString hostName() { return m_host; }
    QString brandName() { return m_brand; }
    QString platform() { return m_platform; }
    QString& dataReceived() { return m_dataReceived; }

    void setLogPath(QString path);

signals:
    void reachable();
    void connected();
    void disconnected();
    void readyRead();
};

#endif // CONNECTION_H
