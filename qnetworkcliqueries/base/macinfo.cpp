#include "macinfo.h"
#include "funciones.h"

#include "queries.h"

SMacInfo::SMacInfo(const SMacInfo &other)
{
    mac = other.mac;
    vlan = other.vlan;
    interfaz = other.interfaz;
    age = other.age;
}

QDataStream& operator<<(QDataStream& out, const SMacInfo& data)
{
    out << data.mac;
    out << data.vlan;
    out << data.interfaz;
    out << data.age;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMacInfo& data)
{
    in >> data.mac;
    in >> data.vlan;
    in >> data.interfaz;
    in >> data.age;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

bool existMacAddress(QList<SMacInfo*> lst,SMacInfo* mac)
{
    foreach (SMacInfo *m, lst)
    {
        if ( m->mac == mac->mac &&
             m->vlan == mac->vlan )
            return true;
    }
    return false;
}

void updateInfoList(QList<SMacInfo> &lstDest, QList<SMacInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SMacInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SMacInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SMacInfo &dest : lstDest )
        {
            if ( origin.mac == dest.mac &&
                 origin.vlan == dest.vlan )
            {
                //Si se encontró, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.interfaz = origin.interfaz;
                dest.age = origin.age;
                dest.operativo = true;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );            
    }
}

MacInfo::MacInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{}

MacInfo::MacInfo(const MacInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_mac = other.m_mac;
    m_vlan = other.m_vlan;
    m_lstMacs = other.m_lstMacs;
}

MacInfo::~MacInfo()
{}

void MacInfo::getMacInfo()
{
    finished();
}

QString MacInfo::macInterfaceLearnedFrom(QString mac)
{
    for ( SMacInfo &m : m_lstMacs )
    {
        if ( mac == m.mac )
            return m.interfaz;
    }
    return "";
}

QDataStream& operator<<(QDataStream& out, const MacInfo& info)
{
    out << info.m_lstMacs;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, MacInfo& info)
{
    in >> info.m_lstMacs;
    in >> info.m_queryoption;
    return in;
}

QDataStream& operator<<(QDataStream& out, const MacInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, MacInfo*& info)
{
    info = new MacInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const MacInfo &info)
{
    dbg.nospace() << "MacInfo: (" << info.m_lstMacs.size() << ")\n";
    for (SMacInfo i : info.m_lstMacs)
        dbg.space() << i.interfaz << i.mac << i.vlan << i.age << i.datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
