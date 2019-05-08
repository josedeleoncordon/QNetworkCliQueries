#ifndef SNMPGET_H
#define SNMPGET_H

#include <QObject>

class Get;

class SnmpGet : public QObject
{
    Q_OBJECT
private:
    QString IP;
    QStringList community;
    QString objectId;
    Get *get;
    QStringList m_response;
    int pos;

    void siguienteComm();
public:
    explicit SnmpGet(QString IP,QStringList community, QString objectId, QObject *parent = nullptr);
    SnmpGet(const SnmpGet &other);
    ~SnmpGet();
    void execute();
    QStringList response() { return m_response; }

signals:
    void finished();

public slots:
    void on_get_noResponseReceived();
    void on_get_responseReceived();
};

#endif // SNMPGET_H
