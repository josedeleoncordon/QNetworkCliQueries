#include "bgpneighborinfo.h"

SBGPNetwork::SBGPNetwork(const SBGPNetwork &other)
{
    network = other.network;
    RD = other.RD;
    nexthop = other.nexthop;
    path = other.path;
}

QDataStream& operator<<(QDataStream& out, const SBGPNetwork* data)
{
    out << data->network;
    out << data->nexthop;
    out << data->RD;
    out << data->path;
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

BGPNeighborInfo::BGPNeighborInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{
    m_type = IPV4;
}

BGPNeighborInfo::BGPNeighborInfo(const BGPNeighborInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_lstIPs = other.m_lstIPs;
    m_type = other.m_type;
}

void BGPNeighborInfo::getBGPNeighbors()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "BGPNeighborInfo::getBGPNeighbors():" << m_brand << "no soportado";
        finished();
        return;
    }    
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_BGPNeighbors()));

    m_type = QueriesConfiguration::instance()->mapQueries.value("BGPNeig_Type");
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

void BGPNeighborInfo::on_term_receiveText_BGPNeighbors()
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

void BGPNeighborInfo::getNetworks()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "BGPNeighborInfo::getRoute():" << m_brand << "no soportado";
        finished();
        return;
    }

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_networks()));

    m_type = QueriesConfiguration::instance()->mapQueries.value("BGPNeig_Type");
    m_vrf = QueriesConfiguration::instance()->mapQueries.value("BGPNeig_VRF");
    m_community = QueriesConfiguration::instance()->mapQueries.value("BGPNeig_Community");

    if ( m_type == "VPNV4" )
    {
        if ( m_os == "IOS XR" )
            termSendText("sh bgp vpnv4 unicast"+
                         (!m_vrf.isEmpty()?" vrf "+m_vrf:" ")+
                         (!m_community.isEmpty()?" community "+m_community:" "));
        else
        {
            finished();
            return;
        }
    }
    else
    {
        finished();
        return;
    }
}

void BGPNeighborInfo::on_term_receiveText_networks()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    int pathi=63;
    QString network;
    for (QString line : lines)
    {
        if ( line.contains("Weight Path") )
            pathi = line.indexOf("Path");

        exp.setPattern("\\d+\\.\\d+\\.\\d+\\.\\d+");
        if ( ! line.contains(exp) )
            continue;

        exp.setPattern("(\\d+\\.\\d+\\.\\d+\\.\\d+/\\d+) +(\\d+\\.\\d+\\.\\d+\\.\\d+) ");
        if ( line.contains(exp) )
        {
            SBGPNetwork *s = new SBGPNetwork;
            network = exp.cap(1);
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
            s->network = network;
            s->nexthop = exp.cap(1);
            s->path = line.mid( pathi ).replace("i","").simplified();

            m_lstNetworks.append(s);
            continue;
        }
    }
    finished();
}

QDataStream& operator<<(QDataStream& out, const BGPNeighborInfo* info)
{
    out << info->m_lstIPs;
    return out;
}

QDataStream& operator>>(QDataStream& in, BGPNeighborInfo*& info)
{
    info =new BGPNeighborInfo(nullptr,nullptr);
    in >> info->m_lstIPs;
    return in;
}

QDebug operator<<(QDebug dbg, const BGPNeighborInfo &info)
{
    dbg.nospace() << "BGPNeighbor:\n";
    for (QString i : info.m_lstIPs)
        dbg.space() << i << "\n";

    dbg.nospace() << "BGPNetworks:\n";
    for (SBGPNetwork *i : info.m_lstNetworks)
        dbg.space() << i->network << "," << i->nexthop << "," << i->path << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
