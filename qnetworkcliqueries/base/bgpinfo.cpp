#include "bgpinfo.h"
#include "queries.h"

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const SBGPNetwork& data)
{
    out << data.neighborip;
    out << data.vrf;
    out << data.network;
    out << data.RD;
    out << data.nexthop;
    out << data.from;
    out << data.path;
    out << data.communityList;
    out << data.originator;
    out << data.best;
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
    in >> data.vrf;
    in >> data.network;
    in >> data.RD;
    in >> data.nexthop;
    in >> data.from;
    in >> data.path;
    in >> data.communityList;
    in >> data.originator;
    in >> data.best;
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
    out << data.vrf;
    out << data.as;
    out << data.upDownTime;
    out << data.prfxRcd;
    out << data.vrf;
    out << data.addressfamily;
    out << data.prefixFilterIN;
    out << data.state;
    //infobase
    out << data.datetime;
    out << data.operativo;
    out << data.equipo;    
    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, SBGPNeighbor& data)
{
    in >> data.neighborip;
    in >> data.vrf;
    in >> data.as;
    in >> data.upDownTime;
    in >> data.prfxRcd;
    in >> data.vrf;
    in >> data.addressfamily;
    in >> data.prefixFilterIN;
    in >> data.state;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    in >> data.equipo;
    return in;
}

BGPInfo::BGPInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
    m_BGPNetworkAttAddOnlyBest=true;
    _mikrotiksecondquery=false;
}

BGPInfo::BGPInfo(const BGPInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_lstNeigbors= other.m_lstNeigbors;
    m_lstNetworks = other.m_lstNetworks;
    m_mapNeighborLstNetworks = other.m_mapNeighborLstNetworks;
    m_type = other.m_type;
    m_BGPNetworkAttAddOnlyBest = other.m_BGPNetworkAttAddOnlyBest;
    m_queryName = other.m_queryName;
}

BGPInfo::~BGPInfo()
{}

void BGPInfo::getBGPNeighbors()
{
    m_type = m_queriesConfiguration.value("BGPNeig_Type",m_ip,m_os,m_conexionID,m_queryName);
    m_vrf_vrfs = m_queriesConfiguration.values("BGPNeig_VRF_VRFs",m_ip,m_os,m_conexionID,m_queryName);
    if ( m_type.isEmpty() )
        m_type = "IPV4";

    if ( m_brand == "Cisco" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_BGPNeighbors()));

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
    else if ( m_os == "VRP" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_BGPNeighbors()));

        if ( m_type.isEmpty() )
            m_type = "IPV4";

        if ( m_type == "IPV4" || m_type == "IPV4LU" )
            termSendText("display bgp peer");
        else if ( m_type == "VRF" )
            getBGPNeighbors_VRF_nextVRF();
        else
            finished();
    }
    else if ( m_os == "RouterOS" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_BGPNeighbors()));
        if ( m_type == "IPV4" )
            termSendText("routing bgp session print\r");
        else
            finished();
    }
    else
    {
        qDebug() << "BGPInfo::getBGPNeighbors():" << m_brand << "no soportado";
        finished();
    }
}

