#include "bgpinfo.h"
#include "queries.h"

SBGPNetwork::SBGPNetwork(const SBGPNetwork &other)
{
    neighborip = other.neighborip;
    network = other.network;
    RD = other.RD;
    nexthop = other.nexthop;
    from = other.from;
    path = other.path;
    communityList = other.communityList;
    //infobase
    datetime = other.datetime;
    operativo = other.operativo;
    equipo = other.equipo;
    as = other.as;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const SBGPNetwork& data)
{
    out << data.neighborip;
    out << data.network;
    out << data.RD;
    out << data.nexthop;
    out << data.from;
    out << data.path;
    out << data.communityList;
    //infobase
    out << data.datetime;
    out << data.operativo;
    out << data.equipo;
    out << data.as;
    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, SBGPNetwork& data)
{
    in >> data.neighborip;
    in >> data.network;
    in >> data.RD;
    in >> data.nexthop;
    in >> data.from;
    in >> data.path;
    in >> data.communityList;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    in >> data.equipo;
    in >> data.as;
    return in;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const SBGPNeighbor &data)
{
    out << data.neighborip;
    out << data.as;
    out << data.upDownTime;
    out << data.prfxRcd;
    out << data.vrf;
    out << data.addressfamily;
    //infobase
    out << data.datetime;
    out << data.operativo;
    out << data.equipo;    
    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, SBGPNeighbor& data)
{
    in >> data.neighborip;
    in >> data.as;
    in >> data.upDownTime;
    in >> data.prfxRcd;
    in >> data.vrf;
    in >> data.addressfamily;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    in >> data.equipo;
    return in;
}

QNETWORKCLIQUERIES_EXPORT QDBusArgument& operator<<(QDBusArgument &argument, const SBGPNetwork &data)
{
    argument.beginStructure();
    argument << data.neighborip;
    argument << data.network;
    argument << data.RD;
    argument << data.nexthop;
    argument << data.from;
    argument << data.path;
    argument << data.communityList;
    argument << data.datetime;
    argument << data.operativo;
    argument << data.equipo;
    argument << data.as;
    argument.endStructure();
    return argument;
}

QNETWORKCLIQUERIES_EXPORT const QDBusArgument& operator>>(const QDBusArgument& argument, SBGPNetwork &data)
{
    argument.beginStructure();
    argument >> data.neighborip;
    argument >> data.network;
    argument >> data.RD;
    argument >> data.nexthop;
    argument >> data.from;
    argument >> data.path;
    argument >> data.communityList;
    argument >> data.datetime;
    argument >> data.operativo;
    argument >> data.equipo;
    argument >> data.as;
    argument.endStructure();
    return argument;
}

//falta DBUS para SBGPNeighbor

BGPInfo::BGPInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
    m_currentNetwork=nullptr;
}

BGPInfo::BGPInfo(const BGPInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_currentNetwork=nullptr;
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_lstNeigbors= other.m_lstNeigbors;
    m_lstNetworks = other.m_lstNetworks;
    m_mapNeighborLstNetworks = other.m_mapNeighborLstNetworks;
    m_type = other.m_type;    
}

BGPInfo::~BGPInfo()
{}

void BGPInfo::getBGPNeighbors()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "BGPInfo::getBGPNeighbors():" << m_brand << "no soportado";
        finished();
        return;
    }    
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_BGPNeighbors()));

    m_type = m_queriesConfiguration.value("BGPNeig_Type",m_ip,m_os,m_conexionID);
    m_vrf_vrfs = m_queriesConfiguration.values("BGPNeig_VRF_VRFs",m_ip,m_os,m_conexionID);

    if ( m_type.isEmpty() )
        m_type = "IPV4";

    if ( m_type == "IPV4" )
    {
        if ( m_os == "IOS XR" )
            termSendText("show bgp summary");
        else
            termSendText("sh bgp ipv4 unicast summary");
    }
    else if ( m_type == "IPV4LU" )
    {
        if ( m_os == "IOS XR" )
            termSendText("show bgp ipv4 labeled-unicast summary");
        else
            termSendText("sh bgp ipv4 unicast summary");
    }
    else if ( m_type == "VPNV4" )
    {
        if ( m_os == "IOS XR" )
            termSendText("sh bgp vpnv4 unicast summary");
        else
            termSendText("sh bgp vpnv4 unicast all summary");
    }
    else if ( m_type == "VRF" )
        getBGPNeighbors_VRF_nextVRF();
    else
        finished();
}

