#include "mplsl2transport.h"

SMplsL2PWInfo::SMplsL2PWInfo(const SMplsL2PWInfo &other)
{
    VCID = other.VCID;
    AC = other.AC;
    destino = other.destino;
    preferredPath = other.preferredPath;
    remoteDescripcion = other.remoteDescripcion;
    estado = other.estado;
}

SMplsL2XconnectInfo::SMplsL2XconnectInfo(const SMplsL2XconnectInfo &other)
{
    VCID = other.VCID;
    AC = other.AC;
    destino = other.destino;
    preferredPath = other.preferredPath;
    remoteDescripcion = other.remoteDescripcion;
    estado = other.estado;
    xc = other.xc;
    descripcion = other.descripcion;
    interfaz = other.interfaz;
}

SMplsL2VFIInfo::SMplsL2VFIInfo(const SMplsL2VFIInfo &other)
{
    vfi = other.vfi;
    descripcion = other.descripcion;
    bridge = other.bridge;
    for ( SMplsL2PWInfo *i : other.lstPWs )
    {
        SMplsL2PWInfo *ii = new SMplsL2PWInfo( *i );
        lstPWs.append( ii );
    }
}

QDataStream& operator<<(QDataStream& out, const SMplsL2PWInfo* data)
{
    out << data->VCID;
    out << data->AC;
    out << data->destino;
    out << data->preferredPath;
    out << data->remoteDescripcion;
    out << data->estado;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMplsL2PWInfo*& data)
{
    data = new SMplsL2XconnectInfo;
    in >> data->VCID;
    in >> data->AC;
    in >> data->destino;
    in >> data->preferredPath;
    in >> data->remoteDescripcion;
    in >> data->estado;
    //infobase
    in >> data->datetime;
    in >> data->operativo;
    return in;
}

QDataStream& operator<<(QDataStream& out, const SMplsL2XconnectInfo* data)
{
    out << data->VCID;
    out << data->AC;
    out << data->destino;
    out << data->preferredPath;
    out << data->remoteDescripcion;
    out << data->estado;
    out << data->xc;
    out << data->descripcion;
    out << data->interfaz;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMplsL2XconnectInfo*& data)
{
    data = new SMplsL2XconnectInfo;
    in >> data->VCID;
    in >> data->AC;
    in >> data->destino;
    in >> data->preferredPath;
    in >> data->remoteDescripcion;
    in >> data->estado;
    in >> data->xc;
    in >> data->descripcion;
    in >> data->interfaz;
    //infobase
    in >> data->datetime;
    in >> data->operativo;
    return in;
}

QDataStream& operator<<(QDataStream& out, const SMplsL2VFIInfo* data)
{
    out << data->vfi;
    out << data->descripcion;
    out << data->bridge;
    out << data->lstPWs;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMplsL2VFIInfo*& data)
{
    data = new SMplsL2VFIInfo;
    in >> data->vfi;
    in >> data->descripcion;
    in >> data->bridge;
    in >> data->lstPWs;
    //infobase
    in >> data->datetime;
    in >> data->operativo;
    return in;
}

void updateInfoList(QList<SMplsL2XconnectInfo*> &lstDest, QList<SMplsL2XconnectInfo*> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SMplsL2XconnectInfo *dest = lstDest.at(c);
        if ( dest->datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest->operativo )
             lstDest.removeAt( c );
        else
        {
            dest->operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    foreach ( SMplsL2XconnectInfo *origin, lstOrigin )
    {
        bool encontrado=false;
        foreach ( SMplsL2XconnectInfo *dest, lstDest )
        {
            if ( origin->xc == dest->xc )
            {
                //Si se encontro, actualizamos los datos
                dest->datetime = origin->datetime;
                dest->operativo = true;
                dest->VCID = origin->VCID;
                dest->AC = origin->AC;
                dest->destino = origin->destino;
                dest->preferredPath = origin->preferredPath;
                dest->remoteDescripcion = origin->remoteDescripcion;
                dest->estado = origin->estado;
                dest->descripcion = origin->descripcion;
                dest->interfaz = origin->interfaz;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

void updateInfoList(QList<SMplsL2VFIInfo*> &lstDest, QList<SMplsL2VFIInfo*> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SMplsL2VFIInfo *dest = lstDest.at(c);
        if ( dest->datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest->operativo )
             lstDest.removeAt( c );
        else
        {
            dest->operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    foreach ( SMplsL2VFIInfo *origin, lstOrigin )
    {
        bool encontrado=false;
        foreach ( SMplsL2VFIInfo *dest, lstDest )
        {
            if ( origin->vfi == dest->vfi )
            {
                //Si se encontro, actualizamos los datos
                dest->datetime = origin->datetime;
                dest->operativo = true;
                dest->vfi = origin->vfi;
                dest->descripcion = origin->descripcion;
                dest->bridge = origin->bridge;
                dest->lstPWs = origin->lstPWs;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

MplsL2TransportInfo::MplsL2TransportInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{
}

MplsL2TransportInfo::MplsL2TransportInfo(const MplsL2TransportInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    foreach (SMplsL2XconnectInfo *ii, other.m_lstMplsL2Xconnects)
    {
        SMplsL2XconnectInfo *ii2 = new SMplsL2XconnectInfo(*ii);
        m_lstMplsL2Xconnects.append(ii2);
    }
    foreach (SMplsL2VFIInfo *ii, other.m_lstMplsL2VFIs)
    {
        SMplsL2VFIInfo *ii2 = new SMplsL2VFIInfo(*ii);
        m_lstMplsL2VFIs.append(ii2);
    }
}

MplsL2TransportInfo::~MplsL2TransportInfo()
{
    qDeleteAll(m_lstMplsL2Xconnects);
    qDeleteAll(m_lstMplsL2VFIs);
}

void MplsL2TransportInfo::getMplsL2Transport()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "MplsL2TransportInfo::getMplsL2Transport()" << m_brand << "no soportado";
        finished();
        return;
    }

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_MplsTETunnels()));
    termSendText("sh mpls traffic-eng tunnels detail");
}

void MplsL2TransportInfo::on_term_receiveText_MplsL2Transport_IOS()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    foreach (QString line, lines)
    {
        line = line.simplified();

    }
    finished();
}

void MplsL2TransportInfo::on_term_receiveText_MplsL2Transport_XR()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    foreach (QString line, lines)
    {
        line = line.simplified();

    }
    finished();
}

QDataStream& operator<<(QDataStream& out, const MplsL2TransportInfo* info)
{
    out << info->m_lstMplsL2Xconnects;
    out << info->m_lstMplsL2VFIs;
    return out;
}

QDataStream& operator>>(QDataStream& in, MplsL2TransportInfo*& info)
{
    info =new MplsL2TransportInfo(nullptr,nullptr);
    in >> info->m_lstMplsL2Xconnects;
    in >> info->m_lstMplsL2VFIs;
    return in;
}

QDebug operator<<(QDebug dbg, const MplsL2TransportInfo &info)
{
    dbg.nospace() << "MplsL2TransportInfo:\n";

    foreach (SMplsL2XconnectInfo *i, info.m_lstMplsL2Xconnects)
        dbg.space() << i->xc << i->interfaz << i->descripcion << i->AC << i->VCID <<
                       i->estado << i->destino << i->preferredPath << i->remoteDescripcion << "\n";

    for ( SMplsL2VFIInfo *i : info.m_lstMplsL2VFIs )
        dbg.space() << i->vfi << i->bridge << i->descripcion << i->lstPWs;

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