void BGPInfo::getBGPNeighbors_VRF_nextVRF()
{
    if ( !m_vrf_vrfs.isEmpty() )
    {
        m_vrf_currentVRF = m_vrf_vrfs.takeFirst();
        if ( m_os == "IOS XR" )
            termSendText("sh bgp vrf "+m_vrf_currentVRF+" summary");
        else if (m_os == "IOS")
            termSendText("sh bgp vpnv4 unicast vrf "+m_vrf_currentVRF+" summary");
        else if (m_os == "VRP")
            termSendText("display bgp vpnv4 vpn-instance "+m_vrf_currentVRF+" peer");
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
    QStringList data;
    SBGPNeighbor *lastS=nullptr;
    bool append=false;
    for (QString line : lines)
    {
        //en una linea
        //172.16.148.210  4 52468    1306    1306        4    0    0 21:42:44 1
        //
        //o en varias filas indentadas
        //172.17.145.101  4 273068
        //                              0       0        0    0    0 21:42:53 Idle

        line = line.left( line.size()-1 );
        // qDebug() << "line" << line;

        if ( m_brand == "Cisco" || m_os == "VRP" )
        {
            exp.setPattern("^ *(\\d+\\.\\d+\\.\\d+\\.\\d+).+");
            if ( line.contains(exp,&match) )
            {
                data.clear();
                append=true;
            }
            if ( append )
                data.append( line.split(" ",Qt::SkipEmptyParts) );

            if ( data.isEmpty() )
                continue;

            if ( m_brand == "Cisco" )
            {
                if ( data.size() == 10 )
                {
                    SBGPNeighbor s;
                    s.neighborip = data.at(0).simplified().toLower();
                    s.as = data.at(2).simplified();
                    s.upDownTime = data.at(8).simplified();
                    s.prfxRcd = data.at(9).simplified();
                    s.vrf = m_vrf_currentVRF;
                    s.addressfamily = m_type;

                    m_lstNeigbors.append(s);
                }
            }
            else if ( m_os == "VRP" )
            {
                if ( data.size() == 9 )
                {
                    SBGPNeighbor s;
                    s.neighborip = data.at(0).simplified().toLower();
                    s.as = data.at(2).simplified();
                    s.upDownTime = data.at(6).simplified();
                    if ( data.at(7) != "Established" )
                        s.prfxRcd = "Active";
                    else
                        s.prfxRcd = data.at(8).simplified();
                    s.vrf = m_vrf_currentVRF;
                    s.addressfamily = m_type;

                    m_lstNeigbors.append(s);
                }
            }
        }
        else if ( m_os == "RouterOS" )
        {
            //MikroTik
            QRegularExpression exp("remote.address=(\\S+) ");
            if ( line.contains(exp,&match) )
            {
                SBGPNeighbor s;
                s.neighborip = match.captured(1);
                m_lstNeigbors.append(s);
                lastS = &m_lstNeigbors.last();
            }
            if ( !lastS ) continue;

            exp.setPattern("remote-as=(\\S+) ");
            if ( line.contains(exp,&match) )
                lastS->as = match.captured(1);
            exp.setPattern("remote.+ \\.as=(\\S+) ");
            if ( line.contains(exp,&match) )
                lastS->as = match.captured(1);
            exp.setPattern("in-filter=(\\S+) ");
            if ( line.contains(exp,&match) )
            {
                QString str = match.captured(1);
                str.replace("\"","");
                str.replace("\\","");
                lastS->prefixFilterIN = str;
            }
            exp.setPattern("input.+ .filter=(\\S+) ");
            if ( line.contains(exp,&match) )
            {
                QString str = match.captured(1);
                str.replace("\"","");
                str.replace("\\","");
                lastS->prefixFilterIN = str;
            }
        }
    }

    if ( m_type == "VRF" )
        getBGPNeighbors_VRF_nextVRF();
    else if ( m_os == "RouterOS" && !_mikrotiksecondquery )
    {
        _mikrotiksecondquery=true;
        termSendText("routing bgp peer print detail\r");
    }
    else
        finished();
}

void BGPInfo::getNetworks()
{
    m_neighborsPos=-1;
    m_neighborIPs = m_queriesConfiguration.values("BGPNetworks_NeighborIP",m_ip,m_os,m_conexionID,m_queryName);
    m_neighborIPsVRFs = m_queriesConfiguration.values("BGPNetworks_NeighborIPsVRFs",m_ip,m_os,m_conexionID,m_queryName);
    m_vrf = m_queriesConfiguration.value("BGPNetworks_VRF",m_ip,m_os,m_conexionID,m_queryName);
    m_community = m_queriesConfiguration.value("BGPNetworks_Community",m_ip,m_os,m_conexionID,m_queryName);
    m_neighbor_int_out = m_queriesConfiguration.value("BGPNetworks_NeighborIn_Out",m_ip,m_os,m_conexionID,m_queryName);

    qDebug() << "\n BGP getNetworks";
    qDebug() << m_vrfs << m_neighborIPs << m_neighborIPsVRFs;

    if ( m_neighborIPs.isEmpty() && m_neighborIPsVRFs.isEmpty() )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networks()));
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
            QString version;
            if ( m_currentNeighbor.contains(":") )
            {
                version="6";
                connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networksv6()));
            }
            else
            {
                version="4";
                connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networks()));
            }
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
                             (!m_vrf.isEmpty()?"vpnv"+version+" vpn-instance "+m_vrf:" ")+
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
            QString version;
            if ( Neighbor_VRF.contains("_") )
            {
                QStringList data = Neighbor_VRF.split("_",Qt::SkipEmptyParts);
                m_currentNeighbor = data.first();
                m_vrf_currentVRF = data.last();
            }
            else
            {
                m_currentNeighbor = Neighbor_VRF;
                m_vrf_currentVRF.clear();
            }
            if ( m_currentNeighbor.contains(":") )
            {
                version="6";
                connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networksv6()));
            }
            else
            {
                version="4";
                connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networks()));
            }

            if ( m_os == "IOS XR" )
            {
                termSendText("sh bgp "+
                             (!m_vrf_currentVRF.isEmpty()?" vrf "+m_vrf_currentVRF:" ")+
                             " ipv"+version+" unicast "+
                             " neighbor "+m_currentNeighbor+" "+
                             (m_neighbor_int_out=="IN"
                              ?"routes":(m_neighbor_int_out=="OUT"
                                         ?"advertised-routes":"")));
            }
            else if ( m_os == "VRP" )
            {
                termSendText("display bgp "+
                             (!m_vrf_currentVRF.isEmpty()?"vpnv"+version+" vpn-instance "+m_vrf_currentVRF:" ")+
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

    qDebug() << "BGPInfo::on_term_receiveText_networks()" << m_currentNeighbor;

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
        if ( ! line.contains(exp,&match) || line.contains("routes") )
            continue;

        //Advertised-routes Cisco
        exp.setPattern("(\\d+\\.\\d+\\.\\d+\\.\\d+/\\d+) +(\\d+\\.\\d+\\.\\d+\\.\\d+) +((\\d+\\.\\d+\\.\\d+\\.\\d+|Local)) ");
        if ( line.contains(exp,&match) )
        {
            SBGPNetwork s;
            network = match.captured(1).toLower();
            s.neighborip = m_currentNeighbor;
            s.vrf = m_vrf_currentVRF;
            s.network = network;
            s.nexthop = match.captured(2);
            s.from = match.captured(3);
            s.path = line.mid( pathi ).replace("i","").replace("?","").simplified();
            s.operativo = true;
            s.datetime = QDateTime::currentDateTime();
            s.equipo = m_parentQuery->hostName();

            qDebug() << "Agregando" << m_currentNeighbor << s.network;

            m_lstNetworks.append(s);
            m_lstNeighborNetworks.append(s);
            continue;
        }

        //Routes Cisco,  Advertised-routes y Routes Huawei
        exp.setPattern("(\\d+\\.\\d+\\.\\d+\\.\\d+/\\d+) +(\\d+\\.\\d+\\.\\d+\\.\\d+) ");
        if ( line.contains(exp,&match) )
        {
            SBGPNetwork s;
            network = match.captured(1).toLower();
            s.neighborip = m_currentNeighbor;
            s.vrf = m_vrf_currentVRF;
            s.network = network;
            s.nexthop = match.captured(2);
            s.path = line.mid( pathi ).replace("i","").replace("?","").simplified();
            s.operativo = true;
            s.datetime = QDateTime::currentDateTime();
            s.equipo = m_parentQuery->hostName();

            qDebug() << "Agregando" << m_currentNeighbor << s.network;

            m_lstNetworks.append(s);
            m_lstNeighborNetworks.append(s);
            continue;
        }

        exp.setPattern(" +(\\d+\\.\\d+\\.\\d+\\.\\d+) ");
        if ( line.contains(exp,&match) && !network.isEmpty() && !m_currentNeighbor.isEmpty())
        {
            SBGPNetwork s;
            s.neighborip = m_currentNeighbor.toLower();
            s.vrf = m_vrf_currentVRF;
            s.network = network.toLower();
            s.nexthop = match.captured(1);
            s.path = line.mid( pathi ).replace("i","").replace("?","").simplified();
            s.operativo = true;
            s.datetime = QDateTime::currentDateTime();
            s.equipo = m_parentQuery->hostName();

            qDebug() << "Agregando" << m_currentNeighbor << s.network;

            m_lstNetworks.append(s);
            m_lstNeighborNetworks.append(s);
            continue;
        }
    }
    m_mapNeighborLstNetworks.insert( m_currentNeighbor, m_lstNeighborNetworks );
    term->disconnectReceiveTextSignalConnections();

    if ( m_neighborIPs.isEmpty() && m_neighborIPsVRFs.isEmpty() )
        finished();
    else
        networksNextNeighbor();
}