void BGPInfo::getBGPNeighbors_VRF_nextVRF()
{
    if ( !m_vrf_vrfs.isEmpty() )
    {
        m_vrf_currentVRF = m_vrf_vrfs.takeFirst();
        if ( m_os == "IOS XR" )
            termSendText("sh bgp vrf "+m_vrf_currentVRF+" summary");
        else
            termSendText("sh bgp vpnv4 unicast vrf "+m_vrf_currentVRF+" summary");
    }
    else
        finished();
}

void BGPInfo::on_term_receiveText_BGPNeighbors()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    for (QString line : lines)
    {
        exp.setPattern("^(\\d+\\.\\d+\\.\\d+\\.\\d+).+");
        if ( ! line.contains(exp) )
            continue;

        QStringList data = line.split(" ",QString::SkipEmptyParts);
        if ( data.size() < 10 )
            continue;

        SBGPNeighbor s;
        s.neighborip = data.at(0).simplified();
        s.as = data.at(2).simplified();
        s.upDownTime = data.at(8).simplified();
        s.prfxRcd = data.at(9).simplified();
        s.vrf = m_vrf_currentVRF;
        s.addressfamily = m_type;

        m_lstNeigbors.append(s);
    }

    if ( m_type == "VRF" )
        getBGPNeighbors_VRF_nextVRF();
    else
        finished();
}

void BGPInfo::getNetworks()
{
    m_neighborsPos=-1;
    m_neighborIPs = m_queriesConfiguration.values("BGPNetworks_NeighborIP",m_ip,m_os,m_conexionID);
    m_neighborIPsVRFs = m_queriesConfiguration.values("BGPNetworks_NeighborIPsVRFs",m_ip,m_os,m_conexionID);
    m_vrf = m_queriesConfiguration.value("BGPNetworks_VRF",m_ip,m_os,m_conexionID);
    m_community = m_queriesConfiguration.value("BGPNetworks_Community",m_ip,m_os,m_conexionID);
    m_neighbor_int_out = m_queriesConfiguration.value("BGPNetworks_NeighborIn_Out",m_ip,m_os,m_conexionID);

    qDebug() << "\n BGP getNetworks";
    qDebug() << m_vrfs << m_neighborIPs << m_neighborIPsVRFs;

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networks()));

    if ( m_neighborIPs.isEmpty() && m_neighborIPsVRFs.isEmpty() )
    {        
        if ( m_os == "IOS XR" )
        {
            if ( !m_community.isEmpty() )
                termSendText("sh bgp "+
                             (!m_vrf.isEmpty()?" vrf "+m_vrf:" ")+
                             "community "+m_community);
            else
                termSendText("sh bgp "+
                             (!m_vrf.isEmpty()?" vrf "+m_vrf:" "));
        }
        else if ( m_os == "VRP" )
        {
            if ( !m_community.isEmpty() )
                termSendText("display bgp "+
                             (!m_vrf.isEmpty()?" vpnv4 vpn-instance "+m_vrf:" ")+
                             "routing-table community "+m_community);
            else
                termSendText("display bgp "+
                             (!m_vrf.isEmpty()?" vpnv4 vpn-instance "+m_vrf:" "));
        }
        else
        {
            finished();
            return;
        }
    }
    else
        networksNextNeighbor();
}

