#include "bgpinfo.h"

SBGPNetwork::SBGPNetwork(const SBGPNetwork &other)
{
    neighborip = other.neighborip;
    network = other.network;
    RD = other.RD;
    nexthop = other.nexthop;
    from = other.from;
    path = other.path;
    //infobase
    datetime = other.datetime;
    operativo = other.operativo;
}

QDataStream& operator<<(QDataStream& out, const SBGPNetwork* data)
{
    out << data->neighborip;
    out << data->network;
    out << data->RD;
    out << data->nexthop;
    out << data->from;
    out << data->path;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SBGPNetwork*& data)
{
    data = new SBGPNetwork;
    in >> data->network;
    in >> data->nexthop;
    in >> data->RD;
    in >> data->path;
    return in;
}

BGPInfo::BGPInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{
    m_type = IPV4;
}

BGPInfo::BGPInfo(const BGPInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_lstIPs = other.m_lstIPs;
    m_type = other.m_type;
}

void BGPInfo::getBGPNeighbors()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "BGPInfo::getBGPNeighbors():" << m_brand << "no soportado";
        finished();
        return;
    }    
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_BGPNeighbors()));

    m_type = QueriesConfiguration::instance()->value("BGPNeig_Type",m_ip);
    if ( m_type.isEmpty() )
        m_type = "VPNV4";

    if ( m_type == "VPNV4" )
    {
        if ( m_os == "IOS XR" )
            termSendText("sh bgp vpnv4 unicast summary");
        else
            termSendText("sh ip bgp vpnv4 all summary");
    }
}

void BGPInfo::on_term_receiveText_BGPNeighbors()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        exp.setPattern("^(\\d+\\.\\d+\\.\\d+\\.\\d+).+");
        if ( ! line.contains(exp) )
            continue;

        QStringList data = line.split(" ",QString::SkipEmptyParts);
        m_lstIPs.append( data.at(0).simplified() );
    }
    finished();
}

void BGPInfo::getNetworks()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "BGPInfo::getRoute():" << m_brand << "no soportado";
        finished();
        return;
    }

    m_neighborsPos=-1;
    m_neighborIPs = QueriesConfiguration::instance()->values("BGPNetworks_NeighborIP",m_ip);
    m_vrf = QueriesConfiguration::instance()->value("BGPNetworks_VRF",m_ip);
    m_community = QueriesConfiguration::instance()->value("BGPNetworks_Community",m_ip);
    m_neighbor_int_out = QueriesConfiguration::instance()->value("BGPNetworks_NeighborIn_Out",m_ip);

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networks()));

    if ( m_neighborIPs.isEmpty() )
    {
        if ( m_os == "IOS XR" )
        {
            if ( !m_community.isEmpty() )
                termSendText("sh bgp "+
                             (!m_vrf.isEmpty()?"vrf "+m_vrf:" ")+
                             "community "+m_community);
            else
                termSendText("sh bgp "+
                             (!m_vrf.isEmpty()?" vrf "+m_vrf:" "));
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
    if ( m_neighbor_int_out.isEmpty() )
    {
        finished();
        return;
    }

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
        else
        {
            finished();
            return;
        }
    }
    else
        finished();
}

void BGPInfo::on_term_receiveText_networks()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    int pathi = 0;
    QString network;
    for (QString line : lines)
    {
        //no realizar line.simplified ya que es necesario que este el texto por la posicion del as-path

        if ( line.contains("Weight Path") )
            pathi = line.indexOf("Path");
        if ( line.contains("AS Path") )
             pathi = line.indexOf("AS ");


        exp.setPattern("\\d+\\.\\d+\\.\\d+\\.\\d+");
        if ( ! line.contains(exp) || line.contains("routes") )
            continue;

        //Advertised-routes
        exp.setPattern("(\\d+\\.\\d+\\.\\d+\\.\\d+/\\d+) +(\\d+\\.\\d+\\.\\d+\\.\\d+) +(\\d+\\.\\d+\\.\\d+\\.\\d+) ");
        if ( line.contains(exp) )
        {
            SBGPNetwork *s = new SBGPNetwork;
            network = exp.cap(1);
            s->neighborip = m_currentNeighbor;
            s->network = network;
            s->nexthop = exp.cap(2);
            s->from = exp.cap(3);
            s->path = line.mid( pathi ).replace("i","").simplified();

            m_lstNetworks.append(s);
            continue;
        }

        //routes
        exp.setPattern("(\\d+\\.\\d+\\.\\d+\\.\\d+/\\d+) +(\\d+\\.\\d+\\.\\d+\\.\\d+) ");
        if ( line.contains(exp) )
        {
            SBGPNetwork *s = new SBGPNetwork;
            network = exp.cap(1);
            s->neighborip = m_currentNeighbor;
            s->network = network;
            s->nexthop = exp.cap(2);
            s->path = line.mid( pathi ).replace("i","").simplified();

            m_lstNetworks.append(s);
            continue;
        }

        exp.setPattern(" +(\\d+\\.\\d+\\.\\d+\\.\\d+) ");
        if ( line.contains(exp) )
        {
            SBGPNetwork *s = new SBGPNetwork;
            s->neighborip = m_currentNeighbor;
            s->network = network;
            s->nexthop = exp.cap(1);
            s->path = line.mid( pathi ).replace("i","").simplified();

            m_lstNetworks.append(s);
            continue;
        }
    }

    if ( m_neighborIPs.isEmpty() )
        finished();
    else
        networksNextNeighbor();
}

QDataStream& operator<<(QDataStream& out, const BGPInfo* info)
{
    out << info->m_lstIPs;
    out << info->m_lstNetworks;
    return out;
}

QDataStream& operator>>(QDataStream& in, BGPInfo*& info)
{
    info =new BGPInfo(nullptr,nullptr);
    in >> info->m_lstIPs;
    in >> info->m_lstNetworks;
    return in;
}

QDebug operator<<(QDebug dbg, const BGPInfo &info)
{
    dbg.nospace() << "\n";

    if ( !info.m_lstIPs.isEmpty() )
    {
        dbg.nospace() << "BGPNeighbor:\n";
        for (QString i : info.m_lstIPs)
            dbg.space() << i << "\n";
    }

    if ( !info.m_lstNetworks.isEmpty() )
    {
        dbg.nospace() << "BGPNetworks:\n";
        for (SBGPNetwork *i : info.m_lstNetworks)
            dbg.space() << i->neighborip << i->network << i->nexthop << i->from << i->path << "\n";
    }

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