void BGPInfo::on_term_receiveText_networksv6()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    qDebug() << "BGPInfo::on_term_receiveText_networksv6()" << m_currentNeighbor;
    // qDebug() << "txt" << txt;

    QList<SBGPNetwork> m_lstNeighborNetworks;

    if ( m_os == "IOS XR" )
    {
        int in=false;
        if (m_neighbor_int_out=="IN") in=true;
        QMap<int,QRegularExpression> map;
        if (in)
        {
            map.insert( 3,QRegularExpression("([0-9a-f]{1,4}:\\S+/\\d{1,3})"));
            map.insert(22,QRegularExpression(" {,3}\\S+"));
            map.insert(63,QRegularExpression(".+$"));
        }
        else
        {
            map.insert( 0,QRegularExpression("([0-9a-f]{1,4}:\\S+/\\d{1,3})"));
            map.insert(19,QRegularExpression(" {,3}\\S+"));
            map.insert(51,QRegularExpression(".+$"));
        }
        QList<QStringList> lstCampos = tableMultiRow2Table(txt,map);

        for ( QStringList lst : lstCampos )
        {
            if ( lst.size() != 3 )
                continue;

            QString red = lst.at(0);

            if ( red == "::/0" )
                continue;

            QString peer = lst.at(1);
            QString aspath = lst.at(2);
            aspath.replace("i","");
            aspath.replace("?","");

            SBGPNetwork s;
            s.neighborip = m_currentNeighbor.toLower();
            s.vrf = m_vrf_currentVRF;
            s.network = red.toLower();
            s.nexthop = peer.toLower();
            s.path = aspath;
            s.operativo = true;
            s.datetime = QDateTime::currentDateTime();
            s.equipo = m_parentQuery->hostName();

            qDebug() << "Agregando" << m_currentNeighbor << s.network;

            m_lstNetworks.append(s);
            m_lstNeighborNetworks.append(s);
            continue;
        }
        m_mapNeighborLstNetworks.insert( m_currentNeighbor, m_lstNeighborNetworks );
    }
    else if ( m_os == "VRP" )
    {
        QString lastNetwork;
        QString lastPrefix;
        QString lastNexthop;
        for ( QString line : txt.split("\n",Qt::SkipEmptyParts) )
        {
            line = line.simplified();
            qDebug() << "Line IPV6" << line;
            QRegularExpression exp("Network +: (\\S+) +PrefixLen +: (\\d+)$");
            QRegularExpressionMatch match;
            if ( line.contains(exp,&match) )
            {
                lastNetwork = match.captured(1).toLower();
                lastPrefix = match.captured(2);
                continue;
            }
            exp.setPattern("NextHop +: (\\S+) +");
            if ( line.contains(exp,&match) )
            {
                lastNexthop = match.captured(1).toLower();
                continue;
            }

            if (lastNetwork.isEmpty() && lastNexthop.isEmpty())
                continue;

            exp.setPattern("Path/Ogn +: (.+)");
            if ( line.contains(exp,&match) )
            {
                SBGPNetwork s;
                s.neighborip = m_currentNeighbor.toLower();
                s.vrf = m_vrf_currentVRF;
                s.network = lastNetwork+"/"+lastPrefix;
                s.nexthop = lastNexthop;
                s.path = match.captured(1).replace("i","").replace("?","");
                s.operativo = true;
                s.datetime = QDateTime::currentDateTime();
                s.equipo = m_parentQuery->hostName();

                m_lstNetworks.append(s);
                m_lstNeighborNetworks.append(s);

                lastNetwork.clear();
                lastPrefix.clear();
                lastNexthop.clear();
            }
        }
        m_mapNeighborLstNetworks.insert( m_currentNeighbor, m_lstNeighborNetworks );
    }

    term->disconnectReceiveTextSignalConnections();
    if ( m_neighborIPs.isEmpty() && m_neighborIPsVRFs.isEmpty() )
        finished();
    else
        networksNextNeighbor();
}

