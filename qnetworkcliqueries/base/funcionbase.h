#ifndef FUNCIONBASE_H
#define FUNCIONBASE_H

#include <QObject>
#include <QString>
#include <QDataStream>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QFile>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

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
    QString equipo;
    QDateTime datetime;
    bool operativo;

    QString id;
};

//En toda la aplicacion se utilizara como int por el problema de conversion en QDataStream
enum QueryOpcion {
    Null,
    Connect,
    Platform,
    EquipmentNeighbors,
    MacAddress,
    InterfaceInformation,
    InterfacePermitedVlans,
    InterfaceDescription,
    InterfaceIpAddresses,
    Ospf,
    MplsTEtunnels,
    MplsLdpDiscovery,
    MplsLdpNeighbors,
    MplsLdpInterfaces,
    PimInterfaces,
    PortChannel,
    VRFfVlans,
    VRFfRT,
    VRFs,
    Arp,
    BGPNeig,
    IpRoutes,
    Configuration,
    Mplsl2Transport,
    Funcion,
    BGPNetworks,
    BGPNetworksBGPAttr,
    RplRoutes,
    RplPrefixes,
    RplCommunities,
    Exit
};

class QNETWORKCLIQUERIES_EXPORT QueriesConfigurationValue
{

public:

    QString _key;
    QString _value;
    QString _IP;
    QString _Platform;
    QString _IDConexion;
    bool _appendValue;

    QueriesConfigurationValue(QString key,
                              QString value,
                              QString IP,
                              QString plataforma,
                              QString IDConexion,
                              bool appendValue=true);
    QueriesConfigurationValue(const QueriesConfigurationValue &other);
    QueriesConfigurationValue() {}
    QueriesConfigurationValue& operator =(const QueriesConfigurationValue &other);
    void clone(const QueriesConfigurationValue &other);

    friend QDataStream& operator<<(QDataStream& out, const QueriesConfigurationValue& qcv);
    friend QDataStream& operator>>(QDataStream& in, QueriesConfigurationValue& qcv);
    friend QDebug operator<<(QDebug dbg, const QueriesConfigurationValue &info);
};

class QNETWORKCLIQUERIES_EXPORT QueriesConfiguration
{
private:
    QList<QueriesConfigurationValue> m_lstQueryParameters;
    QList<QueriesConfigurationValue> m_lstConfiguration;

    QVariant m_find(QString parameter, QString IP, QString platform, QString IDconexion,bool platformContains);

public:
    QueriesConfiguration();

    void addQueryParameter(const QList<QueriesConfigurationValue>&);    

    bool state(QString parameter, QString IP, QString platform, QString IDconexion,bool platformContains=false);
    QString value(QString parameter, QString IP, QString platform, QString IDconexion,bool platformContains=false);
    QStringList values(QString parameter, QString IP, QString platform, QString IDconexion,bool platformContains=false);

    QList<QueriesConfigurationValue> lstQueryParameters() { return m_lstQueryParameters; }

    void clear();
};

class QNETWORKCLIQUERIES_EXPORT FuncionBase : public QObject
{
    Q_OBJECT
    friend class Queries;
private:
    void init();

protected:
    QRemoteShell *term;
    QString txt;
    QRegularExpression exp, exp2, exp3, exp4;
    QRegularExpressionMatch match, match2, match3, match4;
    QString m_brand;
    QString m_platform;
    QString m_model;
    QString m_os;
    QString m_xr_location;
    bool m_xr64;
    QString m_location;
    QString m_name;
    QString m_ip;
    Queries* m_parentQuery;
    int m_queryoption;
    QueriesConfiguration m_queriesConfiguration;
    QString m_conexionID;

    QString m_lastCommand;

    void termSendText(QString str);
    void termSendTextSync(QString str);
    void finished();
    bool allTextReceived();
    bool lastCommandFailed;

public:
    FuncionBase();
    FuncionBase(QRemoteShell *terminal, int option=QueryOpcion::Null);
    ~FuncionBase();

    int queryOption() { return m_queryoption; }

    void setBrand(QString brand) { m_brand = brand; }
    void setPlatform(QString platform);
    void setModel(QString model) { m_model = model; }
    void setXR64(bool xr64) { m_xr64=xr64; }
    void setXRLocation(QString location) { m_xr_location=location; }
    void setHostName(QString name) { m_name=name; }
    void setIp(QString ip) { m_ip = ip; }
    void setParentQuery(Queries *qry);
    void setQueryOption(int option) { m_queryoption=option; }

private slots:
    void m_on_term_receiveText();

signals:
    void lastCommand(QString);
    void processFinished();
    void working();

    void m_termReceivedFinished();
};

#endif
