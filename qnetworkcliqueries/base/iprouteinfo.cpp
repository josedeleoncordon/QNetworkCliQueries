#include "iprouteinfo.h"

SIpRouteInfo::SIpRouteInfo(const SIpRouteInfo &other)
{
    network = other.network;
    mask = other.mask;
    protocol = other.protocol;
    via = other.via;
    vrf = other.vrf;
}

QDataStream& operator<<(QDataStream& out, const SIpRouteInfo& data)
{
    out << data.mask;
    out << data.network;
    out << data.vrf;
    out << data.protocol;
    out << data.via;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SIpRouteInfo& data)
{
    in >> data.mask;
    in >> data.network;
    in >> data.vrf;
    in >> data.protocol;
    in >> data.via;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SIpRouteInfo> &lstDest, QList<SIpRouteInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SIpRouteInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SIpRouteInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SIpRouteInfo &dest : lstDest )
        {
            if ( origin.network == dest.network &&
                 origin.mask == dest.mask /*&&
                 origin->vrf == dest->vrf*/ )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.protocol = origin.protocol;
                dest.via = origin.via;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

IPRouteInfo::IPRouteInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
    m_vrfsPos=-1;
    m_redPos=-1;
}

IPRouteInfo::IPRouteInfo(const IPRouteInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;    
    m_lstRoutes = other.m_lstRoutes;
    m_protocol = other.m_protocol;
    m_vrfs = other.m_vrfs;
}

IPRouteInfo::~IPRouteInfo()
{}

void IPRouteInfo::getIPRouteInfo()
{
    if ( m_brand != "Cisco" && m_brand != "Huawei" )
    {
        qDebug() << "IPRouteInfo::getIPRouteInfo()" << m_brand << "no soportado";
        finished();
        return;
    }

    m_vrfs = m_queriesConfiguration.values("IPRoutes_VRFs",m_ip,m_os,m_conexionID);
    m_protocol = m_queriesConfiguration.value("IPRoutes_protocol",m_ip,m_os,m_conexionID);
    m_redes = m_queriesConfiguration.values("IPRoutes_redes",m_ip,m_os,m_conexionID);

    if ( m_vrfs.isEmpty() )
        m_vrfs.append(""); //para la global

    if ( m_redes.isEmpty() ) {
        //consulta de tabla completa
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_general()));
        m_generalSiguienteVRF();
    }
    else {
        if ( m_vrfs.size() == 1 )
        {
            //consulta de rutas individuales
            m_vrf = m_vrfs.first();
            connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_individual()));
            m_individualSiguienteRed();
        }
        else {
            qDebug() << "IPRouteInfo::getIPRouteInfo() consulta individual de rutas. Unicamente establecer un maximo de 1 vrf";
            finished();
            return;
        }
    }
}

void IPRouteInfo::m_generalSiguienteVRF()
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

