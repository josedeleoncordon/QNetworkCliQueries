#include "bgpneighborinfo.h"

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
        finished();
        return;
    }    

    qDebug() << "ip" << m_ip << m_brand << m_platform << m_os;

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));

    m_type = QueriesConfiguration::instance()->mapQueries.value("BGPNeig_Type");
    if ( m_type.isEmpty() )
        m_type = "VPNV4";

    if ( m_type == "VPNV4" )
    {
        if ( m_os == "IOS XR" )
        {
            qDebug() << m_ip << m_brand << m_platform << m_os << "fdasfdsafdsafdsafdsafdsafsad";
            termSendText("sh bgp vpnv4 unicast summary");
        }
        else
            termSendText("sh ip bgp vpnv4 all summary");
    }
}

void BGPNeighborInfo::on_term_receiveText()
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
    foreach (QString i, info.m_lstIPs)
        dbg.space() << i << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
