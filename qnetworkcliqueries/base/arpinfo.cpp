#include "arpinfo.h"

SIpInfo::SIpInfo(const SIpInfo &other)
{
    ip = other.ip;
    vrf = other.vrf;
    mac = other.mac;
    vlan = other.vlan;
    interfaz = other.interfaz;
    age = other.age;
    //infobase
    datetime = other.datetime;
    operativo = other.operativo;
}

QDataStream& operator<<(QDataStream& out, const SIpInfo& data)
{
    out << data.ip;
    out << data.vrf;
    //macinfo
    out << data.mac;
    out << data.vlan;
    out << data.interfaz;
    out << data.age;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SIpInfo& data)
{
    in >> data.ip;
    in >> data.vrf;
    //macinfo
    in >> data.mac;
    in >> data.vlan;
    in >> data.interfaz;
    in >> data.age;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SIpInfo> &lstDest, QList<SIpInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SIpInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SIpInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SIpInfo &dest : lstDest )
        {
            if ( origin.ip == dest.ip &&
                 origin.vlan == dest.vlan )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.interfaz = origin.interfaz;
                dest.age = origin.age;
                dest.mac = origin.mac;
                dest.vrf = origin.vrf;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

ArpInfo::ArpInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
    m_vrfsPos=-1;
}

ArpInfo::ArpInfo(const ArpInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{   
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_lstArp = other.m_lstArp;
    m_vrfsPos=-1;
    m_vrfs = other.m_vrfs;
    m_vrf = other.m_vrf;
    m_vrfsPos = other.m_vrfsPos;
    m_macip = other.m_macip;
}

ArpInfo::~ArpInfo()
{}

void ArpInfo::getArpInfo()
{
    finished();
}

QString ArpInfo::macFromIP(QString ip)
{
    for (SIpInfo &m : m_lstArp)
    {
        if ( ip == m.ip )
            return m.mac;
    }
    return "";
}

QDataStream& operator<<(QDataStream& out, const ArpInfo& info)
{
    out << info.m_lstArp;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, ArpInfo& info)
{
    in >> info.m_lstArp;
    in >> info.m_queryoption;
    return in;
}

QDataStream& operator<<(QDataStream& out, const ArpInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, ArpInfo*& info)
{
    info = new ArpInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const ArpInfo &info)
{
    dbg.nospace() << "ArpInfo:\n";
    for (SIpInfo i : info.m_lstArp)
        dbg.space() << i.ip << i.mac << i.vlan << i.age << i.vrf
                    << i.interfaz << i.datetime.toString("yyyy-MM-dd_hh:mm:ss") << i.operativo << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
