#ifndef FUNCIONBASE_H
#define FUNCIONBASE_H

#include <QObject>
#include <QString>
#include <QDataStream>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QFile>

#include "qnetworkcliqueries_global.h"
#include "qremoteshell.h"
#include "funciones.h"

/*Todas las funciones se basan en esta clase.
 * Esta programado para que al ser usadas por Queries se realicen las consultas y se guarden en un archivo.
 * Al abrir el archivo unicamente serán para consulta, no para volver a consultar.
 * Para una nueva consulta se utilizan nuevas instancias y se fucionan con las abiertas de un archivo.
*/

class Queries;

struct InfoBase
{
    QDateTime datetime;
    bool operativo;
    Queries* queryParent;

    QString id;

    InfoBase() { operativo=true; queryParent=nullptr; }
    InfoBase(const InfoBase &other);
};

class QNETWORKCLIQUERIES_EXPORT QueriesConfigurationValue
{

public:

    QString _key;
    QVariant _value;
    QString _IP;

    QueriesConfigurationValue(QString key, QVariant value, QString IP="*");
    QueriesConfigurationValue(const QueriesConfigurationValue &other);
    QueriesConfigurationValue() {}

    friend QDataStream& operator<<(QDataStream& out, const QueriesConfigurationValue& qcv);
    friend QDataStream& operator>>(QDataStream& in, QueriesConfigurationValue& qcv);
    friend QDebug operator<<(QDebug dbg, const QueriesConfigurationValue &info);
};

class QNETWORKCLIQUERIES_EXPORT QueriesConfiguration
{
private:
    static QueriesConfiguration* m_instance;
    QList<QueriesConfigurationValue> m_lstQueryParameters;
    QList<QueriesConfigurationValue> m_lstConfiguration;

    QVariant m_find(QString parameter, QString IP);

public:
    QueriesConfiguration();
    static QueriesConfiguration *instance();

    void addQueryParameter(const QList<QueriesConfigurationValue>&);
    void addConfiguration(const QList<QueriesConfigurationValue>&);

    bool state(QString paramete, QString IP);
    QString value(QString parameter, QString IP);
    QStringList values(QString parameter, QString IP);

    QString configuration(QString platform, QString os, QString IP);

    QList<QueriesConfigurationValue> lstQueryParameters() { return m_lstQueryParameters; }
};

class QNETWORKCLIQUERIES_EXPORT FuncionBase : public QObject
{
    Q_OBJECT
protected:
    QRemoteShell *term;
    QString txt;
    QRegExp exp, exp2;
    QString m_brand;
    QString m_platform;
    QString m_os;
    QString m_xr_location;
    QString m_location;
    QString m_name;
    QString m_ip;
    Queries* m_queriesParent;

    QString m_lastCommand;

    void termSendText(QString str);
    void termSendTextSync(QString str);
    void finished();
    bool allTextReceived();
    bool lastCommandFailed;

public:
    FuncionBase(QRemoteShell *terminal, QObject *parent = nullptr);
    ~FuncionBase();

    void setBrand(QString brand) { m_brand = brand; }
    void setPlatform(QString platform);
    void setXRLocation(QString location) { m_xr_location=location; }
    void setHostName(QString name) { m_name=name; }
    void setIp(QString ip) { m_ip = ip; }

private slots:
    void m_on_term_receiveText();

signals:
    void lastCommand(QString);
    void processFinished();
    void working();

    void m_termReceivedFinished();
};

Q_DECLARE_METATYPE(FuncionBase*);

#endif