void BGPInfo::getNetworksBGPAttr()
{
    m_networks = m_queriesConfiguration.values("BGPNetworksBGPInfo_Network",m_ip,m_os,m_conexionID,m_queryName);
    m_vrf = m_queriesConfiguration.value("BGPNetworksBGPInfo_VRF",m_ip,m_os,m_conexionID,m_queryName);
    QString bestall = m_queriesConfiguration.value("BGPNetworksBGPInfo_BestOrALL",m_ip,m_os,m_conexionID,m_queryName);
    if ( bestall == "All" ) m_BGPNetworkAttAddOnlyBest = false;

    qDebug() << "\n BGP getNetworksBGPInfo";
    qDebug() << m_vrf << bestall << m_networks;

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

    m_currentNetworkTxt = m_networks.takeFirst();
    QString redhuawei = m_currentNetworkTxt;
    redhuawei.replace("/"," ");

    if ( m_os == "IOS XR" )
    {
        termSendText("sh bgp "+
                     (!m_vrf.isEmpty()?" vrf "+m_vrf+" ":" ")+
                     m_currentNetworkTxt);
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
    QString lastLP;
    QString lastOriginator;
    bool lastBest = false;
    QStringList lastLstCommunities;
    for (QString line : lines)
    {
        if ( m_os == "IOS XR" )
        {
            //no realizar line.simplified ya que es necesario tener los caracteres de espacio al inicio de la cadena
//            qDebug() << line;

            QRegularExpression exp("BGP routing table entry for (.+),");
            if ( line.contains(exp,&match) )
            {
//                qDebug() << "----- lastred" << match.captured(1);
                continue;
            }

            //AS
            exp.setPattern("^  \\d+");
            if ( line.contains(exp,&match) )
            {
                if ( line.contains("received-only") )
                    continue;

                lastNH.clear();
                lastASN.clear();
                lastFrom.clear();
                lastBest=false;
                lastLstCommunities.clear();

                line = line.simplified();
                exp.setPattern("(\\d| )+");
                if ( line.contains(exp,&match) )
                {
                    lastASN = match.captured(0);
                    if ( lastASN.contains(" ") )
                        lastASN = lastASN.split(" ",Qt::SkipEmptyParts).last();
//                    qDebug() << "----- lastASN" << lastASN;
                }
                continue;
            }
            exp.setPattern("^  Local");
            if ( line.contains(exp,&match) )
            {
                lastNH.clear();
                lastASN.clear();
                lastFrom.clear();
                lastBest=false;
                lastLstCommunities.clear();

                lastASN = "52468";
//                qDebug() << "lastASN" << lastASN;
                continue;
            }

            if ( lastASN.isEmpty() )
                continue;

            //Peer IP y From
            exp.setPattern("^    (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}).+from (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) .+");
            if ( line.contains(exp,&match) )
            {
                lastNH = match.captured(1);
                lastFrom = match.captured(2);
//                qDebug() << "---- lastNh" << lastNH;
//                qDebug() << "---- lastfrom" << lastFrom;
                continue;
            }

            //lp y best
            exp.setPattern("localpref (\\d+),");
            if ( line.contains(exp,&match) )
            {
                lastLP=match.captured(1);
                if ( line.contains(", best,") )
                {
                    //qDebug() << "---- BEST";
                    lastBest=true;
                }
                continue;
            }

            //comunidades
            exp.setPattern("^ +Community:");
            if ( line.contains(exp,&match) )
            {
                line.replace("Community:","");
                lastLstCommunities=line.simplified().split(" ",Qt::SkipEmptyParts);
                continue;
            }

            //Originator
            exp.setPattern("Originator: (\\S+)\\,");
            if ( line.contains(exp,&match) )
            {
                lastOriginator=match.captured(1);
                continue;
            }

            exp.setPattern("Path \\#\\d+:");
            if ( line.contains(exp,&match) )
            {
                if ( (!m_BGPNetworkAttAddOnlyBest) || (m_BGPNetworkAttAddOnlyBest && lastBest) )
                {
                    SBGPNetwork network;
                    network.equipo = m_parentQuery->ip();
                    network.vrf = m_vrf;
                    network.network = m_currentNetworkTxt;
                    network.neighborip = lastNH;
                    network.as = lastASN;
                    network.from = lastFrom;
                    network.communityList = lastLstCommunities;
                    network.best = lastBest;
                    network.lp = lastLP;
                    network.originator = lastOriginator;

                    m_lstNetworks.append( network );

                    if ( m_BGPNetworkAttAddOnlyBest && lastBest )
                        break;
                }

                lastNH.clear();
                lastASN.clear();
                lastFrom.clear();
                lastLP.clear();
                lastOriginator.clear();
                lastBest=false;
                lastLstCommunities.clear();
            }
        }
        else if ( m_os == "VRP" )
        {
            line = line.simplified();
//            qDebug() << "on_term_receiveText_networksAttr()" << line;

            QRegularExpression exp("BGP routing table entry information of (.+),");
            if ( line.contains(exp,&match) )
            {
//                qDebug() << "----- lastred" << match.captured(1);
                lastNH.clear();
                lastASN.clear();
                lastFrom.clear();
                continue;
            }

            //From
            exp.setPattern("^From: (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})");
            if ( line.contains(exp,&match) )
            {
                lastFrom = match.captured(1).toLower();
//                qDebug() << "---- lastfrom" << lastFrom;
                continue;
            }

            //IP nexthop
            exp.setPattern("^Original nexthop: (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})");
            if ( line.contains(exp,&match) )
            {
                lastNH = match.captured(1).toLower();
//                qDebug() << "---- lastNH" << lastNH;
                continue;
            }

            //AS, LP y best
            exp.setPattern("^AS-path ((\\w| )+),");
            if ( line.contains(exp,&match) )
            {
                QString AP = match.captured(1);

                exp.setPattern("(\\d| )+");
                if ( AP.contains(exp,&match) )
                {
                    lastASN = AP;
                    if ( lastASN.contains(" ") )
                        lastASN = lastASN.split(" ",Qt::SkipEmptyParts).last();
//                    qDebug() << "----- lastASN" << lastASN;
                }
                else if ( AP.contains("Nil") )
                {
                    lastASN = "52468";
//                    qDebug() << "lastASN" << lastASN;
                }

                //TODO agregar LP

                if ( line.contains(" best,") )
                    lastBest=true;

                if ( (!m_BGPNetworkAttAddOnlyBest) || (m_BGPNetworkAttAddOnlyBest && lastBest) )
                {
                    SBGPNetwork network;
                    network.network = m_currentNetworkTxt;
                    network.neighborip = lastNH;
                    network.as = lastASN;
                    network.from = lastFrom;
                    network.best = lastBest;
                    m_lstNetworks.append( network );

                    qDebug() << "Agregando SBGPNetwork attr" << network.network << network.neighborip << network.from;

                    if ( m_BGPNetworkAttAddOnlyBest && lastBest )
                        break;
                }

                lastNH.clear();
                lastASN.clear();
                lastFrom.clear();
                lastBest=false;
            }
        }                
    }
    if ( !lastNH.isEmpty() && !lastLstCommunities.isEmpty() )
    {
        //ultimo path que no se guardo ya que no hubo un Path #\\d para guardarlo
        if ( (!m_BGPNetworkAttAddOnlyBest) || (m_BGPNetworkAttAddOnlyBest && lastBest) )
        {
            SBGPNetwork network;
            network.equipo = m_parentQuery->ip();
            network.vrf = m_vrf;
            network.network = m_currentNetworkTxt;
            network.neighborip = lastNH;
            network.as = lastASN;
            network.from = lastFrom;
            network.communityList = lastLstCommunities;
            network.best = lastBest;
            network.lp = lastLP;
            network.originator = lastOriginator;

            qDebug() << "Agregando SBGPNetwork attr" << network.network << network.neighborip << network.from;

            m_lstNetworks.append( network );
        }

        lastNH.clear();
        lastASN.clear();
        lastFrom.clear();
        lastLP.clear();
        lastOriginator.clear();
        lastBest=false;
        lastLstCommunities.clear();
    }

    if ( m_os == "IOS XR" )
        bgpNextNetwork();
    else if ( m_os == "VRP" )
    {
        QString redhuawei = m_currentNetworkTxt;
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

    QString lastfrom;
    for (QString line : lines)
    {
        line = line.simplified();

        qDebug() << line;

        QRegularExpression exp("From: (.+)$");
        if ( line.contains(exp,&match) )
        {
            lastfrom = match.captured(1);
            qDebug() << "---- lastFrom" << lastfrom;
            continue;
        }

        exp.setPattern("Community: (.+)$");
        if ( line.contains( exp,&match ) )
        {
            for ( SBGPNetwork &m_currentNetwork : m_lstNetworks )
                if ( lastfrom == m_currentNetwork.from )
                {
                    QString comunidades = match.captured(1);
                    comunidades.replace("<","");
                    comunidades.replace(">","");
                    comunidades.replace(",","");
                    m_currentNetwork.communityList = comunidades.split(" ",Qt::SkipEmptyParts);
                    qDebug() << "---- Agregando comunidades";
                    break;
                }
        }
    }

    bgpNextNetwork();
}

void BGPInfo::getBGPNeighborsDetail()
{
    m_type = m_queriesConfiguration.value("BGPNeig_Type",m_ip,m_os,m_conexionID,m_queryName);
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_BGPNeighborsDetail()));
    if ( m_type.isEmpty() )
        m_type = "IPV4";

    if ( m_type == "IPV4" )
    {
        if ( m_os == "IOS XR" || m_os == "IOS" )
            termSendText("sh bgp neighbor");
        else if ( m_os == "VRP" )
            termSendText("display bgp peer verbose");
    }
    else if ( m_type == "IPV4LU" )
    {
        if ( m_os == "IOS XR" )
            termSendText("show bgp ipv4 labeled-unicast neighbor");
        else if (m_os=="IOS")
            termSendText("sh bgp neighbor");
        else if (m_os=="VRP")
            termSendText("display bgp peer verbose");
    }
    else if ( m_type == "VPNV4" )
    {
        //TODO terminar
        finished();
    }
    else if ( m_type == "VRF" )
        // getBGPNeighborsImportExportRPL_VRF_nextVRF();
        finished();
    else
        finished();
}

