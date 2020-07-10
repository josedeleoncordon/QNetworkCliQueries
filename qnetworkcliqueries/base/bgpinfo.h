#ifndef BGPINFO_H
#define BGPINFO_H

#include <QtDBus/QDBusArgument>
#include "funcionbase.h"
#include "macinfo.h"

struct SBGPNeighbor : InfoBase
{
    QString neighborip;
    QString as;
    QString upDownTime;
    QString prfxRcd;
};

struct SBGPNetwork : InfoBase
{
    QString neighborip;
    QString network;
    QString RD;
    QString nexthop;
    QString from;
    QString path;

    SBGPNetwork() {}
    SBGPNetwork(const SBGPNetwork &other);
};

QDataStream& operator<<(QDataStream& out, const SBGPNetwork &data);
QDataStream& operator>>(QDataStream& in, SBGPNetwork& data);

QDataStream& operator<<(QDataStream& out, const SBGPNeighbor &data);
QDataStream& operator>>(QDataStream& in, SBGPNeighbor& data);

QDBusArgument& operator<<(QDBusArgument &argument, const SBGPNetwork &data);
const QDBusArgument& operator>>(const QDBusArgument& argument, SBGPNetwork &data);

class QNETWORKCLIQUERIES_EXPORT BGPInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QStringList m_neighborIPs;
    QString m_type;
    QString m_neighbor_int_out;
    QString m_community;
    QList<SBGPNeighbor> m_lstNeigbors;
    QString m_vrf;
    QList<SBGPNetwork> m_lstNetworks;

    int m_neighborsPos;
    QString m_currentNeighbor;

    void networksNextNeighbor();

public:
    BGPInfo() {}
    BGPInfo(QRemoteShell *terminal, QueryOpcion option=QueryOpcion::Null);
    BGPInfo(const BGPInfo &other);

    virtual void getBGPNeighbors();
    virtual void getNetworks();

    //
    QList<SBGPNeighbor>& bgpNeighborInfo() { return m_lstNeigbors; }
    QList<SBGPNetwork>& bgpNetworksInfo() { return m_lstNetworks; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const BGPInfo& info);
    friend QDataStream& operator>>(QDataStream& in, BGPInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const BGPInfo* info);
    friend QDataStream& operator>>(QDataStream& in, BGPInfo*& info);
    friend QDebug operator<<(QDebug dbg, const BGPInfo &info);

private slots:
    void on_term_receiveText_BGPNeighbors();
    void on_term_receiveText_networks();
};

Q_DECLARE_METATYPE(SBGPNetwork)

#endif
