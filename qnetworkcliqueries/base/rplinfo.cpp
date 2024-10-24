#include "rplinfo.h"

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

QDataStream& operator<<(QDataStream& out, const SRplPrefixInfo& data)
{
    out << data.nombre;
    out << data.type;
    out << data.ipversion;
    out << data.lstPrefixes;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SRplPrefixInfo& data)
{
    in >> data.nombre;
    in >> data.type;
    in >> data.ipversion;
    in << data.lstPrefixes;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

QDataStream& operator<<(QDataStream& out, const SRplCommunityInfo &data)
{
    out << data.nombre;
    out << data.lstComunities;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SRplCommunityInfo& data)
{
    in >> data.nombre;
    in >> data.lstComunities;
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

void updateInfoList(QList<SRplPrefixInfo> &lstDest, QList<SRplPrefixInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SRplPrefixInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
            lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SRplPrefixInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SRplPrefixInfo &dest : lstDest )
        {
            if ( origin.nombre == dest.nombre )
            {
                //Si se encontro, actualizamos los datos
                dest.nombre = origin.nombre;
                dest.operativo = true;
                dest.lstPrefixes = origin.lstPrefixes;
                dest.ipversion = origin.ipversion;
                dest.type = origin.type;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

void updateInfoList(QList<SRplCommunityInfo> &lstDest, QList<SRplCommunityInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SRplCommunityInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
            lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SRplCommunityInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SRplCommunityInfo &dest : lstDest )
        {
            if ( origin.nombre == dest.nombre )
            {
                //Si se encontro, actualizamos los datos
                dest.nombre = origin.nombre;
                dest.lstComunities = origin.lstComunities;
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

RplInfo::RplInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
    vrp_xpl_ipv4_trabajado=false;
    _mikrotiksecondquery=false;
}

RplInfo::RplInfo(const RplInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_lstRplRoutes = other.m_lstRplRoutes;
    m_lstRplPrefixes = other.m_lstRplPrefixes;
}

RplInfo::~RplInfo()
{}

void RplInfo::getRplRouteInfo()
{
    if ( m_os == "IOS XR" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_rplRoute()));
        termSendText("show rpl route");        
    }
    else if ( m_os == "VRP" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_rplRoute()));
        termSendText("display current-configuration configuration xpl-filter");
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
    QRegularExpression exp("^(route-policy|xpl route-filter) (\\S+)$");
    QString lastRPL;
    QString rpltxt;
    for (QString line : lines)
    {
        line = line.left( line.size()-1 );
        if ( line.contains(exp,&match) )
        {
            lastRPL = match.captured(2);
            rpltxt.append( line+"\n" );
            continue;
        }

        if ( lastRPL.isEmpty() )
            continue;

        rpltxt.append( line+"\n" );

        if ( line.contains(QRegularExpression("^(end-policy|#)$")) )
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

void RplInfo::getRplPrefixInfo()
{
    if ( m_os == "IOS" || m_os == "IOS NX" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_prefix_list()));
        termSendText("sh run | i prefix-list");
    }
    else if ( m_os == "IOS XR" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_rplPrefix()));
        termSendText("show rpl prefix");
    }
    else if ( m_os == "VRP" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_rplPrefix()));
        termSendText("display current-configuration configuration xpl-pfx");
    }
    else if ( m_os == "RouterOS" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_rplPrefix()));
        termSendText("routing filter rule print\r"); //este es un tipo de rpl route. sacaremos las redes que tengan accept
    }
    else
        finished();
}

