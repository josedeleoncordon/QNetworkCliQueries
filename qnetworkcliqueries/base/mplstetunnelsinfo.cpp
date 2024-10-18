#include "mplstetunnelsinfo.h"
#include "qnetworkquerieslogging.h"

SMplsTETunnelInfo::SMplsTETunnelInfo(const SMplsTETunnelInfo &other)
{
    name = other.name;
    origen = other.origen;
    destino = other.destino;
    TuID = other.TuID;
    route = other.route;
    role = other.role;
    InterfaceIn = other.InterfaceIn;
    InterfaceOut = other.InterfaceOut;
    status = other.status;
    autoRouteDestinationsCount = other.autoRouteDestinationsCount;
}

QDataStream& operator<<(QDataStream& out, const SMplsTETunnelInfo& data)
{
    out << data.name;
    out << data.origen;
    out << data.destino;
    out << data.TuID;
    out << data.route;
    out << data.role;
    out << data.InterfaceIn;
    out << data.InterfaceOut;
    out << data.status;
    out << data.autoRouteDestinationsCount;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMplsTETunnelInfo& data)
{
    in >> data.name;
    in >> data.origen;
    in >> data.destino;
    in >> data.TuID;
    in >> data.route;
    in >> data.role;
    in >> data.InterfaceIn;
    in >> data.InterfaceOut;
    in >> data.status;
    in >> data.autoRouteDestinationsCount;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SMplsTETunnelInfo> &lstDest, QList<SMplsTETunnelInfo> &lstOrigin )
{
    //Los tuneles TEs no es necesario fusionar, simplemente reemplazar los viejos por los nuevos
    lstDest = lstOrigin;
}

MplsTEtunnelsInfo::MplsTEtunnelsInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
}

MplsTEtunnelsInfo::MplsTEtunnelsInfo(const MplsTEtunnelsInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_lstMplsTEtunnels = other.m_lstMplsTEtunnels;
}

MplsTEtunnelsInfo::~MplsTEtunnelsInfo()
{}

void MplsTEtunnelsInfo::getMplsTETunnels()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "MplsTEtunnelsInfo::getMplsTETunnels()" << m_brand << "no soportado";
        finished();
        return;
    }

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_MplsTETunnels()));
    termSendText("sh mpls traffic-eng tunnels detail");
}

