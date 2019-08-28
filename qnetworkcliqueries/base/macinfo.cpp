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

QDataStream& operator<<(QDataStream& out, const SMacInfo* data)
{
    out << data->mac;
    out << data->vlan;
    out << data->interfaz;
    out << data->age;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMacInfo*& data)
{
    data = new SMacInfo;
    in >> data->mac;
    in >> data->vlan;
    in >> data->interfaz;
    in >> data->age;
    //infobase
    in >> data->datetime;
    in >> data->operativo;
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

void updateInfoList(QList<SMacInfo *> &lstDest, QList<SMacInfo *> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SMacInfo *dest = lstDest.at(c);
        if ( dest->datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest->operativo )
             lstDest.removeAt( c );
        else
        {
            dest->operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    foreach ( SMacInfo *origin, lstOrigin )
    {
        bool encontrado=false;
        foreach ( SMacInfo *dest, lstDest )
        {
            if ( origin->mac == dest->mac &&
                 origin->vlan == dest->vlan )
            {
                //Si se encontró, actualizamos los datos
                dest->datetime = origin->datetime;
                dest->interfaz = origin->interfaz;
                dest->age = origin->age;
                dest->operativo = true;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );            
    }
}

MacInfo::MacInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{}

MacInfo::MacInfo(const MacInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_mac = other.m_mac;
    m_vlan = other.m_vlan;
    foreach (SMacInfo *ii, other.m_lstMacs)
    {
        SMacInfo *ii2 = new SMacInfo(*ii);
        m_lstMacs.append(ii2);
    }
}

MacInfo::~MacInfo()
{
    qDeleteAll(m_lstMacs);
}

void MacInfo::getMacInfo()
{
    finished();
}

QString MacInfo::macInterfaceLearnedFrom(QString mac)
{
    foreach ( SMacInfo *m , m_lstMacs )
    {
        if ( mac == m->mac )
            return m->interfaz;
    }
    return "";
}

QDataStream& operator<<(QDataStream& out, const MacInfo* info)
{
    out << info->m_lstMacs;
    return out;
}

QDataStream& operator>>(QDataStream& in, MacInfo*& info)
{
    info =new MacInfo(nullptr,nullptr);
    in >> info->m_lstMacs;
    return in;
}

QDebug operator<<(QDebug dbg, const MacInfo &info)
{
    dbg.nospace() << "MacInfo: (" << info.m_lstMacs.size() << ")\n";
    foreach (SMacInfo *i, info.m_lstMacs)
        dbg.space() << i->interfaz << i->mac << i->vlan << i->age << i->datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