void RplInfo::on_term_receiveText_rplPrefix()
{
    txt.append(term->dataReceived());

    // qDebug() << "\n\n\ntxt datareceived" << txt;

    if ( !allTextReceived() )
        return;

    // qDebug() << "RplInfo::on_term_receiveText_rplPrefix()";

    QStringList lines = txt.split("\n");
    QRegularExpression exp("^(xpl ip.+prefix-list|prefix-set) (\\S+)$");
    QString lastName;
    QString lastPrefix;
    QStringList lst;
    short lastVersion = 0;
    SRplPrefixInfo *lastS = nullptr;
    for (QString line : lines)
    {
        line = line.left( line.size()-1 );
        // qDebug() << "line" << line;

        if ( m_brand == "Cisco" || m_os == "VRP" )
        {
            if ( line.contains(exp,&match) )
            {
                lastName = match.captured(2);
                if ( m_os == "VRP" )
                {
                    if ( line.contains( "xpl ipv6-" ) )
                        lastVersion=6;
                    else
                        lastVersion=4;
                }//XR queda con version 0
                continue;
            }

            if ( lastName.isEmpty() )
                continue;

            if ( line.contains(QRegularExpression("^(end-set| end-list)$")) )
            {
                // qDebug() << "Agregar rpl";
                SRplPrefixInfo s;
                s.nombre = lastName;
                s.lstPrefixes = lst;
                s.ipversion = lastVersion;
                s.type = SRplPrefixInfo::Rpl;
                m_lstRplPrefixes.append(s);
                lastName.clear();
                lst.clear();
                continue;
            }

            //convertimos: 172.17.23.0 24 -> 172.17.23.0/24  VRP
            QString p = line.replace(",","").simplified();
            p.replace(QRegularExpression("^(\\S+) (\\d{1,3})(.*)"),
                      "\\1/\\2\\3");

            lst.append( p );
        }
        else if ( m_os == "RouterOS" )
        {
            qDebug() << "line" << line;

            QRegularExpression exp("chain=(\\S+) rule=\"if \\(dst (in|==) (.+)\\) \\{ accept; \\}");
            QRegularExpression exp2("chain=(\\S+) prefix=(\\S+) .+action=accept");
            QString nombre;
            QString prefix;
            if ( line.contains(exp,&match) )
            {
                nombre=match.captured(1);
                prefix=match.captured(3);
            }
            else if ( line.contains(exp2,&match) )
            {
                nombre=match.captured(1);
                prefix=match.captured(2);
            }
            else
            {
                //debido que algunos equipos la linea con esta informacion la parte en dos lineas
                exp.setPattern("chain=(\\S+) ");
                if ( line.contains(exp,&match) ) lastName=match.captured(1);
                exp.setPattern("prefix=(\\S+) ");
                if ( line.contains(exp,&match) ) lastPrefix=match.captured(1);
                if ( line.contains("action=accept") )
                {
                    if ( !lastName.isEmpty() && !lastPrefix.isEmpty() )
                    {
                        nombre=lastName;
                        prefix=lastPrefix;
                        lastName.clear();
                        lastPrefix.clear();
                    }
                }
            }

            qDebug() << "nombre" << nombre << "prefix" << prefix;

            if ( !nombre.isEmpty() && !prefix.isEmpty() )
            {
                bool nuevo=false;
                if ( !lastS )
                    nuevo=true;
                else
                {
                    if ( lastS->nombre != nombre )
                        nuevo=true;
                }

                if ( nuevo )
                {
                    SRplPrefixInfo s;
                    s.nombre = nombre;
                    QStringList lst;
                    lst.append( prefix );
                    s.lstPrefixes = lst;
                    m_lstRplPrefixes.append(s);
                    lastS = &m_lstRplPrefixes.last();
                }
                else
                    lastS->lstPrefixes.append( prefix );
            }
        }
    }

    if ( m_brand == "Cisco" )
        finished();    
    else if ( m_os == "RouterOS" )
    {
        if ( !_mikrotiksecondquery )
        {
            _mikrotiksecondquery=true;
            termSendText("routing filter print\r");
        }
        else
            finished();
    }
    else if ( m_os == "VRP" )
    {
        if ( !vrp_xpl_ipv4_trabajado )
        {
            vrp_xpl_ipv4_trabajado = true;
            termSendText("display current-configuration configuration xpl-pfx6");
        }
        else
        {
            term->disconnectReceiveTextSignalConnections();
            connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_prefix_list()));
            termSendText("display current-configuration | i ip.+-prefix");
        }
    }
}

