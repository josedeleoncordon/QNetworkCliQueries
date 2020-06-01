#ifndef IPROUTEINFO_H
#define IPROUTEINFO_H

#include "funcionbase.h"

struct SIpRouteInfo : InfoBase
{
    QString network;
    QString mask;
    QString protocol;
    QStringList via;
    QString vrf;

    SIpRouteInfo() {}
    SIpRouteInfo(const SIpRouteInfo &other);
};

QDataStream& operator<<(QDataStream& out, const SIpRouteInfo& data);
QDataStream& operator>>(QDataStream& in, SIpRouteInfo& data);

void updateInfoList(QList<SIpRouteInfo> &lstDest, QList<SIpRouteInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT IPRouteInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SIpRouteInfo> m_lstRoutes;

    QString m_protocol;
    int m_vrfsPos;
    int m_redPos;
    QStringList m_vrfs;
    QString m_vrf;
    QStringList m_redes; //a consultar
    QString m_red;

    void m_generalSiguienteVRF();
    void m_individualSiguienteRed();

public:
    IPRouteInfo() {}
    IPRouteInfo(QRemoteShell *terminal, QObject *parent=0);
    IPRouteInfo(const IPRouteInfo &other);
    ~IPRouteInfo();

    virtual void getIPRouteInfo();

    //
    QList<SIpRouteInfo>& ipRouteInfo() { return m_lstRoutes; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const IPRouteInfo& info);
    friend QDataStream& operator>>(QDataStream& in, IPRouteInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const IPRouteInfo* info);
    friend QDataStream& operator>>(QDataStream& in, IPRouteInfo*& info);
    friend QDebug operator<<(QDebug dbg, const IPRouteInfo &info);

private slots:
    void on_term_receiveText_general(); //consultando toda la tabla
    void on_term_receiveText_individual(); //consultado un prefijo
};

Q_DECLARE_METATYPE(IPRouteInfo)

#endif // IPROUTEINFO_H