void BGPInfo::on_term_receiveText_BGPNeighborsDetail()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    term->disconnectReceiveTextSignalConnections();

    QStringList lines = txt.split("\n");

    QString lastNeighbor;
    QString lastState;
    QString lastAF;
    QString lastAS;
    QString lastVRF;
    QString lastRPLIN;
    QString lastRPLOUT;
    QString lastPrefixin;
    bool lastAPenabled=false;

    QRegularExpression reg;
    QRegularExpressionMatch match;
    for (QString line : lines)
    {
        line = removeEOL(line);
        if (m_os=="IOS XR")
        {
            reg.setPattern("BGP neighbor is (\\S+)");
            if (line.contains(reg,&match))
            {
                lastNeighbor=match.captured(1);
                exp.setPattern("vrf (\\S+)");
                if (line.contains(reg,&match))
                    lastVRF=match.captured(1);
                continue;
            }
            reg.setPattern("BGP state = (\\S+)( |,|$)");
            if (line.contains(reg,&match))
            {
                lastState=match.captured(1).simplified().replace(",","");
                continue;
            }
            reg.setPattern("Remote AS (\\d+)");
            if (line.contains(reg,&match))
            {
                lastAS=match.captured(1);
                continue;
            }
            reg.setPattern("For Address Family: (.+)");
            if (line.contains(reg,&match))
            {
                lastAF=match.captured(1);
                continue;
            }
            reg.setPattern("Policy for incoming advertisements is (\\S+)");
            if (line.contains(reg,&match))
            {
                lastRPLIN=match.captured(1);
                continue;
            }
            reg.setPattern("Policy for outgoing advertisements is (\\S+)");
            if (line.contains(reg,&match))
            {
                lastRPLOUT=match.captured(1);
                continue;
            }
            reg.setPattern("(\\d+) accepted prefixes");
            if (line.contains(reg,&match))
            {
                lastPrefixin=match.captured(1);
                continue;
            }
            reg.setPattern("Additional-paths operation: (.+)");
            if (line.contains(reg,&match))
            {
                if (match.captured(1).contains("Receive"))
                    lastAPenabled=true;

                if (lastNeighbor.isEmpty() || lastAS.isEmpty())
                    continue;

                if (m_type == "IPV4" && lastAF == "IPv4 Unicast" ||
                    m_type == "IPV4LU" && lastAF == "IPv4 Labeled-unicast"
                    )
                {
                    SBGPNeighborDetail s;
                    s.additionalPathsEnabled = lastAPenabled;
                    s.addressfamily = m_type;
                    s.as = lastAS;
                    s.datetime=QDateTime::currentDateTime();
                    s.equipo = m_parentQuery->hostName();
                    s.neighborip = lastNeighbor;
                    s.operativo = true;
                    s.prfxRcd=lastPrefixin;
                    s.rplIN=lastRPLIN;
                    s.rplOUT=lastRPLOUT;
                    s.vrf = lastVRF;
                    s.state = lastState;
                    m_lstNeigborsDetail.append(s);
                    qDebug() << "Agregando SBGPNeighborDetail" << lastAS << lastNeighbor;

                    lastRPLIN.clear();
                    lastPrefixin="0";
                    lastAPenabled=false;
                    lastAS.clear();
                    lastAF.clear();
                    lastNeighbor.clear();
                    lastRPLIN.clear();
                    lastRPLOUT.clear();
                    lastVRF.clear();
                    lastState.clear();
                }
                else
                {
                    lastRPLIN.clear();
                    lastRPLOUT.clear();
                    lastPrefixin="0";
                }
                continue;
            }
        }
        else if (m_os=="IOS")
        {
            line=line.simplified();
            reg.setPattern("BGP neighbor is (\\S+), remote AS (\\d+)");
            if (line.contains(reg,&match))
            {
                lastNeighbor=match.captured(1);
                lastAS=match.captured(2);
                continue;
            }
            reg.setPattern("BGP state = (\\S+)(,|$)");
            if (line.contains(reg,&match))
            {
                lastState=match.captured(1);
                continue;
            }
            reg.setPattern("Route map for incoming advertisements is (\\S+)");
            if (line.contains(reg,&match))
            {
                lastRPLIN=match.captured(1);
                continue;
            }
            reg.setPattern("Route map for outgoing advertisements is (\\S+)");
            if (line.contains(reg,&match))
            {
                lastRPLOUT=match.captured(1);
                continue;
            }
            reg.setPattern("Additional Paths receive capability: (\\S+)");
            if (line.contains(reg,&match))
            {
                if (match.captured(1).contains("advertised"))
                    lastAPenabled=true;
                continue;
            }
            reg.setPattern("Prefixes Current: \\d+ (\\d+)");
            if (line.contains(reg,&match))
            {
                lastPrefixin=match.captured(1);
                SBGPNeighborDetail s;
                s.additionalPathsEnabled = lastAPenabled;
                s.addressfamily = m_type;
                s.as = lastAS;
                s.datetime=QDateTime::currentDateTime();
                s.equipo = m_parentQuery->hostName();
                s.neighborip = lastNeighbor;
                s.operativo = true;
                s.prfxRcd = lastPrefixin;
                s.rplIN=lastRPLIN;
                s.rplOUT=lastRPLOUT;
                s.vrf = lastVRF;
                s.state = lastState;
                m_lstNeigborsDetail.append(s);
                qDebug() << "Agregando SBGPNeighborDetail" << lastAS << lastNeighbor;

                lastRPLIN.clear();
                lastPrefixin="0";
                lastAPenabled=false;
                lastAS.clear();
                lastNeighbor.clear();
                lastRPLIN.clear();
                lastRPLOUT.clear();
                lastVRF.clear();
                lastState.clear();
                continue;
            }
        }
        else if (m_os=="VRP")
        {
            reg.setPattern("BGP Peer is (\\S+),  remote AS (\\d+)");
            if (line.contains(reg,&match))
            {
                lastNeighbor=match.captured(1);
                lastAS=match.captured(2);
                continue;
            }
            reg.setPattern("BGP current state: (\\S+)(,|$)");
            if (line.contains(reg,&match))
            {
                lastState=match.captured(1);
                continue;
            }
            reg.setPattern("Received total routes: (\\d+)");
            if (line.contains(reg,&match))
            {
                lastPrefixin=match.captured(1);
                continue;
            }
            reg.setPattern("^     (\\S+) address-family: (.+)");
            if (line.contains(reg,&match))
            {
                QString family=match.captured(1);
                bool ap=false;
                if (match.captured(2).contains("receive"))
                    ap=true;
                if (family=="IPv4-UNC" && m_type=="IPV4")
                    lastAPenabled=ap;
                else if (family=="IPv4-Label" && m_type=="IPV4LU")
                    lastAPenabled=ap;
                else if (family=="VPNv4" && m_type=="VPNV4")
                    lastAPenabled=ap;
                continue;
            }
            reg.setPattern("Import route policy is: (\\S+)");
            if (line.contains(reg,&match))
            {
                lastRPLIN=match.captured(1);
                continue;
            }
            reg.setPattern("Export route policy is: (\\S+)");
            if (line.contains(reg,&match))
            {
                lastRPLOUT=match.captured(1);
                continue;
            }
            reg.setPattern("Import route filter is: (\\S+)");
            if (line.contains(reg,&match))
            {
                lastRPLIN=match.captured(1);
                continue;
            }
            reg.setPattern("Export route filter is: (\\S+)");
            bool exportrplfilter=false;
            if (line.contains(reg,&match))
            {
                lastRPLOUT=match.captured(1);
                exportrplfilter=true;
            }

            reg.setPattern("^ No export route filter$");
            if (line.contains(reg) || exportrplfilter || line.contains("No routing policy is configured") )
            {
                SBGPNeighborDetail s;
                s.additionalPathsEnabled = lastAPenabled;
                s.addressfamily = m_type;
                s.as = lastAS;
                s.datetime=QDateTime::currentDateTime();
                s.equipo = m_parentQuery->hostName();
                s.neighborip = lastNeighbor;
                s.operativo = true;
                s.prfxRcd = lastPrefixin;
                s.rplIN=lastRPLIN;
                s.rplOUT=lastRPLOUT;
                s.vrf = lastVRF;
                s.state = lastState;
                m_lstNeigborsDetail.append(s);
                qDebug() << "Agregando SBGPNeighborDetail" << lastAS << lastNeighbor;

                lastRPLIN.clear();
                lastPrefixin="0";
                lastAPenabled=false;
                lastAS.clear();
                lastNeighbor.clear();
                lastRPLIN.clear();
                lastRPLOUT.clear();
                lastVRF.clear();
                lastState.clear();
                continue;
            }
        }
    }

    finished();
}

