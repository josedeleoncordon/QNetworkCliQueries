#include "rplinfo.h"

SRplRouteInfo::SRplRouteInfo(const SRplRouteInfo &other)
{
    nombre = other.nombre;
    txt = other.txt;
    //infobase
    datetime = other.datetime;
    operativo = other.operativo;
}

QDataStream& operator<<(QDataStream& out, const SRplRouteInfo& data)
{
    out << data.nombre;
    out << data.txt;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SRplRouteInfo& data)
{
    in >> data.nombre;
    in << data.txt;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SRplRouteInfo> &lstDest, QList<SRplRouteInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SRplRouteInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
            lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SRplRouteInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SRplRouteInfo &dest : lstDest )
        {
            if ( origin.nombre == dest.nombre )
            {
                //Si se encontro, actualizamos los datos
                dest.nombre = origin.nombre;
                dest.operativo = true;
                dest.txt = origin.txt;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

RplInfo::RplInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{}

RplInfo::RplInfo(const RplInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_lstRplRoutes = other.m_lstRplRoutes;
}

RplInfo::~RplInfo()
{}

void RplInfo::getRplRouteInfo()
{
    if ( m_os == "IOS XR" )
    {
        termSendText("show rpl route");
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_rplRoute()));
    }
    else
        finished();
}

void RplInfo::on_term_receiveText_rplRoute()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QRegExp exp("^route-policy (\\S+)$");
    QString lastRPL;
    QString rpltxt;
    for (QString line : lines)
    {
        line = line.left( line.size()-1 );
        if ( line.contains(exp) )
        {
            lastRPL = exp.cap(1);
            rpltxt.append( line+"\n" );
            continue;
        }

        if ( lastRPL.isEmpty() )
            continue;

        rpltxt.append( line+"\n" );

        if ( line.contains(QRegExp("^end-policy$")) )
        {
            SRplRouteInfo s;
            s.nombre = lastRPL;
            s.txt = rpltxt;
            m_lstRplRoutes.append(s);
            lastRPL.clear();
            rpltxt.clear();
        }
    }

    finished();
}

QDataStream& operator<<(QDataStream& out, const RplInfo& info)
{
    out << info.m_lstRplRoutes;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, RplInfo& info)
{
    in >> info.m_lstRplRoutes;
    in >> info.m_queryoption;
    return in;
}

QDataStream& operator<<(QDataStream& out, const RplInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, RplInfo*& info)
{
    info = new RplInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const RplInfo &info)
{
    dbg.nospace() << "RplInfo:\n";
    for (SRplRouteInfo i : info.m_lstRplRoutes)
        dbg.space() << "------" << i.nombre << "\n" << i.txt << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