void BGPInfo::networksNextNeighbor()
{
    qDebug() << "BGPInfo::networksNextNeighbor()";

    if ( m_neighbor_int_out.isEmpty() )
    {
        finished();
        return;
    }

    if ( !m_neighborIPs.isEmpty() )
    {
        qDebug() << "m_neighborIPs.isEmpty()";
        if ( m_neighborsPos < m_neighborIPs.size()-1 )
        {
            m_currentNeighbor = m_neighborIPs.at( ++m_neighborsPos );

            if ( m_os == "IOS XR" )
            {
                termSendText("sh bgp "+
                             (!m_vrf.isEmpty()?" vrf "+m_vrf:" ")+
                             " neighbor "+m_currentNeighbor+" "+
                             (m_neighbor_int_out=="IN"
                              ?"routes":(m_neighbor_int_out=="OUT"
                                         ?"advertised-routes":"")));
            }
            else if ( m_os == "VRP" )
            {
                termSendText("display bgp "+
                             (!m_vrf.isEmpty()?"vpnv4 vpn-instance "+m_vrf:" ")+
                             " routing-table peer "+m_currentNeighbor+" "+
                             (m_neighbor_int_out=="IN"
                              ?"accepted-routes":(m_neighbor_int_out=="OUT"
                                                  ?"advertised-routes":"")));
            }
            else
            {
                finished();
                return;
            }
        }
        else
            finished();
    }
    else if ( !m_neighborIPsVRFs.isEmpty() )
    {
        qDebug() << "m_neighborIPsVRFs.isEmpty()";
        if ( m_neighborsPos < m_neighborIPsVRFs.size()-1 )
        {
            QString Neighbor_VRF = m_neighborIPsVRFs.at( ++m_neighborsPos );
            if ( Neighbor_VRF.contains("_") )
            {
                QStringList data = Neighbor_VRF.split("_",QString::SkipEmptyParts);
                m_currentNeighbor = data.first();
                m_vrf_currentVRF = data.last();
            }
            else
            {
                m_currentNeighbor = Neighbor_VRF;
                m_vrf_currentVRF.clear();
            }

            if ( m_os == "IOS XR" )
            {
                termSendText("sh bgp "+
                             (!m_vrf_currentVRF.isEmpty()?" vrf "+m_vrf_currentVRF:" ")+
                             " neighbor "+m_currentNeighbor+" "+
                             (m_neighbor_int_out=="IN"
                              ?"routes":(m_neighbor_int_out=="OUT"
                                         ?"advertised-routes":"")));
            }
            else if ( m_os == "VRP" )
            {
                termSendText("display bgp "+
                             (!m_vrf_currentVRF.isEmpty()?"vpnv4 vpn-instance "+m_vrf_currentVRF:" ")+
                             " routing-table peer "+m_currentNeighbor+" "+
                             (m_neighbor_int_out=="IN"
                              ?"accepted-routes":(m_neighbor_int_out=="OUT"
                                                  ?"advertised-routes":"")));
            }
            else
            {
                finished();
                return;
            }
        }
        else
            finished();
    }
    else
        finished();
}