QDataStream& operator<<(QDataStream& out, const BGPInfo& info)
{
    out << info.m_lstNeigbors;
    out << info.m_lstNetworks;
    out << info.m_mapNeighborLstNetworks;
    out << info.m_queryoption;
    out << info.m_queryName;
    return out;
}

QDataStream& operator>>(QDataStream& in, BGPInfo& info)
{
    in >> info.m_lstNeigbors;
    in >> info.m_lstNetworks;
    in >> info.m_mapNeighborLstNetworks;
    in >> info.m_queryoption;
    in >> info.m_queryName;
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
            dbg.space() << i.addressfamily << i.vrf << i.neighborip << i.as << i.upDownTime
                        << i.prfxRcd << i.prefixFilterIN << "\n";
    }

    if ( !info.m_lstNeigborsDetail.isEmpty() )
    {
        dbg.nospace() << "BGPNeighborDetail:\n";
        for (SBGPNeighborDetail i: info.m_lstNeigborsDetail)
            dbg.space() << i.addressfamily << i.vrf << i.neighborip
                        << i.as << i.rplIN << i.rplOUT << i.state << i.prfxRcd << i.additionalPathsEnabled << "\n";
    }

    if ( !info.m_lstNetworks.isEmpty() )
    {
        dbg.nospace() << "BGPNetworks:\n";

        for (SBGPNetwork i : info.m_lstNetworks)
            dbg.space() << i.neighborip << i.network << i.nexthop
                        << i.from << i.path << i.communityList << i.datetime << i.best << i.originator << "\n";
    }

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