void MplsTEtunnelsInfo::on_term_receiveText_MplsTETunnels()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    SMplsTETunnelInfo *tunel = nullptr;
    bool route = false;
    foreach (QString line, lines)
    {
        line = line.simplified();
        qCDebug(mplstetunnels) << "line" << line;

        if ( m_os == "IOS XR" )
        {
            //Name: tunnel-te50502  Destination: 172.16.30.235  Ifhandle:0x8000560
            exp.setPattern("Name: (.+) Destination: (.+) ");
            if ( line.contains(exp,&match) )
            {
                //head
                SMplsTETunnelInfo i;
                i.datetime = QDateTime::currentDateTime();
                i.operativo = true;
                i.role = "Head";
                i.TuID = match.captured(1).simplified();
                i.destino = match.captured(2).simplified();
                m_lstMplsTEtunnels.append( i );
                tunel = &m_lstMplsTEtunnels.last();
                continue;
            }
            //LSP Tunnel 172.16.30.2 1 [7603] is signalled, Signaling State: up
            exp.setPattern("LSP Tunnel .+ signalled.+State: (.+)$");
            if ( line.contains(exp,&match) )
            {
                //mid tail
                SMplsTETunnelInfo i;
                i.datetime = QDateTime::currentDateTime();
                i.operativo = true;
                i.status = match.captured(1).simplified();
                m_lstMplsTEtunnels.append( i );
                tunel = &m_lstMplsTEtunnels.last();
                continue;
            }

            if ( !tunel )
                continue;

            if ( route )
            {
                if ( line.contains("Record Route") )
                    route = false;
                else
                {
                    exp.setPattern(".+ (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})$");
                    if ( line.contains(exp,&match) )
                        tunel->route.append(" "+match.captured(1).replace("*",""));
                }
                continue;
            }

            //Admin:    up Oper:   up   Path:  valid   Signalling: connected
            exp.setPattern("Admin: (.+) Oper: (.+) Path:");
            if ( line.contains(exp,&match) )
            {
                tunel->status = match.captured(2).simplified();
                continue;
            }

            //Autoroute Destinations
            exp.setPattern("Autoroute Destinations: (\\d+)$");
            if ( line.contains(exp,&match) )
            {
                tunel->autoRouteDestinationsCount = match.captured(1).toShort();
                continue;
            }

            //Signalled-Name: RO-NAP-COR9K-2_t50502
            exp.setPattern("Signalled-Name: (.+)$");
            if ( line.contains(exp,&match) )
            {
                tunel->name = match.captured(1).simplified();
                continue;
            }

            //Router-IDs: local      172.16.30.254
            exp.setPattern("Router-IDs: local (.+)$");
            if ( line.contains(exp,&match) )
            {
                tunel->origen = match.captured(1).simplified();
                continue;
            }

            //Outgoing Interface: TenGigE0/1/0
            exp.setPattern("Outgoing Interface: (.+)( |,|$)");
            if ( line.contains(exp,&match) )
            {
                tunel->InterfaceOut = estandarizarInterfaz( match.captured(1).simplified() );
                continue;
            }

            //Hop0: 172.16.20.150
            exp.setPattern("Hop\\d+: (.+)$");
            if ( line.contains(exp,&match) )
            {
                tunel->route.append(" "+match.captured(1).replace("*","").simplified());
                continue;
            }


            //LSPs:

            //Tunnel Name: ASR_920_A.DATA_REDUNDANCIA_t5 Tunnel Role: Mid
            exp.setPattern("Tunnel Name: (.+) Tunnel Role: (.+)$");
            if ( line.contains(exp,&match) )
            {
                tunel->name = match.captured(1).simplified();
                tunel->role = match.captured(2).simplified();
                continue;
            }

            //InLabel: TenGigE0/1/1/1, 514006
            //OutLabel: TenGigE0/0/1/3, 760
            exp.setPattern("InLabel: (.+), ");
            if ( line.contains(exp,&match) )
            {
                tunel->InterfaceIn = estandarizarInterfaz(match.captured(1).simplified());
                continue;
            }
            exp.setPattern("OutLabel: (.+), ");
            if ( line.contains(exp,&match) )
            {
                tunel->InterfaceOut = estandarizarInterfaz(match.captured(1).simplified());
                continue;
            }

            //Src 172.16.30.13 Dst 172.16.30.5, Tun_Id 57001, Tun_Instance 3205
            exp.setPattern("Src (.+) Dst (.+), Tun ID (.+), Tun");
            if ( line.contains(exp,&match) )
            {
                tunel->origen = match.captured(1).simplified();
                tunel->destino = match.captured(2).simplified();
                tunel->TuID = match.captured(3).simplified();
                continue;
            }

            //Outgoing:
            if ( line == "Outgoing:" )
            {
                route = true;
                continue;
            }
        }
        else
        {
            //Name: SITEL-COPA                          (Tunnel50757) Destination: 172.19.28.21
            exp.setPattern("Name: (.+)\\(Tunnel\\d+\\) Des");
            if ( line.contains(exp,&match) )
            {
                SMplsTETunnelInfo i;
                i.datetime = QDateTime::currentDateTime();
                i.operativo = true;
                i.name = match.captured(1).simplified();
                i.role = "Head";
                m_lstMplsTEtunnels.append( i );
                tunel = &m_lstMplsTEtunnels.last();
                qCDebug(mplstetunnels) << "Nuevo tunnel" << i.name << i.role;
                continue;
            }
            //LSP Tunnel FROM-CMY3-TO-WBP is signalled, connection is up
            exp2.setPattern("LSP Tunnel (.+) is .+ connection is (.+)$");
            if ( line.contains(exp2,&match) )
            {
                SMplsTETunnelInfo i;
                i.datetime = QDateTime::currentDateTime();
                i.operativo = true;
                i.name = match.captured(1).simplified();
                i.status = match.captured(2).simplified();
                m_lstMplsTEtunnels.append( i );
                tunel = &m_lstMplsTEtunnels.last();
                qCDebug(mplstetunnels) << "Nuevo tunnel" << i.name << i.role;
                continue;
            }

            if ( !tunel )
                continue;

            //Explicit Route: 172.16.20.81 172.16.20.90 172.16.20.106 172.16.20.157
            //Record   Route:    <--- simplified() lo deja solo con un espacio
            if ( route )
            {
                if ( line.contains("Record Route") )
                    route = false;
                else
                    tunel->route.append( " "+line.replace("*","").simplified() );
                continue;
            }

            //Admin: up         Oper: up     Path: valid       Signalling: connected
            exp.setPattern("Admin: (.+) Oper: (.+) Path: ");
            if ( line.contains(exp,&match) )
            {
                tunel->status = match.captured(2).simplified();
                continue;
            }

            //Src 172.16.30.13, Dst 172.16.30.5, Tun_Id 57001, Tun_Instance 3205
            exp.setPattern("Src (.+), Dst (.+), Tun_Id (.+), Tun_Instance");
            if ( line.contains(exp,&match) )
            {
                tunel->origen = match.captured(1).simplified();
                tunel->destino = match.captured(2).simplified();
                tunel->TuID = (tunel->role=="Head"?"Tunnel":"")+match.captured(3).simplified();
                qCDebug(mplstetunnels) << "origen destino" << tunel->origen << tunel->destino;
                continue;
            }

//            Explicit Route: 172.16.20.81 172.16.20.90 172.16.20.106 172.16.20.157
//                                  172.16.30.5
            exp.setPattern("Explicit Route: (.+)$");
            if ( line.contains(exp,&match) )
            {
                QString ro = match.captured(1);

                if ( !ro.contains("NONE") )
                {
                    tunel->route.append(match.captured(1).replace("*",""));
                    if ( tunel->role.isEmpty() ) tunel->role = "Mid";
                    route=true;
                }
                else
                    tunel->role = "Tail";

                continue;
            }

            //InLabel  : TenGigabitEthernet0/0/4, implicit-null
            exp.setPattern("^InLabel : (.+), .+");
            if ( line.contains(exp,&match) )
            {
                tunel->InterfaceIn=estandarizarInterfaz(match.captured(1).simplified());
                qCDebug(mplstetunnels) << "interfaz in" << tunel->InterfaceIn;
                continue;
            }

            //OutLabel : TenGigabitEthernet0/0/4, 1211
            exp.setPattern("^OutLabel : (.+), .+");
            if ( line.contains(exp,&match) )
            {
                tunel->InterfaceOut=estandarizarInterfaz(match.captured(1).simplified());
                qCDebug(mplstetunnels) << "interfaz out" << tunel->InterfaceOut;
                continue;
            }
        }
    }
    finished();
}

SMplsTETunnelInfo *MplsTEtunnelsInfo::mplsTEsFromTunnel(QString interfaz)
{
    for ( SMplsTETunnelInfo &mi : m_lstMplsTEtunnels )
    {
        if ( mi.TuID == interfaz )
            return &mi;
    }
    return nullptr;
}

QDataStream& operator<<(QDataStream& out, const MplsTEtunnelsInfo& info)
{
    out << info.m_lstMplsTEtunnels;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, MplsTEtunnelsInfo& info)
{
    in >> info.m_lstMplsTEtunnels;
    in >> info.m_queryoption;
    return in;
}

QDataStream& operator<<(QDataStream& out, const MplsTEtunnelsInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, MplsTEtunnelsInfo*& info)
{
    info = new MplsTEtunnelsInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const MplsTEtunnelsInfo &info)
{
    dbg.nospace() << "MplsTEtunnels:\n";

    for (SMplsTETunnelInfo i : info.m_lstMplsTEtunnels)
        dbg.space() << i.TuID << i.name << i.status << i.origen << i.destino
                    << i.role << i.InterfaceIn << i.InterfaceOut
                    << i.route << i.autoRouteDestinationsCount << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