void BGPInfo::on_term_receiveText_networks()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QList<SBGPNetwork> m_lstNeighborNetworks;

    QStringList lines = txt.split("\n");
    int pathi = 0;
    QString network;
    for (QString line : lines)
    {
        //no realizar line.simplified ya que es necesario que este el texto por la posicion del as-path

        if ( m_os == "IOS XR" )
        {
            if ( line.contains("Weight Path") )
                pathi = line.indexOf("Path");
            if ( line.contains("AS Path") )
                pathi = line.indexOf("AS ");
        }
        else if ( m_os == "VRP" )
        {
            if ( line.contains("Path/Ogn") )
                pathi = line.indexOf("Path")+1;
        }

        exp.setPattern("\\d+\\.\\d+\\.\\d+\\.\\d+");
        if ( ! line.contains(exp) || line.contains("routes") )
            continue;

        //Advertised-routes Cisco
        exp.setPattern("(\\d+\\.\\d+\\.\\d+\\.\\d+/\\d+) +(\\d+\\.\\d+\\.\\d+\\.\\d+) +((\\d+\\.\\d+\\.\\d+\\.\\d+|Local)) ");
        if ( line.contains(exp) )
        {
            SBGPNetwork s;
            network = exp.cap(1);
            s.neighborip = m_currentNeighbor;
            s.network = network;
            s.nexthop = exp.cap(2);
            s.from = exp.cap(3);
            s.path = line.mid( pathi ).replace("i","").simplified();
            s.operativo = true;
            s.datetime = QDateTime::currentDateTime();
            s.equipo = m_parentQuery->hostName();

            m_lstNetworks.append(s);
            m_lstNeighborNetworks.append(s);
            continue;
        }

        //Routes Cisco,  Advertised-routes y Routes Huawei
        exp.setPattern("(\\d+\\.\\d+\\.\\d+\\.\\d+/\\d+) +(\\d+\\.\\d+\\.\\d+\\.\\d+) ");
        if ( line.contains(exp) )
        {
            SBGPNetwork s;
            network = exp.cap(1);
            s.neighborip = m_currentNeighbor;
            s.network = network;
            s.nexthop = exp.cap(2);
            s.path = line.mid( pathi ).replace("i","").simplified();
            s.operativo = true;
            s.datetime = QDateTime::currentDateTime();
            s.equipo = m_parentQuery->hostName();

            m_lstNetworks.append(s);
            m_lstNeighborNetworks.append(s);
            continue;
        }

        exp.setPattern(" +(\\d+\\.\\d+\\.\\d+\\.\\d+) ");
        if ( line.contains(exp) && !network.isEmpty() && !m_currentNeighbor.isEmpty())
        {
            SBGPNetwork s;
            s.neighborip = m_currentNeighbor;
            s.network = network;
            s.nexthop = exp.cap(1);
            s.path = line.mid( pathi ).replace("i","").simplified();
            s.operativo = true;
            s.datetime = QDateTime::currentDateTime();
            s.equipo = m_parentQuery->hostName();

            m_lstNetworks.append(s);
            m_lstNeighborNetworks.append(s);
            continue;
        }
    }
    m_mapNeighborLstNetworks.insert( m_currentNeighbor, m_lstNeighborNetworks );

    if ( m_neighborIPs.isEmpty() && m_neighborIPsVRFs.isEmpty() )
        finished();
    else
        networksNextNeighbor();
}

void BGPInfo::getNetworksBGPAttr()
{
    m_networks = m_queriesConfiguration.values("BGPNetworksBGPInfo_Network",m_ip,m_os,m_conexionID);
    m_vrf = m_queriesConfiguration.value("BGPNetworksBGPInfo_VRF",m_ip,m_os,m_conexionID);

    qDebug() << "\n BGP getNetworksBGPInfo";
    qDebug() << m_vrf << m_networks;

    if ( m_networks.isEmpty() )
    {
        finished();
        return;
    }

    bgpNextNetwork();
}

void BGPInfo::bgpNextNetwork()
{
    if ( m_networks.isEmpty() )
    {
        finished();
        return;
    }

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networksAttr()));

    SBGPNetwork network;
    network.network = m_networks.takeFirst();
    QString redhuawei = network.network;
    redhuawei.replace("/"," ");

    m_lstNetworks.append( network );
    m_currentNetwork = &m_lstNetworks.last();

    if ( m_os == "IOS XR" )
    {
        termSendText("sh bgp "+
                     (!m_vrf.isEmpty()?" vrf "+m_vrf+" ":" ")+
                     m_currentNetwork->network);
    }
    else if ( m_os == "VRP" )
    {
        termSendText("display bgp "+
                     (!m_vrf.isEmpty()?" vpnv4 vpn-instance "+m_vrf:" ")+
                     " routing-table "+redhuawei);
    }
    else
    {
        finished();
        return;
    }
}

