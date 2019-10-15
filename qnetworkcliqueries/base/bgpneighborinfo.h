#ifndef BGPNEIGHBORINFO_H
#define BGPNEIGHBORINFO_H

#include "funcionbase.h"
#include "macinfo.h"

struct SBGPNetwork
{
    QString network;
    QString RD;
    QString nexthop;
    QString path;

    SBGPNetwork() {}
    SBGPNetwork(const SBGPNetwork &other);
};

QDataStream& operator<<(QDataStream& out, const SBGPNetwork* data);
QDataStream& operator>>(QDataStream& in, SBGPNetwork*& data);

class QNETWORKCLIQUERIES_EXPORT BGPNeighborInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QString m_type;
    QString m_vrf;
    QString m_community;
    QStringList m_lstIPs;
    QList<SBGPNetwork*> m_lstNetworks;

public:
    BGPNeighborInfo(QRemoteShell *terminal, QObject *parent=0);
    BGPNeighborInfo(const BGPNeighborInfo &other);

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
    friend QDataStream& operator<<(QDataStream& out, const BGPNeighborInfo* info);
    friend QDataStream& operator>>(QDataStream& in, BGPNeighborInfo*& info);
    friend QDebug operator<<(QDebug dbg, const BGPNeighborInfo &info);

private slots:
    void on_term_receiveText_BGPNeighbors();
    void on_term_receiveText_networks();
};

#endif