void RplInfo::on_term_receiveText_prefix_list()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QRegularExpression exp("(list|prefix) (\\S+) (seq|index) \\d+ permit (.+)$");
    QString lastName;
    SRplPrefixInfo *lastS = nullptr;
    for (QString line : lines)
    {
        line = line.left( line.size()-1 );
        // qDebug() << "line2" << line;

        if ( m_brand == "Cisco" || m_os == "VRP" )
        {
            if ( line.contains(exp,&match) )
            {
                QString name = match.captured(2);
                QString prefix = match.captured(4);
                prefix.replace("greater-equal","ge").replace("less-equal","le");
                //convertimos: 172.17.23.0 24 -> 172.17.23.0/24  VRP
                prefix.replace(QRegularExpression("^(\\S+) (\\d{1,3})(.*)"),
                               "\\1/\\2\\3");
                // qDebug() << "entro" << name << lastName;
                if ( lastName != name )
                {
                    //nuevo prefix-list
                    QStringList lst;
                    lst.append(prefix);

                    short version=0;
                    if ( line.contains( "ip ipv6-prefix" ) || line.contains( "ipv6 prefix-list" ) )
                        version=6;
                    else
                        version=4;

                    // qDebug() << "agregando prefixlist" << name << match.captured(4);

                    SRplPrefixInfo s;
                    s.nombre = name;
                    s.lstPrefixes = lst;
                    s.type = SRplPrefixInfo::PrefixList;
                    s.ipversion = version;
                    m_lstRplPrefixes.append(s);
                    lastName = name;
                    lastS = &m_lstRplPrefixes[m_lstRplPrefixes.size()-1];
                    continue;
                }
                else
                {
                    //sigue siendo el mismo prefix-list
                    // qDebug() << "sigue el mismo";
                    lastS->lstPrefixes.append(prefix);
                }
            }
        }
        else if ( m_os == "RouterOS" )
        {

        }
    }

    finished();
}

void RplInfo::getRplCommunityInfo()
{
    if ( m_os == "IOS" || m_os == "IOS NX" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_comunity_list()));
        termSendText("sh run | i community-list");
    }
    else if ( m_os == "IOS XR" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_rplCommunity()));
        termSendText("show rpl community-set");
    }
    else if ( m_os == "VRP" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_rplCommunity()));
        termSendText("display current-configuration configuration xpl-community");
    }
    else
        finished();
}

void RplInfo::on_term_receiveText_rplCommunity()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QString lastName;
    QStringList lst;
    QRegularExpression exp("^(xpl community-list|community-set) (\\S+)$");
    for (QString line : lines)
    {
        line = line.left( line.size()-1 );
        // qDebug() << "line" << line;
        if ( line.contains(exp,&match) )
        {
            lastName = match.captured(2);
            continue;
        }

        if ( lastName.isEmpty() )
            continue;

        if ( line.contains(QRegularExpression("^(end-set| end-list)$")) )
        {
            // qDebug() << "Agregar rpl";
            SRplCommunityInfo s;
            s.nombre = lastName;
            s.lstComunities = lst;
            m_lstRplCommunities.append(s);
            lastName.clear();
            lst.clear();
            continue;
        }

        lst.append( line.replace(",","").simplified() );
    }
    finished();
}

void RplInfo::on_term_receiveText_comunity_list()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QRegularExpression exp("ip community-list expanded (\\S+) permit (.+)$");
    QString lastName;
    SRplCommunityInfo *lastS = nullptr;
    for (QString line : lines)
    {
        line = line.left( line.size()-1 );
        // qDebug() << "line2" << line;
        if ( line.contains(exp,&match) )
        {
            QString name = match.captured(1);
            QString community = match.captured(2);
            // qDebug() << "entro" << name << lastName;
            if ( lastName != name )
            {
                //nuevo community-list
                QStringList lst;
                lst.append(community);

                // qDebug() << "agregando communitylist" << name << match.captured(2);

                SRplCommunityInfo s;
                s.nombre = name;
                s.lstComunities = lst;
                m_lstRplCommunities.append(s);
                lastName = name;
                lastS = &m_lstRplCommunities[m_lstRplCommunities.size()-1];
                continue;
            }
            else
            {
                //sigue siendo el mismo community-list
                // qDebug() << "sigue el mismo";
                lastS->lstComunities.append(community);
            }
        }
    }

    finished();
}

QDataStream& operator<<(QDataStream& out, const RplInfo& info)
{
    out << info.m_lstRplRoutes;
    out << info.m_lstRplPrefixes;
    out << info.m_lstRplCommunities;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, RplInfo& info)
{
    in >> info.m_lstRplRoutes;
    in >> info.m_lstRplPrefixes;
    in >> info.m_lstRplCommunities;
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
    dbg.nospace() << "RplRouteInfo:\n";
    for (SRplRouteInfo i : info.m_lstRplRoutes)
        dbg.space() << "------" << i.nombre << "\n" << i.txt << "\n";

    dbg.nospace() << "RplPrefixInfo:\n";
    for (SRplPrefixInfo i : info.m_lstRplPrefixes)
        dbg.space() << "------" << i.nombre << i.type << i.ipversion << i.lstPrefixes << "\n";

    dbg.nospace() << "RplCommunitiesInfo:\n";
    for (SRplCommunityInfo i : info.m_lstRplCommunities)
        dbg.space() << "------" << i.nombre << i.lstComunities << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
