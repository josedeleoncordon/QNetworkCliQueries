#include "ospfinfo.h"
#include "funciones.h"

QDataStream& operator<<(QDataStream& out, const SOSPFInfo& data)
{
    out << data.address;
    out << data.cost;
    out << data.id;
    out << data.interfaz;
    out << data.process;
    out << data.state;
    out << data.area;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SOSPFInfo& data)
{
    in >> data.address;
    in >> data.cost;
    in >> data.id;
    in >> data.interfaz;
    in >> data.process;
    in >> data.state;
    in >> data.area;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SOSPFInfo> &lstDest, QList<SOSPFInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SOSPFInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SOSPFInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SOSPFInfo &dest : lstDest )
        {
            if ( origin.interfaz == dest.interfaz )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.cost = origin.cost;
                dest.id = origin.id;
                dest.state = origin.state;
                dest.address = origin.address;
                dest.process = origin.process;
                dest.area = origin.area;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

OSPFInfo::OSPFInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
    m_process = 0;    
    m_abr = false;
    m_asbr = false;

    //actualizar para neighbors, para que consulte los de las vrfs
}

OSPFInfo::OSPFInfo(const OSPFInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_abr = other.m_abr;
    m_asbr = other.m_asbr;
    m_lstOSPFInfo = other.m_lstOSPFInfo;
    m_lstRouterIDs = other.m_lstRouterIDs;
}

OSPFInfo::~OSPFInfo()
{}

void OSPFInfo::getOSPFInfo()
{
    finished();
}

bool OSPFInfo::interfaceHasNeighbor(QString interface)
{
    for (SOSPFInfo &o : m_lstOSPFInfo)
    {
        if ( o.interfaz == interface )
            return true;
    }
    return false;
}

SOSPFInfo *OSPFInfo::interfaceOspfInfo(QString interface)
{
    for (SOSPFInfo &o : m_lstOSPFInfo)
    {
        if ( o.interfaz == interface )
            return &o;
    }
    return nullptr;
}

void OSPFInfo::addInterfaceOSPFInfo(SOSPFInfo s)
{
    m_lstOSPFInfo.append( s );
}

QDataStream& operator<<(QDataStream& out, const OSPFInfo& info)
{
    out << info.m_lstOSPFInfo;
    out << info.m_lstRouterIDs;
    out << info.m_abr;
    out << info.m_asbr;
    out << info.m_queryoption;
    out << info.m_queryName;
    return out;
}

QDataStream& operator>>(QDataStream& in, OSPFInfo& info)
{
    in >> info.m_lstOSPFInfo;
    in >> info.m_lstRouterIDs;
    in >> info.m_abr;
    in >> info.m_asbr;
    in >> info.m_queryoption;
    in >> info.m_queryName;
    return in;
}

QDataStream& operator<<(QDataStream& out, const OSPFInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, OSPFInfo*& info)
{
    info = new OSPFInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const OSPFInfo &info)
{
    dbg.space() << "OSPFInfo" << info.m_lstRouterIDs << "ABR:" << info.m_abr << "ASBR:" << info.m_asbr << ":\n";
    for (SOSPFInfo i : info.m_lstOSPFInfo)
        dbg.space() << "Id:" << i.id << i.interfaz << i.process << i.area << i.state << "Ad:" << i.address
                    << i.cost << i.operativo << i.datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}

