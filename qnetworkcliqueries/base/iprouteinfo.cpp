#include "iprouteinfo.h"

SIpRouteInfo::SIpRouteInfo(const SIpRouteInfo &other)
{
    network = other.network;
    mask = other.mask;
    protocol = other.protocol;
    via = other.via;
    vrf = other.vrf;
}

QDataStream& operator<<(QDataStream& out, const SIpRouteInfo* data)
{
    out << data->mask;
    out << data->network;
    out << data->vrf;
    out << data->protocol;
    out << data->via;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SIpRouteInfo*& data)
{
    data = new SIpRouteInfo;
    in >> data->mask;
    in >> data->network;
    in >> data->vrf;
    in >> data->protocol;
    in >> data->via;
    //infobase
    in >> data->datetime;
    in >> data->operativo;
    return in;
}

void updateInfoList(QList<SIpRouteInfo *> &lstDest, QList<SIpRouteInfo *> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SIpRouteInfo *dest = lstDest.at(c);
        if ( dest->datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest->operativo )
             lstDest.removeAt( c );
        else
        {
            dest->operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    foreach ( SIpRouteInfo *origin, lstOrigin )
    {
        bool encontrado=false;
        foreach ( SIpRouteInfo *dest, lstDest )
        {
            if ( origin->network == dest->network &&
                 origin->mask == dest->mask /*&&
                 origin->vrf == dest->vrf*/ )
            {
                //Si se encontro, actualizamos los datos
                dest->datetime = origin->datetime;
                dest->operativo = true;
                dest->protocol = origin->protocol;
                dest->via = origin->via;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

IPRouteInfo::IPRouteInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{
    m_vrfsPos=-1;
}

IPRouteInfo::IPRouteInfo(const IPRouteInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;    
    foreach (SIpRouteInfo *ii, other.m_lstRoutes)
    {
        SIpRouteInfo *ii2 = new SIpRouteInfo(*ii);
        m_lstRoutes.append(ii2);
    }
    m_protocol = other.m_protocol;
    m_vrfs = other.m_vrfs;
}

IPRouteInfo::~IPRouteInfo()
{
    qDeleteAll(m_lstRoutes);
}

void IPRouteInfo::getIPRouteInfo()
{
    if ( m_brand != "Cisco" && m_brand != "Huawei" )
    {
        finished();
        return;
    }

    m_vrfs = QueriesConfiguration::instance()->mapQueriesToList("IPRoutes_VRFs");
    m_protocol = QueriesConfiguration::instance()->mapQueries.value("IPRoutes_protocol");

    if ( m_vrfs.isEmpty() )
        m_vrfs.append(""); //para la global

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));
    m_siguienteVRF();
}

void IPRouteInfo::m_siguienteVRF()
{
    if ( m_vrfsPos < m_vrfs.size()-1 )
    {
        m_vrfsPos++;
        m_vrf = m_vrfs.at( m_vrfsPos );

        if ( m_brand == "Cisco" )
            termSendText("sh ip route "+( !m_vrf.isEmpty()?" vrf "+m_vrf:"" )+" "+m_protocol );
        else if ( m_brand == "Huawei" )
            termSendText("display ip routing-table "+
                         ( !m_vrf.isEmpty()?" vpn-instance "+m_vrf:"" )+
                         ( !m_protocol.isEmpty()?" protocol "+m_protocol:"" ) );
    }
    else
        finished();
}

void IPRouteInfo::on_term_receiveText()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SIpRouteInfo *route = nullptr;
    QString mask;

    foreach (QString line, lines)
    {
        line = line.simplified();       

        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
        if ( ! line.contains(exp) )
            continue;

        if ( m_brand == "Cisco" )
        {
            //  10.0.0.0/8 is variably subnetted, 4762 subnets, 16 masks
            exp.setPattern("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/(\\d{1,2}).+ subnetted.+");
            if ( line.contains(exp) )
            {
                mask = exp.cap(1);
                continue;
            }

            //O E2     10.0.0.0/24 [110/20] via 10.192.56.42, 7w0d, TenGigabitEthernet8/2   --> con mascara
            //S        10.110.163.14/32 [1/0] via 10.110.162.14
            exp.setPattern("^(\\w{1,2}).+(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})/(\\d{1,2}).+ via (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})(,.+)*$");
            if ( line.contains(exp) )
            {
                route = new SIpRouteInfo;
                route->queryParent = m_queriesParent;
                route->protocol = estandarizarProtocoloEnrutamiento(exp.cap(1));
                route->network = exp.cap(2);
                route->mask = exp.cap(3);
                route->vrf = m_vrf;
                route->via.append( exp.cap(4) );
                route->datetime = QDateTime::currentDateTime();
                route->operativo = true;
                m_lstRoutes.append(route);
                continue;
            }

            //O E2     10.0.0.0 [110/20] via 10.192.56.42, 7w0d, TenGigabitEthernet8/2   --> sin mascara
            exp.setPattern("^(\\w{1,2}).+(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}).+ via (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}),.+$");
            if ( line.contains(exp) )
            {
                route = new SIpRouteInfo;
                route->queryParent = m_queriesParent;
                route->datetime = QDateTime::currentDateTime();
                route->operativo = true;

                route->protocol = estandarizarProtocoloEnrutamiento(exp.cap(1));
                route->network = exp.cap(2);
                route->mask = mask;
                route->vrf = m_vrf;
                route->via.append( exp.cap(3) );
                m_lstRoutes.append(route);
                continue;
            }

            //[110/20] via 10.9.2.126, 7w0d, TenGigabitEthernet1/0/0
            //[1/0] via 10.110.162.14, Vlan1667
            exp.setPattern("^\\[.+\\] via (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})(,.+)$");
            if ( line.contains(exp) )
            {
                QString via = exp.cap(1);
                if ( !route->via.contains( via ) )
                    route->via.append( via );
                continue;
            }

            //O E2     129.250.207.152/30  --> con mascara
            exp.setPattern("^(\\w{1,2}).+(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})/(\\d{1,2})$");

            if ( line.contains(exp) )
            {
                route = new SIpRouteInfo;
                route->queryParent = m_queriesParent;
                route->datetime = QDateTime::currentDateTime();
                route->operativo = true;
                route->protocol = estandarizarProtocoloEnrutamiento(exp.cap(1));
                route->network = exp.cap(2);
                route->mask = exp.cap(3);
                route->vrf = m_vrf;
                m_lstRoutes.append(route);
                continue;
            }

            //O E2     129.250.207.152  --> sin mascara
            exp.setPattern("^(\\w{1,2}).+(\\d{1,3}\\.+\\d{1,3}\\.+\\d{1,3}\\.+\\d{1,3})$");

            if ( line.contains(exp) )
            {
                route = new SIpRouteInfo;
                route->queryParent = m_queriesParent;
                route->datetime = QDateTime::currentDateTime();
                route->operativo = true;
                route->protocol = estandarizarProtocoloEnrutamiento(exp.cap(1));
                route->network = exp.cap(2);
                route->vrf = m_vrf;
                route->mask = mask;
                m_lstRoutes.append(route);
                continue;
            }
        }
        else if ( m_brand == "Huawei" )
        {
            //10.10.22.0/24  IBGP    200  20            RD  10.192.33.7     100GE8/0/0
            exp.setPattern("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})/(\\d{1,2}) (\\w+) .+(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) ");
            if ( line.contains(exp) )
            {
                route = new SIpRouteInfo;
                route->queryParent = m_queriesParent;
                route->protocol = estandarizarProtocoloEnrutamiento(exp.cap(3));
                route->network = exp.cap(1);
                route->mask = exp.cap(2);
                route->via.append( exp.cap(4) );
                route->vrf = m_vrf;
                route->datetime = QDateTime::currentDateTime();
                route->operativo = true;
                m_lstRoutes.append(route);
                continue;
            }

            // IBGP    200  20            RD  10.192.32. 7    100GE8/0/0    //otra interfaz o ip de destino de la ruta anterior
            exp.setPattern("(\\w+) .+(\\d{1,3}\\.+\\d{1,3}\\.+\\d{1,3}\\.+\\d{1,3}) ");
            if ( line.contains(exp) )
            {
                if ( !route )
                    continue;

                QString via = exp.cap(2);
                if ( !route->via.contains(via) )
                    route->via.append(via);
                continue;
            }
        }
    }
    m_siguienteVRF();
}

QDataStream& operator<<(QDataStream& out, const IPRouteInfo* info)
{
    out << info->m_lstRoutes;
    return out;
}

QDataStream& operator>>(QDataStream& in, IPRouteInfo*& info)
{
    info =new IPRouteInfo(nullptr,nullptr);
    in >> info->m_lstRoutes;
    return in;
}

QDebug operator<<(QDebug dbg, const IPRouteInfo &info)
{
    dbg.nospace() << "IPRouteInfo:\n";
    foreach (SIpRouteInfo *i, info.m_lstRoutes)    
        dbg.space() << i->vrf << i->network << i->mask << i->via << i->protocol << i->datetime.toString("yyyy-MM-dd_hh:mm:ss") << i->operativo << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