void IPRouteInfo::on_term_receiveText_general()
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
                SIpRouteInfo i;
                i.protocol = estandarizarProtocoloEnrutamiento(exp.cap(1));
                i.network = exp.cap(2);
                i.mask = exp.cap(3);
                i.vrf = m_vrf;
                i.via.append( exp.cap(4) );
                i.datetime = QDateTime::currentDateTime();
                i.operativo = true;
                m_lstRoutes.append(i);
                route = &i;
                continue;
            }

            //O E2     10.0.0.0 [110/20] via 10.192.56.42, 7w0d, TenGigabitEthernet8/2   --> sin mascara
            exp.setPattern("^(\\w{1,2}).+(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}).+ via (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}),.+$");
            if ( line.contains(exp) )
            {
                SIpRouteInfo i;
                i.datetime = QDateTime::currentDateTime();
                i.operativo = true;

                i.protocol = estandarizarProtocoloEnrutamiento(exp.cap(1));
                i.network = exp.cap(2);
                i.mask = mask;
                i.vrf = m_vrf;
                i.via.append( exp.cap(3) );
                m_lstRoutes.append(i);
                route = &i;
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
                SIpRouteInfo i;
                i.datetime = QDateTime::currentDateTime();
                i.operativo = true;
                i.protocol = estandarizarProtocoloEnrutamiento(exp.cap(1));
                i.network = exp.cap(2);
                i.mask = exp.cap(3);
                i.vrf = m_vrf;
                m_lstRoutes.append(i);
                route = &i;
                continue;
            }

            //O E2     129.250.207.152  --> sin mascara
            exp.setPattern("^(\\w{1,2}).+(\\d{1,3}\\.+\\d{1,3}\\.+\\d{1,3}\\.+\\d{1,3})$");

            if ( line.contains(exp) )
            {
                SIpRouteInfo i;
                i.datetime = QDateTime::currentDateTime();
                i.operativo = true;
                i.protocol = estandarizarProtocoloEnrutamiento(exp.cap(1));
                i.network = exp.cap(2);
                i.vrf = m_vrf;
                i.mask = mask;
                m_lstRoutes.append(i);
                route = &i;
                continue;
            }
        }
        else if ( m_brand == "Huawei" )
        {
            //10.10.22.0/24  IBGP    200  20            RD  10.192.33.7     100GE8/0/0
            exp.setPattern("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})/(\\d{1,2}) (\\w+) .+(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) ");
            if ( line.contains(exp) )
            {
                SIpRouteInfo i;
                i.protocol = estandarizarProtocoloEnrutamiento(exp.cap(3));
                i.network = exp.cap(1);
                i.mask = exp.cap(2);
                i.via.append( exp.cap(4) );
                i.vrf = m_vrf;
                i.datetime = QDateTime::currentDateTime();
                i.operativo = true;
                m_lstRoutes.append(i);
                route = &i;
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
    m_generalSiguienteVRF();
}

void IPRouteInfo::m_individualSiguienteRed()
{
    if ( m_redPos < m_redes.size()-1 )
    {
        m_redPos++;
        m_red = m_redes.at( m_redPos );
//        m_red.replace( QRegExp("/\\d+"),"" );

        if ( m_brand == "Cisco" )
            if ( m_red.contains(":") )
                //IPV6
                termSendText("sh route "+( !m_vrf.isEmpty()?" vrf "+m_vrf:"" )+" ipv6 "+m_red );
            else
                //IPV4
                termSendText("sh ip route "+( !m_vrf.isEmpty()?" vrf "+m_vrf:"" )+" "+m_red );
        else
        {
            finished();
            return;
        }
    }
    else
        finished();
}

void IPRouteInfo::on_term_receiveText_individual()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SIpRouteInfo *route = nullptr;

    for (QString line : lines)
    {
        line = line.simplified();

        QRegExp exp("Routing entry for (\\S+)/(\\d+)$");
        if ( line.contains(exp) )
        {
            SIpRouteInfo ri;
            ri.vrf = m_vrf;
            ri.network = exp.cap(1);
            ri.mask = exp.cap(2);
            ri.datetime = QDateTime::currentDateTime();
            ri.operativo = true;
            m_lstRoutes.append(ri);
            route = &m_lstRoutes.last();
            continue;
        }

        if ( !route )
            continue;

        exp.setPattern("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}).*, (from|via) ");
        if ( line.contains(exp) )
        {
            route->via.append( exp.cap(1) );
            continue;
        }
        exp.setPattern("::ffff:(\\S+), (from|via)");
        if ( line.contains(exp) )
        {
            route->via.append( exp.cap(1) );
            continue;
        }
    }

    m_individualSiguienteRed();
}

QDataStream& operator<<(QDataStream& out, const IPRouteInfo& info)
{
    out << info.m_lstRoutes;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, IPRouteInfo& info)
{
    in >> info.m_lstRoutes;
    in >> info.m_queryoption;
    return in;
}

QDataStream& operator<<(QDataStream& out, const IPRouteInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, IPRouteInfo*& info)
{
    info = new IPRouteInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const IPRouteInfo &info)
{
    dbg.nospace() << "IPRouteInfo:\n";
    foreach (SIpRouteInfo i, info.m_lstRoutes)
        dbg.space() << i.vrf << i.network << i.mask << i.via << i.protocol << i.datetime.toString("yyyy-MM-dd_hh:mm:ss") << i.operativo << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
