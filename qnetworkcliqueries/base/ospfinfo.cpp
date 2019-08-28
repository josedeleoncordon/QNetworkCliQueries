#include "ospfinfo.h"
#include "funciones.h"

SOSPFInfo::SOSPFInfo(const SOSPFInfo &other)
{
    interfaz = other.interfaz;
    cost = other.cost;
    id = other.id;
    state = other.state;
    address = other.address;
    process = other.process;
    area = other.area;
}

QDataStream& operator<<(QDataStream& out, const SOSPFInfo* data)
{
    out << data->address;
    out << data->cost;
    out << data->id;
    out << data->interfaz;
    out << data->process;
    out << data->state;
    out << data->area;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SOSPFInfo*& data)
{
    data = new SOSPFInfo;
    in >> data->address;
    in >> data->cost;
    in >> data->id;
    in >> data->interfaz;
    in >> data->process;
    in >> data->state;
    in >> data->area;
    //infobase
    in >> data->datetime;
    in >> data->operativo;
    return in;
}

void updateInfoList(QList<SOSPFInfo *> &lstDest, QList<SOSPFInfo *> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SOSPFInfo *dest = lstDest.at(c);
        if ( dest->datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest->operativo )
             lstDest.removeAt( c );
        else
        {
            dest->operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    foreach ( SOSPFInfo *origin, lstOrigin )
    {
        bool encontrado=false;
        foreach ( SOSPFInfo *dest, lstDest )
        {
            if ( origin->interfaz == dest->interfaz )
            {
                //Si se encontro, actualizamos los datos
                dest->datetime = origin->datetime;
                dest->operativo = true;
                dest->cost = origin->cost;
                dest->id = origin->id;
                dest->state = origin->state;
                dest->address = origin->address;
                dest->process = origin->process;
                dest->area = origin->area;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

OSPFInfo::OSPFInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{
    m_process = 0;    

    //actualizar para neighbors, para que consulte los de las vrfs
}

OSPFInfo::OSPFInfo(const OSPFInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_process = other.m_process;
    foreach (SOSPFInfo *ii, other.m_lstOSPFInfo)
    {
        SOSPFInfo *ii2 = new SOSPFInfo(*ii);
        m_lstOSPFInfo.append(ii2);
    }    
}

OSPFInfo::~OSPFInfo()
{
    qDeleteAll(m_lstOSPFInfo);
}

void OSPFInfo::getOSPFInfo()
{
    finished();
}

bool OSPFInfo::interfaceHasNeighbor(QString interface)
{
    foreach (SOSPFInfo *o, m_lstOSPFInfo)
    {
        if ( o->interfaz == interface )
            return true;
    }
    return false;
}

SOSPFInfo* OSPFInfo::interfaceOspfInfo(QString interface)
{
    foreach (SOSPFInfo *o, m_lstOSPFInfo)
    {
        if ( o->interfaz == interface )
            return o;
    }
    return nullptr;
}

QDataStream& operator<<(QDataStream& out, const OSPFInfo* info)
{
    out << info->m_lstOSPFInfo;
    return out;
}

QDataStream& operator>>(QDataStream& in, OSPFInfo*& info)
{
    info =new OSPFInfo(nullptr,nullptr);
    in >> info->m_lstOSPFInfo;
    return in;
}

QDebug operator<<(QDebug dbg, const OSPFInfo &info)
{
    dbg.nospace() << "OSPFInfo:\n";
    foreach (SOSPFInfo *i, info.m_lstOSPFInfo)
        dbg.space() << i->interfaz << i->process << i->state << i->id << i->address << i->area
                    << i->cost << i->datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}