void BGPInfo::on_term_receiveText_networksAttr()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    term->disconnectReceiveTextSignalConnections();

    QStringList lines = txt.split("\n");

    QString lastNH;
    QString lastASN;
    QString lastFrom;
    bool best = false;
    QStringList lastLstCommunities;
    for (QString line : lines)
    {
        if ( m_os == "IOS XR" )
        {
            //no realizar line.simplified ya que es necesario tener los caracteres de espacio al inicio de la cadena
//            qDebug() << line;

            QRegExp exp("BGP routing table entry for (.+),");
            exp.setMinimal(false);
            if ( line.contains(exp) )
            {
//                qDebug() << "----- lastred" << exp.cap(1);
                continue;
            }

            //AS
            exp.setPattern("^  \\d+");
            if ( line.contains(exp) )
            {
                lastNH.clear();
                lastASN.clear();
                lastFrom.clear();
                best=false;
                lastLstCommunities.clear();

                line = line.simplified();
                exp.setPattern("(\\d| )+");
                if ( line.contains(exp) )
                {
                    lastASN = exp.cap(0);
                    if ( lastASN.contains(" ") )
                        lastASN = lastASN.split(" ",QString::SkipEmptyParts).last();
//                    qDebug() << "----- lastASN" << lastASN;
                }
                continue;
            }
            exp.setPattern("^  Local");
            if ( line.contains(exp) )
            {
                lastNH.clear();
                lastASN.clear();
                lastFrom.clear();
                best=false;
                lastLstCommunities.clear();

                lastASN = "52468";
//                qDebug() << "lastASN" << lastASN;
                continue;
            }

            //Peer IP y From
            exp.setPattern("^    (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}).+from (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) .+");
            if ( line.contains(exp) )
            {
                lastNH = exp.cap(1);
                lastFrom = exp.cap(2);
//                qDebug() << "---- lastNh" << lastNH;
//                qDebug() << "---- lastfrom" << lastFrom;
                continue;
            }

            //best
            if ( line.contains(", best,") )
            {
//                qDebug() << "---- BEST";
                best=true;
                continue;
            }

            //comunidades
            exp.setPattern("^ +Community:");
            if ( line.contains(exp) )
            {
                line.replace("Community:","");
                lastLstCommunities=line.split(" ",QString::SkipEmptyParts);

                if ( best )
                {
                    m_currentNetwork->neighborip = lastNH;
                    m_currentNetwork->as = lastASN;
                    m_currentNetwork->from = lastFrom;
                    m_currentNetwork->communityList = lastLstCommunities;
                }

                lastNH.clear();
                lastASN.clear();
                lastFrom.clear();
                best=false;
                lastLstCommunities.clear();
            }
        }
        else if ( m_os == "VRP" )
        {
            line = line.simplified();
//            qDebug() << "on_term_receiveText_networksAttr()" << line;

            QRegExp exp("BGP routing table entry information of (.+),");
            exp.setMinimal(false);
            if ( line.contains(exp) )
            {
//                qDebug() << "----- lastred" << exp.cap(1);
                lastNH.clear();
                lastASN.clear();
                lastFrom.clear();
                continue;
            }

            //From
            exp.setPattern("^From: (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})");
            if ( line.contains(exp) )
            {
                lastFrom = exp.cap(1);
//                qDebug() << "---- lastfrom" << lastFrom;
                continue;
            }

            //IP nexthop
            exp.setPattern("^Original nexthop: (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})");
            if ( line.contains(exp) )
            {
                lastNH = exp.cap(1);
//                qDebug() << "---- lastNH" << lastNH;
                continue;
            }

            //AS y best
            exp.setPattern("^AS-path ((\\w| )+),");
            if ( line.contains(exp) )
            {
                QString AP = exp.cap(1);

                exp.setPattern("(\\d| )+");
                if ( AP.contains(exp) )
                {
                    lastASN = AP;
                    if ( lastASN.contains(" ") )
                        lastASN = lastASN.split(" ",QString::SkipEmptyParts).last();
//                    qDebug() << "----- lastASN" << lastASN;
                }
                else if ( AP.contains("Nil") )
                {
                    lastASN = "52468";
//                    qDebug() << "lastASN" << lastASN;
                }

                if ( line.contains(" best,") )
                {
//                    qDebug() << "---- BEST";

                    m_currentNetwork->neighborip = lastNH;
                    m_currentNetwork->as = lastASN;
                    m_currentNetwork->from = lastFrom;

                    lastNH.clear();
                    lastASN.clear();
                    lastFrom.clear();
                }

                break;
            }
        }
    }

    if ( m_os == "IOS XR" )
        bgpNextNetwork();
    else if ( m_os == "VRP" )
    {
        QString redhuawei = m_currentNetwork->network;
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networksAttrVRPCommunityList()));
        termSendText("display bgp "+
                     (!m_vrf.isEmpty()?" vpnv4 vpn-instance "+m_vrf:" ")+
                     " routing-table "+redhuawei.replace("/"," ")+" community-list");
    }
    else
        finished();
}

