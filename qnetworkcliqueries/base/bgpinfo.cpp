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
    argument >> data.datetime;
    argument >> data.operativo;
    argument >> data.equipo;
    argument >> data.as;
    argument.endStructure();
    return argument;
}

BGPInfo::BGPInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
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

    m_type = m_queriesConfiguration.value("BGPNeig_Type",m_ip,m_os);

    if ( m_type.isEmpty() )
        m_type = "IPV4";

    if ( m_type == "IPV4" )
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

        m_lstNeigbors.append(s);
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
    m_neighborIPs = m_queriesConfiguration.values("BGPNetworks_NeighborIP",m_ip,m_os);
    m_vrf = m_queriesConfiguration.value("BGPNetworks_VRF",m_ip,m_os);
    m_community = m_queriesConfiguration.value("BGPNetworks_Community",m_ip,m_os);
    m_neighbor_int_out = m_queriesConfiguration.value("BGPNetworks_NeighborIn_Out",m_ip,m_os);

    qDebug() << "\n BGP getNetworks";
    qDebug() << m_vrf << m_neighborIPs;

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
            continue;
        }

        //routes
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
            continue;
        }

        exp.setPattern(" +(\\d+\\.\\d+\\.\\d+\\.\\d+) ");
        if ( line.contains(exp) )
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
            continue;
        }
    }

    if ( m_neighborIPs.isEmpty() )
        finished();
    else
        networksNextNeighbor();
}

QDataStream& operator<<(QDataStream& out, const BGPInfo& info)
{
    out << info.m_lstNeigbors;
    out << info.m_lstNetworks;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, BGPInfo& info)
{
    in >> info.m_lstNeigbors;
    in >> info.m_lstNetworks;
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
            dbg.space() << i.neighborip << i.as << i.upDownTime << i.prfxRcd << "\n";
    }

    if ( !info.m_lstNetworks.isEmpty() )
    {
        dbg.nospace() << "BGPNetworks:\n";

        for (SBGPNetwork i : info.m_lstNetworks)
            dbg.space() << i.neighborip << i.network << i.nexthop
                        << i.from << i.path << i.datetime << "\n";
    }

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
