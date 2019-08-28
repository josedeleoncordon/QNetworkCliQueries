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

QDataStream& operator<<(QDataStream& out, const SIpRouteInfo* data);
QDataStream& operator>>(QDataStream& in, SIpRouteInfo*& data);

void updateInfoList(QList<SIpRouteInfo*> &lstDest, QList<SIpRouteInfo*> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT IPRouteInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SIpRouteInfo*> m_lstRoutes;
    QString m_protocol;
    int m_vrfsPos;
    QStringList m_vrfs;
    QString m_vrf;

    void m_siguienteVRF();

public:
    IPRouteInfo(QRemoteShell *terminal, QObject *parent=0);
    IPRouteInfo(const IPRouteInfo &other);
    ~IPRouteInfo();

    virtual void getIPRouteInfo();

    //
    QList<SIpRouteInfo*>& ipRouteInfo() { return m_lstRoutes; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const IPRouteInfo* info);
    friend QDataStream& operator>>(QDataStream& in, IPRouteInfo*& info);
    friend QDebug operator<<(QDebug dbg, const IPRouteInfo &info);

private slots:
    void on_term_receiveText();
};

#endif // IPROUTEINFO_H
