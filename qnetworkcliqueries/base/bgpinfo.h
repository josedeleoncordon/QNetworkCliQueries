#ifndef BGPINFO_H
#define BGPINFO_H

#include "funcionbase.h"
#include "macinfo.h"

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

QDataStream& operator<<(QDataStream& out, const SBGPNetwork* data);
QDataStream& operator>>(QDataStream& in, SBGPNetwork*& data);

class QNETWORKCLIQUERIES_EXPORT BGPInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QStringList m_neighborIPs;
    QString m_type;
    QString m_vrf;
    QString m_neighbor_int_out;
    QString m_community;
    QStringList m_lstIPs;
    QList<SBGPNetwork*> m_lstNetworks;

    int m_neighborsPos;
    QString m_currentNeighbor;

    void networksNextNeighbor();

public:
    BGPInfo(QRemoteShell *terminal, QObject *parent=0);
    BGPInfo(const BGPInfo &other);

    enum Type
    {
        IPV4,
        VPNV4
    };    
    virtual void getBGPNeighbors();
    virtual void getNetworks();

    //
    QStringList& bgpNeighborInfo() { return m_lstIPs; }
    QList<SBGPNetwork*>& bgpNetworksInfo() { return m_lstNetworks; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const BGPInfo* info);
    friend QDataStream& operator>>(QDataStream& in, BGPInfo*& info);
    friend QDebug operator<<(QDebug dbg, const BGPInfo &info);

private slots:
    void on_term_receiveText_BGPNeighbors();
    void on_term_receiveText_networks();
};

#endif
