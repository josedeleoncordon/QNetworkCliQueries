#ifndef BGPINFO_H
#define BGPINFO_H

#include "funcionbase.h"
#include "macinfo.h"

struct SBGPNeighbor : InfoBase
{
    QString neighborip;
    QString as;
    QString upDownTime;
    QString prfxRcd;
    QString vrf;
    QString addressfamily;
    QString prefixFilterIN; //por si el equipo junto a la informacion de BGP manda el filtro. MickoTik
};

struct SBGPNetwork : InfoBase
{    
    QString neighborip; //cambiar a nexthop
    QString network;
    QString RD;
    QString nexthop;
    QString from;
    QString path;
    QString as;
    QString vrf;
    QString lp;
    QString originator;
    bool best;

    QStringList communityList;

    SBGPNetwork() { best=false; }
    bool operator<(const SBGPNetwork &other) const
    {
        return network < other.network;
    }
};

QDataStream& operator<<(QDataStream& out, const SBGPNetwork &data);
QDataStream& operator>>(QDataStream& in, SBGPNetwork& data);
QDataStream& operator<<(QDataStream& out, const SBGPNeighbor &data);
QDataStream& operator>>(QDataStream& in, SBGPNeighbor& data);

class QNETWORKCLIQUERIES_EXPORT BGPInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QStringList m_neighborIPs;
    QStringList m_neighborIPsVRFs;
    QStringList m_networks;
    QString m_type;
    QStringList m_vrf_vrfs;
    QString m_vrf_currentVRF;
    QString m_neighbor_int_out;
    QString m_community;
    bool m_BGPNetworkAttAddOnlyBest;
    QList<SBGPNeighbor> m_lstNeigbors;
    QString m_vrf;
    QStringList m_vrfs;
    QList<SBGPNetwork> m_lstNetworks;        
    QMap<QString,QList<SBGPNetwork>> m_mapNeighborLstNetworks;

    int m_neighborsPos;
    QString m_currentNeighbor;
    QString m_currentNetworkTxt;
    // SBGPNetwork* m_currentNetwork;

    void networksNextNeighbor();
    void bgpNextNetwork();
    void bgpNextNetworkVRPCommunityList();

public:
    BGPInfo() {}
    BGPInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    BGPInfo(const BGPInfo &other);
    ~BGPInfo();

    virtual void getBGPNeighbors(); //listado summary de los vecinos de BGP
    virtual void getNetworks();  //consulta las redes que se reciben o envian a un vecino de BGP
    virtual void getNetworksBGPAttr(); //consulta los atributos de BGP de una red

    //
    QList<SBGPNeighbor>& bgpNeighborInfo() { return m_lstNeigbors; }
    QList<SBGPNetwork>& bgpNetworksInfo() { return m_lstNetworks; }
    QMap<QString,QList<SBGPNetwork>>& bgpMapNetworksInfo() { return m_mapNeighborLstNetworks; }

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
    void on_term_receiveText_networksv6();
    void on_term_receiveText_networksAttr();
    void on_term_receiveText_networksAttrVRPCommunityList();

private:
    void getBGPNeighbors_VRF_nextVRF();

    bool _mikrotiksecondquery;
};

Q_DECLARE_METATYPE(SBGPNetwork)

#endif
