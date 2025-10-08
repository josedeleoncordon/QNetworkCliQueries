#include "mplsldpinfo.h"
#include "funciones.h"

QDataStream& operator<<(QDataStream& out, const SMplsLdpInfo& data)
{
    out << data.interfaz;
    out << data.neighbor;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMplsLdpInfo& data)
{
    in >> data.interfaz;
    in >> data.neighbor;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SMplsLdpInfo> &lstDest, QList<SMplsLdpInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SMplsLdpInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SMplsLdpInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SMplsLdpInfo &dest : lstDest )
        {
            if ( origin.interfaz == dest.interfaz )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.neighbor = origin.neighbor;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

MplsLdpInfo::MplsLdpInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{}

MplsLdpInfo::MplsLdpInfo(const MplsLdpInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_localID = other.m_localID;
    m_lstMplsLdpInfo = other.m_lstMplsLdpInfo;
    m_queryName = other.m_queryName;
}

MplsLdpInfo::~MplsLdpInfo()
{}

void MplsLdpInfo::getMplsLdpDiscovery()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "MplsLdpInfo::getMplsLdpDiscovery()" << m_brand << "no soportado";
        finished();
        return;
    }

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextDiscovery()));
    termSendText("show mpls ldp discovery");
}

void MplsLdpInfo::getMplsLdpNeighbors()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "MplsLdpInfo::getMplsLdpNeighbors()" << m_brand << "no soportado";
        finished();
        return;
    }

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextNeighbors()));
    termSendText("show mpls ldp neighbor");
}

void MplsLdpInfo::getMplsLdpInterfaces()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "MplsLdpInfo::getMplsLdpInterfaces()" << m_brand << "no soportado";
        finished();
        return;
    }

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextInterfaces()));
    termSendText("show mpls interfaces");
}

void MplsLdpInfo::on_term_receiveTextDiscovery()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    bool reachedLocalID=false;
    bool localid=true;

    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( localid )
        {
            //Local LDP Identifier: 10.192.0.27:0
            exp.setPattern("Local LDP Identifier: (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}):.+");
            if ( line.contains(exp,&match) )
            {
                localid=false;
                m_localID = match.captured(1);
                continue;
            }

            //        Local LDP Identifier:
            //           10.192.33.77:0
            exp.setPattern("Local LDP Identifier:$");
            if ( line.contains(exp,&match) )
            {
                reachedLocalID = true;
                continue;
            }
            exp.setPattern("^(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}):.+");
            if ( reachedLocalID && line.contains(exp,&match) )
            {
                reachedLocalID=false;
                localid=false;
                m_localID = match.captured(1);
                continue;
            }
        }
        else
        {
            //Interfaces, vecinos

            //TODO agregar la ip de los vecinos y la interfaces caidas. Aun no ha sido necesario

            //Interfaces:
            //       Ethernet0/1 (ldp): xmit/recv
            //           LDP Id: 1.1.1.1:0
            //       Ethernet0/2 (ldp): xmit

            //Si la interfaz tiene xmit/recv significa que se esta aprendiendo un vecino por esa interfaz, esta arriba
            //si solo tiene xmit, esta caido, no se aprende un vecino.
            QRegularExpression exp("(([a-zA-Z]|\\-)+\\d+(/\\d+)*(\\.\\d+)*).+xmit/recv");
            if ( line.contains(exp,&match) )
            {
                SMplsLdpInfo sm;
                sm.interfaz = estandarizarInterfaz( match.captured(1) );
                sm.datetime = QDateTime::currentDateTime();
                sm.operativo = true;
                m_lstMplsLdpInfo.append(sm);
            }
        }
    }
    finished();
}

void MplsLdpInfo::on_term_receiveTextNeighbors()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        exp.setPattern("([a-zA-Z]|\\-)+\\d+(/\\d+)*(\\.\\d+)*");
        if ( line.contains(exp,&match) )
        {            
            SMplsLdpInfo sm;
            sm.interfaz = estandarizarInterfaz( match.captured(0) );
            sm.datetime = QDateTime::currentDateTime();
            sm.operativo = true;
            m_lstMplsLdpInfo.append(sm);
        }
    }
    finished();
}

void MplsLdpInfo::on_term_receiveTextInterfaces()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    //TenGigE0/0/0/3/7           Yes      No       No       Yes

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( !line.contains("Yes") )
            continue;

        SMplsLdpInfo sm;
        sm.interfaz = estandarizarInterfaz( line.split(" ",Qt::SkipEmptyParts).at(0) );
        sm.datetime = QDateTime::currentDateTime();
        sm.operativo = true;
        m_lstMplsLdpInfo.append(sm);
    }
    finished();
}

QDataStream& operator<<(QDataStream& out, const MplsLdpInfo& info)
{
    out << info.m_localID;
    out << info.m_lstMplsLdpInfo;
    out << info.m_queryoption;
    out << info.m_queryName;
    return out;
}

QDataStream& operator>>(QDataStream& in, MplsLdpInfo& info)
{
    in >> info.m_localID;
    in >> info.m_lstMplsLdpInfo;
    in >> info.m_queryoption;
    in >> info.m_queryName;
    return in;
}

QDataStream& operator<<(QDataStream& out, const MplsLdpInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, MplsLdpInfo*& info)
{
    info = new MplsLdpInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const MplsLdpInfo &info)
{
    dbg.nospace() << "MplsLdpInfo:\n";

    if ( !info.m_localID.isEmpty() )
        dbg.space() << "Local ID:" << info.m_localID;

    for (SMplsLdpInfo i : info.m_lstMplsLdpInfo)
        dbg.space() << i.interfaz << i.datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