void BGPInfo::on_term_receiveText_networksAttrVRPCommunityList()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    term->disconnectReceiveTextSignalConnections();

    QStringList lines = txt.split("\n");

    qDebug() << "---- on_term_receiveText_networksAttrVRPCommunityList. m_currentNetwork->from" << m_currentNetwork->from;

    QString lastfrom;
    for (QString line : lines)
    {
        line = line.simplified();

        qDebug() << line;

        QRegExp exp("From: (.+)$");
        exp.setMinimal(false);
        if ( line.contains(exp) )
        {
            lastfrom = exp.cap(1);
            qDebug() << "---- lastFrom" << lastfrom;
            continue;
        }

        exp.setPattern("Community: (.+)$");
        if ( line.contains( exp ) )
        {
            qDebug() << "---- Community: comparacion from" << lastfrom << m_currentNetwork->from;

            if ( lastfrom == m_currentNetwork->from )
            {
                QString comunidades = exp.cap(1);
                comunidades.replace("<","");
                comunidades.replace(">","");
                comunidades.replace(",","");
                m_currentNetwork->communityList = comunidades.split(" ",QString::SkipEmptyParts);
                qDebug() << "---- Agregando comunidades";
                break;
            }
        }
    }

    bgpNextNetwork();
}

QDataStream& operator<<(QDataStream& out, const BGPInfo& info)
{
    out << info.m_lstNeigbors;
    out << info.m_lstNetworks;
    out << info.m_mapNeighborLstNetworks;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, BGPInfo& info)
{
    in >> info.m_lstNeigbors;
    in >> info.m_lstNetworks;
    in >> info.m_mapNeighborLstNetworks;
    in >> info.m_queryoption;
    return in;
}

QDataStream& operator<<(QDataStream& out, const BGPInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, BGPInfo*& info)
{
    info =new BGPInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const BGPInfo &info)
{
    dbg.nospace() << "\n";

    if ( !info.m_lstNeigbors.isEmpty() )
    {
        dbg.nospace() << "BGPNeighbor:\n";
        for (SBGPNeighbor i: info.m_lstNeigbors)
            dbg.space() << i.addressfamily << i.vrf << i.neighborip << i.as << i.upDownTime << i.prfxRcd << "\n";
    }

    if ( !info.m_lstNetworks.isEmpty() )
    {
        dbg.nospace() << "BGPNetworks:\n";

        for (SBGPNetwork i : info.m_lstNetworks)
            dbg.space() << i.neighborip << i.network << i.nexthop
                        << i.from << i.path << i.communityList << i.datetime << "\n";
    }

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
