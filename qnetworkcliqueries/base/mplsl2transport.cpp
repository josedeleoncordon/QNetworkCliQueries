#include "mplsl2transport.h"
#include "queries.h"

QDataStream& operator<<(QDataStream& out, const SMplsL2PWInfo& data)
{
    out << data.pwid;
    out << data.neighbor;
    out << data.state;
    out << data.preferredPath;
    out << data.remoteDescripcion;
    out << data.source;
    out << data.neighborbk;
    out << data.pwidbk;
    out << data.statebk;
    out << data.srvid;
    out << data.sdpprimary;
    out << data.xc;
    out << data.descripcion;
    out << data.AC;
    out << data.ACXEInterfaz;
    out << data.ACXEServiceInstance;
    out << data.mtu;
    out << data.pwclass;
    out << data.pwclassbck;
    out << data.group;
    out << data.controlword;
    out << data.controlwordbck;
    out << data.fat;
    out << data.fatbck;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMplsL2PWInfo& data)
{
    in >> data.pwid;
    in >> data.neighbor;
    in >> data.state;
    in >> data.preferredPath;
    in >> data.remoteDescripcion;
    in >> data.source;
    in >> data.neighborbk;
    in >> data.pwidbk;
    in >> data.statebk;
    in >> data.srvid;
    in >> data.sdpprimary;
    in >> data.xc;
    in >> data.descripcion;
    in >> data.AC;
    in >> data.ACXEInterfaz;
    in >> data.ACXEServiceInstance;
    in >> data.mtu;
    in >> data.pwclass;
    in >> data.pwclassbck;
    in >> data.group;
    in >> data.controlword;
    in >> data.controlwordbck;
    in >> data.fat;
    in >> data.fatbck;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

QDebug operator<<(QDebug dbg, const SMplsL2PWInfo &info)
{
    dbg.space() << "(" << info.pwid << info.source << info.neighbor << info.state << info.mtu
                << info.preferredPath << info.remoteDescripcion << info.neighborbk << info.pwidbk << info.statebk << ")";
    return dbg.maybeSpace();
}

QDataStream& operator<<(QDataStream& out, const SMplsL2VFIInfo& data)
{
    out << data.vfi;
    out << data.ACvfi;
    out << data.descripcion;
    out << data.bridge;
    out << data.lstPWs;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMplsL2VFIInfo& data)
{
    in >> data.vfi;
    in >> data.ACvfi;
    in >> data.descripcion;
    in >> data.bridge;
    in >> data.lstPWs;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SMplsL2PWInfo> &lstDest, QList<SMplsL2PWInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SMplsL2PWInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SMplsL2PWInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SMplsL2PWInfo &dest : lstDest )
        {
            if ( origin.xc == dest.xc )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.pwid = origin.pwid;
                dest.AC = origin.AC;
                dest.ACXEInterfaz = origin.ACXEInterfaz;
                dest.ACXEServiceInstance = origin.ACXEServiceInstance;
                dest.neighbor = origin.neighbor;
                dest.neighborbk = origin.neighborbk;
                dest.pwidbk = origin.pwidbk;
                dest.preferredPath = origin.preferredPath;
                dest.remoteDescripcion = origin.remoteDescripcion;
                dest.state = origin.state;
                dest.statebk = origin.statebk;
                dest.descripcion = origin.descripcion;
                dest.source = origin.source;
                dest.srvid = origin.srvid;
                dest.sdpprimary = origin.sdpprimary;
                dest.xc = origin.xc;
                dest.mtu = origin.mtu;
                dest.group = origin.group;
                dest.pwclass = origin.pwclass;
                dest.pwclassbck = origin.pwclassbck;
                dest.controlword = origin.controlword;
                dest.controlwordbck = origin.controlwordbck;
                dest.fat = origin.fat;
                dest.fatbck = origin.fatbck;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

void updateInfoList(QList<SMplsL2VFIInfo> &lstDest, QList<SMplsL2VFIInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SMplsL2VFIInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SMplsL2VFIInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SMplsL2VFIInfo &dest : lstDest )
        {
            if ( origin.vfi == dest.vfi )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.vfi = origin.vfi;
                dest.ACvfi = origin.ACvfi;
                dest.descripcion = origin.descripcion;
                dest.bridge = origin.bridge;
                dest.lstPWs = origin.lstPWs;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

MplsL2TransportInfo::MplsL2TransportInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
}

MplsL2TransportInfo::MplsL2TransportInfo(const MplsL2TransportInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_lstMplsL2PWs = other.m_lstMplsL2PWs;
    m_lstMplsL2VFIs = other.m_lstMplsL2VFIs;
    m_queryName = other.m_queryName;
}

MplsL2TransportInfo::~MplsL2TransportInfo()
{}

void MplsL2TransportInfo::getMplsL2Transport()
{
    if ( m_brand == "Cisco" )
    {
        if ( m_os == "IOS XR" )
        {
            connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_XR_BD()));
            termSendText("sh l2vpn bridge-domain detail | i \"Bridge group|Description|AC:|PWs:|VFI|PW: neighbor|Interface|Preferred|Source|Control word\"");
        }
        else
        {
            connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_IOS_VFI()));
            termSendText("show vfi");
        }
    }
    else if ( m_brand == "Huawei" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_VRP_VSI()));
        termSendText("display vsi peer-info");
    }
    else if ( m_brand == "Nokia" )
    {
        index_nokia_services.clear();
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_SR_OS_SAP_USING()));
        termSendText("show service sap-using");
    }
    else
    {
        qDebug() << "MplsL2TransportInfo::getMplsL2Transport()" << m_brand << "no soportado";
        finished();
        return;
    }
}

void MplsL2TransportInfo::on_term_receiveText_IOS_VFI()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SMplsL2VFIInfo *mvi = nullptr;
    foreach (QString line, lines)
    {
        line = line.simplified();

//        VFI name: L2-CELSIA_TORRE_ARGOS, state: up, type: multipoint, signaling: LDP
//        VPN ID: 3745
//        Bridge-Domain 3745 attachment circuits:
//        Neighbors connected via pseudowires:
//        Peer Address     VC ID        S
//        172.17.27.8      57374502     Y
//        172.17.27.9      57374501     Y

        QRegularExpression exp("VFI name: (\\S+), state: \\S+,");
        if ( line.contains(exp,&match) )
        {
            SMplsL2VFIInfo i;
            i.vfi = match.captured( 1 );
            m_lstMplsL2VFIs.append( i );
            mvi = &m_lstMplsL2VFIs[m_lstMplsL2VFIs.size()-1];
        }

        if ( !mvi )
            continue;

        exp.setPattern("Bridge-Domain (\\S+) attachment");
        if ( line.contains(exp,&match) )
        {
            mvi->bridge = match.captured(1);
            continue;
        }

        exp.setPattern("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) (\\d+) \\S");
        if ( line.contains(exp,&match) )
        {
            SMplsL2PWInfo i;
            i.pwid = match.captured(2);

            i.source = m_parentQuery->ip();
            i.neighbor = match.captured(1);
            mvi->lstPWs.append( i );
            continue;
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_IOS_Xconnect()));
    termSendText("sh xconnect all");
}

void MplsL2TransportInfo::on_term_receiveText_IOS_Xconnect()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QString ac;
    QString acxeinterface;
    QString acxeserviceinstance;
    QString nei;
    QString vfi;
    QString pwid;
    QString state;
    QRegularExpression exp("\\S+ \\S+ +ac (\\S+):(\\d+)\\(.+ (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}):(\\d+) +(\\S+)$");
    QRegularExpression expvfi(".+ vfi (\\S+) .+ (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}):(\\d+) +(\\S+)$");
    QRegularExpressionMatch match;

    QStringList lines = txt.split("\n");
    for ( int c=0; c<lines.size(); c++ )
    {
        QString line = lines.at(c);
        line = line.mid(0,line.size()-1);

        //UP pri ac Te0/0/4:307(Eth VLAN) UP mpls 172.17.22.79:502307 UP
        if ( line.contains(exp,&match) )
        {
            ac = match.captured(1)+":"+match.captured(2);
            acxeinterface = match.captured(1);
            acxeserviceinstance = match.captured(2);
            nei = match.captured(3);
            pwid = match.captured(4);
            state = match.captured(5);
            SMplsL2PWInfo* pw = buscarPW(ac);
            if (!pw)
            {
                //nuevo
                SMplsL2PWInfo i;
                i.AC = ac;
                i.ACXEInterfaz = acxeinterface;
                i.ACXEServiceInstance = acxeserviceinstance;
                i.source = m_parentQuery->ip();
                i.neighbor=nei;
                i.pwid=pwid;
                i.state=state;
                m_lstMplsL2PWs.append(i);
            }
            else
            {
                //Agregando peerbck
                //UP pri   ac Te0/0/26:1193(Eth VLAN)      UP mpls 172.16.30.240:503197201      UP
                //IA sec   ac Te0/0/26:1193(Eth VLAN)      UP mpls 172.16.30.35:503197202       SB
                pw->neighborbk = nei;
                pw->statebk = state;
                pw->pwidbk = pwid;
            }
        }
        else if ( line.contains(expvfi,&match) )
        {
            // qDebug() << "IOS VFI LINE" << q->ip() << line;
            // qDebug() << lastAC << lastNeighbor << lastPW << lastState;
            // qDebug() << match.captured(1) << match.captured(2) << match.captured(3) << match.captured(4);

            //VFIs
            //UP pri  vfi GBM-BGP-4095                 UP mpls 172.16.30.250:502194501      UP
            //UP pri  vfi GBM-BGP-4095                 UP mpls 172.19.8.9:502409501         UP
            //UP pri  vfi GBM-BGP-CUSTOMERS-4094       UP mpls 172.16.30.250:502409401      UP

            vfi = match.captured(1);
            nei = match.captured(2);
            pwid = match.captured(3);
            state = match.captured(4);
            SMplsL2PWInfo *pw = buscarVFI_PW(vfi,nei,pwid);
            if (pw)
                pw->state=state;
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_IOS_L2Transport()));
    termSendText("sh mpls l2transport vc detail | i Local interface|Destination|Preferred|Remote interface|MTU|Control Word");
}

void MplsL2TransportInfo::on_term_receiveText_IOS_L2Transport()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QString lastVFI;
    SMplsL2PWInfo *pw = nullptr;
    foreach (QString line, lines)
    {
        line = line.simplified();

//        Local interface: Te0/0/4 up, line protocol up, Eth VLAN 2500 up
//          Destination address: 172.16.30.11, VC ID: 57250001, VC status: up
//            Preferred path: Tunnel5701,  active
//            Remote interface description: COUS09820001|PGM|CALLZILLA_BOGOTA

        exp.setPattern("Local interface: VFI (\\S+) vfi");
        if (line.contains(exp,&match))
        {
            lastVFI=match.captured(1);
            continue;
        }
        exp.setPattern("Local interface: .+, line protocol");
        if (line.contains(exp))
        {
            lastVFI.clear();
            continue;
        }

        exp.setPattern("Destination address: (\\S+), VC ID: (\\d+), VC status:");
        if ( line.contains(exp,&match) )
        {
            if (lastVFI.isEmpty())
                //xc
                pw = buscarPW(match.captured(1),match.captured(2));
            else
                //vfi
                pw = buscarVFI_PW(lastVFI,match.captured(1),match.captured(2));
            continue;
        }

        if (!pw)
            continue;

        exp.setPattern("Preferred path: (\\S+), \\S+");
        if ( line.contains(exp,&match) )
        {
            pw->preferredPath = match.captured(1);
            continue;
        }
        exp.setPattern("Remote interface description: (.+)$");
        if ( line.contains(exp,&match) )
        {
            pw->remoteDescripcion = match.captured(1);
            continue;
        }
        exp.setPattern("MTU: local (\\d+)");
        if ( line.contains(exp,&match) )
        {
            pw->mtu = match.captured(1);
            continue;
        }
        exp.setPattern("Control Word: (\\S+)( |$)");
        if ( line.contains(exp,&match) )
        {
            bool cw = false;
            if ( match.captured(1) == "On") cw=true;
            pw->controlword = cw;
        }
    }

    finished();

    //terminar on_term_receiveText_IOS_runInterfaces()
    //    term->disconnectReceiveTextSignalConnections();
    //    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_IOS_runInterfaces()));
    //    nextIOSrunInterfaces();
}

void MplsL2TransportInfo::nextIOSrunInterfaces()
{
    if ( !lstIosInterfaces.isEmpty() )
    {
        QString interface = lstIosInterfaces.takeFirst();
        termSendText("sh run interface "+interface);
    }
    else
        finished();
}

void MplsL2TransportInfo::on_term_receiveText_IOS_runInterfaces()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QString lastInterface;
    QString lastServiceInstance;

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        QRegularExpression exp(QRegularExpression::anchoredPattern("^interface (.+)$"));
        QRegularExpressionMatch match = exp.match(line);
        if ( match.hasMatch() )
            lastInterface = estandarizarInterfaz(match.captured(1));
    }

    nextIOSrunInterfaces();
}

void MplsL2TransportInfo::on_term_receiveText_XR_BD()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SMplsL2VFIInfo *mvi = nullptr;
    SMplsL2PWInfo *pw = nullptr;
    foreach (QString line, lines)
    {
        line = line.simplified();

//    Bridge group: TRUNKS, bridge-domain: VLAN1885, id: 37, state: up, ShgId: 0, MSTi: 0
//     Description: CRCRCAB10039792I|CABLE_CENTRO|CIUDAD_NEILY
//      ACs: 1 (1 up), VFIs: 1, PWs: 2 (1 up), PBBs: 0 (0 up), VNIs: 0 (0 up)
//        AC: BVI1885, state is up
//          Type Routed-Interface
//      List of Access PWs:
//      List of VFIs:
//        VFI VLAN1885 (up)
//          PW: neighbor 172.19.24.53, PW ID 506188523, state is up ( established )
//            Source address 172.19.16.42
//            Preferred path Active : tunnel-te50601, Statically configured, fallback enabled
//              Interface    VLAN1885                       CRCRCAB10039792I|CABLE_CENTRO|CIUDAD_NEILY
//              Control word disabled                       unknown
//          PW: neighbor 172.19.24.88, PW ID 506188523, state is down ( all ready ) (Segment-down)
//              Interface    VLAN1885                       CRCRCAB10039792I|CABLE_CENTRO|CIUDAD_NEILY
//              Control word disabled                       unknown
//          VFI Statistics:
//      List of Access VFIs:

        QRegularExpression exp("Bridge group: \\S+, bridge-domain: (\\S+),");
        if ( line.contains(exp,&match) )
        {
            SMplsL2VFIInfo i;
            i.bridge = match.captured(1);
            m_lstMplsL2VFIs.append(i);
            mvi = &m_lstMplsL2VFIs.last();
            continue;
        }

        if ( !mvi )
            continue;

        exp.setPattern("Description: (\\S+)$");
        if ( line.contains(exp,&match) )
        {
            mvi->descripcion = match.captured(1);
            continue;
        }

        exp.setPattern("AC: (\\S+), state");
        if ( line.contains(exp,&match) )
        {
            mvi->ACvfi = match.captured(1);
            continue;
        }

        exp.setPattern("VFI (\\S+) ");
        if ( line.contains(exp,&match) )
        {
            mvi->vfi = match.captured(1);
            continue;
        }

        exp.setPattern("PW: neighbor (\\S+), PW ID (\\d+), state is (\\S+) ");
        if ( line.contains(exp,&match) )
        {
            SMplsL2PWInfo i;
            i.neighbor = match.captured(1);
            i.pwid = match.captured(2);
            i.state = match.captured(3);
            mvi->lstPWs.append(i);
            pw = &mvi->lstPWs.last();
        }

        exp.setPattern("Source address (\\S+)");
        if ( line.contains(exp,&match) )
        {
            if (pw) pw->source = match.captured(1);
            continue;
        }

        exp.setPattern("Preferred path Active : (\\S+),");
        if ( line.contains(exp,&match) )
        {
            if ( pw ) pw->preferredPath = match.captured(1);
            continue;
        }

        exp.setPattern("Interface \\S+ (\\S+)");
        if ( line.contains(exp,&match) )
        {
            if (pw) pw->remoteDescripcion = match.captured(1);
            continue;
        }

        exp.setPattern("Control word (\\S+) ");
        if ( line.contains(exp,&match) )
        {
            if (match.captured(1) == "enabled")
                if (pw) pw->controlword=true;
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_XR_Xconnect()));
    termSendText("sh l2vpn xconnect detail | i \"Group|AC:|PW:|EVPN:|Backup PW:|Interface|Source|MTU|PW class|Control word|Flow Label\"");
}

void MplsL2TransportInfo::on_term_receiveText_XR_Xconnect()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SMplsL2PWInfo *xi = nullptr;
    bool bk=false;
    for (QString line : lines)
    {
        line = line.simplified();

        //Group GT00040003, XC GT00040003, state is up; Interworking none
        //  AC: TenGigE0/0/0/10.50, state is up
        //  PW: neighbor 172.16.30.250, PW ID 502157751, state is up ( established )
        //    Source address 172.17.22.132
        //      Group ID     0x3c0000a8                     0xb
        //      Interface    TenGigE0/0/0/10.50             VLAN50
        //  Backup PW:
        //  PW: neighbor 172.16.30.35, PW ID 502157751, state is standby ( all ready )
        //    Source address 172.17.22.132
        //      Group ID     0x3c0000a8                     0x6
        //      Interface    TenGigE0/0/0/10.50             VLAN50
        //      MTU          9198                           9198

        QRegularExpression exp("Group (\\S+), XC (\\S+), state");
        if ( line.contains(exp,&match) )
        {
            SMplsL2PWInfo i;
            i.group = match.captured(1);
            i.xc = match.captured(2);
            i.source = m_parentQuery->ip();
            m_lstMplsL2PWs.append(i);
            xi = &m_lstMplsL2PWs[ m_lstMplsL2PWs.size()-1 ];
            bk=false;
            continue;
        }

        if ( !xi  )
            continue;

        exp.setPattern("^Backup PW");
        if ( line.contains(exp) )
        {
            bk=true;
            continue;
        }

        exp.setPattern("AC: (\\S+), state");
        if ( line.contains(exp,&match) )
        {
            xi->AC = match.captured(1);
            continue;
        }

        exp.setPattern("(PW|EVPN): neighbor (\\S+), PW ID:* (.+), state is (\\S+) ");
        if ( line.contains(exp,&match) )
        {
            QString state=match.captured(4);
            if ( state == "up" )
                state = "UP";
            else if ( state == "down" )
                state = "DN";
            else if ( state == "standby" )
                state = "SB";
            if (!bk)
            {
                xi->neighbor = match.captured(2);
                xi->pwid = match.captured(3);
                xi->state = state;
            }
            else
            {
                xi->neighborbk = match.captured(2);
                xi->pwidbk = match.captured(3);
                xi->statebk = state;
            }
            continue;
        }

        exp.setPattern("Source address (\\S+)");
        if ( line.contains(exp,&match) )
        {
            xi->source = match.captured(1);
            continue;
        }

        exp.setPattern("Interface \\S+ (\\S+)");
        if ( line.contains(exp,&match) )
        {
            //agregar remoteDescripcion bk
            xi->remoteDescripcion = match.captured(1);
            continue;
        }

        if (line.contains("MTU "))
        {
            QStringList data = line.split(" ",Qt::SkipEmptyParts);
            if (data.size() == 3)
                xi->mtu = data.at(1);
            continue;
        }

        exp.setPattern("PW class (.+), ");
        if (line.contains(exp,&match))
        {
            QString pwclass = match.captured(1);
            if (pwclass == "not set") pwclass.clear();
            if (!bk)
                xi->pwclass=pwclass;
            else
                xi->pwclassbck=pwclass;
            continue;
        }

        exp.setPattern("Control word (\\S+) ");
        if ( line.contains(exp,&match) )
        {
            if (match.captured(1) == "enabled")
            {
                if (!bk)
                    xi->controlword=true;
                else
                    xi->controlwordbck=true;
            }
        }

        if ( line.contains("Flow Label flags configured (Tx=1,Rx=1)") )
        {
            if (!bk)
                xi->fat=true;
            else
                xi->fatbck=true;
        }
    }
    finished();
}

void MplsL2TransportInfo::on_term_receiveText_VRP_VSI()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SMplsL2VFIInfo *mvi = nullptr;
    bool agregarPrincipal=false;
    bool agregarBk=false;
    foreach (QString line, lines)
    {
        line = line.simplified();

        //display vsi peer-info
        //
        //VSI Name: BRBRESP10097311C                            Signaling: ldp
        //--------------------------------------------------------------------
        //Peer                Transport  Local      Remote      VC
        //Addr                VC ID      VC Label   VC Label    State
        //--------------------------------------------------------------------
        //172.16.31.129       55248903   49804      18481       up
        //172.17.38.18        55248903   49801      48065       backup

        QRegularExpression exp("VSI Name: (\\S+) ");
        if ( line.contains(exp,&match) )
        {
            SMplsL2VFIInfo i;
            i.vfi = match.captured( 1 );
            m_lstMplsL2VFIs.append( i );
            mvi = &m_lstMplsL2VFIs[m_lstMplsL2VFIs.size()-1];
            agregarPrincipal=false;
            agregarBk=false;
        }

        if ( !mvi )
            continue;

        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3} ");
        if ( line.contains(exp,&match) )
        {
            QStringList data = line.split(" ",Qt::SkipEmptyParts);
            if ( data.size() < 5 )
                continue;

            QString state = data.at(4);
            if ( state == "up" )
                state = "UP";
            else if ( state == "down" )
                state = "DN";
            else if ( state == "backup" )
                state = "SB";

            //las dos primeras lineas las tomamos como principal y bck
            //si hubiera mas como pw adicionales
            //(esto no tendria que suceder en los equipos remotos)
            if (mvi->lstPWs.isEmpty())
            {
                if (state=="SB")
                {
                    SMplsL2PWInfo i;
                    i.neighborbk = data.at(0);
                    i.source = m_parentQuery->ip();
                    i.pwidbk = data.at(1);
                    i.statebk = state;
                    mvi->lstPWs.append( i );
                    agregarPrincipal=true;
                }
                else
                {
                    SMplsL2PWInfo i;
                    i.neighbor = data.at(0);
                    i.source = m_parentQuery->ip();
                    i.pwid = data.at(1);
                    i.state = state;
                    mvi->lstPWs.append( i );
                    agregarBk=true;
                }
            }
            else
            {
                if (agregarPrincipal)
                {
                    SMplsL2PWInfo &i = mvi->lstPWs[0];
                    i.neighbor = data.at(0);
                    i.source = m_parentQuery->ip();
                    i.pwid = data.at(1);
                    i.state = state;
                    agregarPrincipal=false;
                }
                else if (agregarBk)
                {
                    SMplsL2PWInfo &i = mvi->lstPWs[0];
                    i.neighborbk = data.at(0);
                    i.source = m_parentQuery->ip();
                    i.pwidbk = data.at(1);
                    i.statebk = state;
                    agregarBk=false;
                }
                else
                {
                    SMplsL2PWInfo i;
                    i.neighbor = data.at(0);
                    i.source = m_parentQuery->ip();
                    i.pwid = data.at(1);
                    i.state = state;
                    mvi->lstPWs.append( i );
                }
            }
            continue;
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_VRP_VSI_SERVICES_ALL()));
    termSendText("display vsi services all");
}

void MplsL2TransportInfo::on_term_receiveText_VRP_VSI_SERVICES_ALL()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    //asociamos la vsi con el bridge o ac
    //Virtual-Ethernet1/0/1.1731          COCOINT10114314I                up    up
    //Virtual-Ethernet1/0/1.1732          COCOECO10119621I                up    up
    //Virtual-Ethernet1/0/1.1733          COCOINV10119506I                up    up
    //Bridge-domain 3989                  COCOINT10075827I                --    --
    //Bridge-domain 3990                  COCOINT10076499I                --    --
    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        //reemplazamos la lineas de Bridge-domain 3990 por Vbdif
        // QRegularExpression expbridge("Bridge-domain (\\d+)");
        // line.replace(expbridge,"Vbdif\\1");

        QStringList data=line.split(" ",Qt::SkipEmptyParts);
        if ( data.size() < 4 )
            continue;

        for ( SMplsL2VFIInfo& i : m_lstMplsL2VFIs )
        {
            if ( i.vfi == data.at(1) )
            {
                i.ACvfi=estandarizarInterfaz(data.at(0));
                break;
            }
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_VRP_L2Transport()));
    termSendText("display mpls l2vc");
}

void MplsL2TransportInfo::on_term_receiveText_VRP_L2Transport()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QString lastAc;
    QString lastNeighbor;
    QString lastPW;
    QString lastState;
    QString lastPWclass;
    bool lastControlWord = false;
    SMplsL2PWInfo *lastSpwremoto = nullptr;
    foreach (QString line, lines)
    {
        line = line.simplified();

//        *Client Interface     : 100GE0/1/48.1533
//        Administrator PW     : no
//        AC status            : up
//        VC state             : up
//        Label state          : 0
//        Token state          : 0
//        VC ID                : 55128910
//        VC Type              : Ethernet
//        session state        : up
//        Destination          : 172.16.31.129
//        link state           : up
//		  local VC MTU         : 9206         remote VC MTU        : 9206

        exp.setPattern("\\*client interface : (\\S+) ");
        if (line.contains(exp,&match))
        {
            if ( lastAc != match.captured(1) )
            {
                lastAc = match.captured(1);
                SMplsL2PWInfo i;
                i.AC = lastAc;
                m_lstMplsL2PWs.append(i);
                lastSpwremoto = &m_lstMplsL2PWs[m_lstMplsL2PWs.size()-1];

                lastNeighbor.clear();
                lastPW.clear();
                lastState.clear();
                lastControlWord=false;
                lastPWclass.clear();
            }
            continue;
        }

        exp.setPattern("local status code : (\\S+)");
        if (line.contains(exp,&match))
        {
            QString state = match.captured(1);
            if (state=="0x0") state="UP"; else if (state=="0x1") state="DN"; else if (state=="0x20") state="SB";
            lastState = state;
            continue;
        }

        exp.setPattern("VC ID : (\\S+)");
        if (line.contains(exp,&match))
        {
            lastPW = match.captured(1);
            continue;
        }

        exp.setPattern("destination : (\\S+)");
        if (line.contains(exp,&match))
        {
            lastNeighbor = match.captured(1);
            continue;
        }

        exp.setPattern("local VC MTU : (\\d+)( |$)");
        if (line.contains(exp,&match))
        {
            lastSpwremoto->mtu = match.captured(1);
            continue;
        }

        exp.setPattern("PW template name : (\\S+)");
        if (line.contains(exp,&match))
        {
            lastPWclass = match.captured(1);
            if (lastPWclass == "--") lastPWclass.clear();
            continue;
        }

        exp.setPattern("control word : (\\S+)");
        if (line.contains(exp,&match))
        {
            if (match.captured(1) == "enable")
                lastControlWord=true;
            continue;
        }

        exp.setPattern("primary or secondary : (\\S+)");
        if (line.contains(exp,&match))
        {
            if (match.captured(1) == "primary")
            {
                lastSpwremoto->neighbor = lastNeighbor;
                lastSpwremoto->pwid = lastPW;
                lastSpwremoto->state = lastState;
                lastSpwremoto->controlword = lastControlWord;
                lastSpwremoto->pwclass = lastPWclass;
            }
            else
            {
                lastSpwremoto->neighborbk = lastNeighbor;
                lastSpwremoto->pwidbk = lastPW;
                lastSpwremoto->statebk = lastState;
                lastSpwremoto->controlwordbck = lastControlWord;
                lastSpwremoto->pwclassbck = lastPWclass;
            }

            continue;
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_VRP_BGP_EVPN_EVPL()));
    termSendText("display bgp evpn evpl | i \"EVPL ID|State|Interface|Peer IP\"");
}

void MplsL2TransportInfo::on_term_receiveText_VRP_BGP_EVPN_EVPL()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SMplsL2PWInfo *xi = nullptr;
    foreach (QString line, lines)
    {
        line = line.simplified();

        //EVPL ID : 235
        //State : up
        //Interface : Eth-Trunk23.235
        //Interface Status : up
        //Ignore AcState : disable
        //Local DF State : primary(-)
        //Peer IP : 172.17.33.251
        // DF State : primary
        //Last Interface UP Timestamp: 2026-01-23 20:17:02-05:00

        QRegularExpression exp("^EVPL ID : (.*)$");
        if ( line.contains(exp,&match) )
        {
            SMplsL2PWInfo i;
            i.xc = match.captured(1);

            //suponemos que el local-id y remote-id son iguales a la instancia
            //evpl instance 1286
            // evpn binding vpn-instance ECECVAS10330963C
            // local-service-id 1286 remote-service-id 1286
            i.pwid = "evi "+i.xc+", ac-id "+i.xc;

            i.source = m_parentQuery->ip();
            m_lstMplsL2PWs.append(i);
            xi = &m_lstMplsL2PWs.last();
            continue;
        }

        if ( !xi  )
            continue;

        exp.setPattern("^State : (.+)$");
        if ( line.contains(exp,&match) )
        {
            xi->state = match.captured(1);
            continue;
        }

        exp.setPattern("^Interface : (.+)$");
        if ( line.contains(exp,&match) )
        {
            xi->AC = match.captured(1);
            continue;
        }

        exp.setPattern("^Peer IP : (.+)$");
        if ( line.contains(exp,&match) )
        {
            xi->neighbor = match.captured(1);
            continue;
        }
    }

    finished();
}

void MplsL2TransportInfo::on_term_receiveText_SR_OS_SAP_USING()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    bool ok=false;
    for (QString line: lines)
    {
        //-------------------------------------------------------------------------------
        //1/1/c28/1:18                    18         1     none          none   Up   Up
        //1/1/c28/1:20                    20         1     none          none   Up   Up
        //1/1/c28/1:21                    21         1     none          none   Up   Up
        //-------------------------------------------------------------------------------
        line = removeEOL(line);
        if (line.contains("------------------------------------"))
        {
            ok=!ok;
            continue;
        }
        if (!ok) continue;

        QStringList data=line.split(" ",Qt::SkipEmptyParts);
        if (data.size()<7)
            continue;

        SMplsL2PWInfo i;
        i.AC = data.at(0);
        i.srvid = data.at(1);
        m_lstMplsL2PWs.append(i);
    }

    //creamos index de srvid para busqueda rapida para las siguientes consultas
    for (int i = 0; i < m_lstMplsL2PWs.size(); i++) {
        index_nokia_services[m_lstMplsL2PWs[i].srvid] = i;
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_SR_OS_info_service_epipe()));
    termSendText("info full-context | match 'service epipe.+(service-id|endpoint precedence)'");
}

void MplsL2TransportInfo::on_term_receiveText_SR_OS_info_service_epipe()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QString lastSrvid;
    QRegularExpression exp;
    QRegularExpressionMatch match;
    for (QString line: lines)
    {
        ///configure service epipe "COCOS3S10263096C-852" service-id 852
        ///configure service epipe "COCOS3S10263096C-852" spoke-sdp 9200:57085201 endpoint precedence primary
        ///configure service epipe "COCOS3S10263096C-852" spoke-sdp 28216:57085201 endpoint precedence 2
        line = removeEOL(line);
        exp.setPattern("service epipe (\\S+) service-id (\\d+)");
        if (line.contains(exp,&match))
        {
            lastSrvid=match.captured(2);
            if (index_nokia_services.contains(lastSrvid))
                m_lstMplsL2PWs[index_nokia_services[lastSrvid]].xc = match.captured(1).replace("\"","");
            continue;
        }
        if (lastSrvid.isEmpty()) continue;

        exp.setPattern("spoke-sdp (\\S+) endpoint precedence primary");
        if (line.contains(exp,&match))
        {
            if (index_nokia_services.contains(lastSrvid))
                m_lstMplsL2PWs[index_nokia_services[lastSrvid]].sdpprimary = match.captured(1);
            lastSrvid.clear();
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_SR_OS_SDP_USING()));
    termSendText("show service sdp-using");
}

void MplsL2TransportInfo::on_term_receiveText_SR_OS_SDP_USING()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    bool ok=false;
    for (QString line: lines)
    {
        //-------------------------------------------------------------------------------
        //18         9109:57001823      Spok   172.17.49.109        Up    1048574 49061
        //18         28209:57001823     Spok   172.17.28.209        Up    1047983 28479
        //20         28227:10247128     Spok   172.17.28.227        Up    1047949 77468
        //21         28227:10247188     Spok   172.17.28.227        Up    1047948 74397
        //-------------------------------------------------------------------------------
        line = removeEOL(line);
        if (line.contains("------------------------------------"))
        {
            ok=!ok;
            continue;
        }
        if (!ok) continue;

        QStringList data=line.split(" ",Qt::SkipEmptyParts);
        if (data.size()<7)
            continue;

        //si es el mismo srvid asumimos que la primera es el principal y el segundo el bck
        QString srvid = data.at(0);
        QString sdp = data.at(1);
        QString neighbor = data.at(3);
        QString state = data.at(4);
        if (state=="Up")
            state="UP";
        else if (state=="Dn" || state=="Down")
            state="DN";
        QString pwid;
        if (sdp.contains(":"))
            pwid = sdp.split(":",Qt::SkipEmptyParts).at(1);

        qDebug() << "line 33" << srvid << sdp << neighbor << state;

        if (index_nokia_services.contains(srvid))
        {
            qDebug() << "index_nokia_services[srvid]" << index_nokia_services[srvid];
            SMplsL2PWInfo &i = m_lstMplsL2PWs[index_nokia_services[srvid]];
            if (i.sdpprimary.isEmpty())
            {
                //no hay configuracion de precedence primary los almacenamos en el orden en el que aparece
                if (i.pwid.isEmpty())
                {
                    qDebug() << "agregando principal" << i.srvid;
                    i.pwid = pwid;
                    i.neighbor = neighbor;
                    i.state = state;
                }
                else
                {
                    qDebug() << "agregando bck" << i.srvid;
                    if (state=="UP")
                        state="SB";
                    i.pwidbk = pwid;
                    i.neighborbk = neighbor;
                    i.statebk = state;
                }
            }
            else
            {
                //precedence configurado
                if (sdp == i.sdpprimary)
                {
                    qDebug() << "agregando principal 2" << i.srvid;
                    i.pwid = pwid;
                    i.neighbor = neighbor;
                    i.state = state;
                }
                else
                {
                    qDebug() << "agregando bck 2" << i.srvid;
                    if (state=="UP")
                        state="SB";
                    i.pwidbk = pwid;
                    i.neighborbk = neighbor;
                    i.statebk = state;
                }
            }
        }
    }

    finished();
}

SMplsL2PWInfo *MplsL2TransportInfo::buscarVFI_PW(QString vfi, QString destino, QString vcid)
{
    for ( SMplsL2VFIInfo &i : m_lstMplsL2VFIs )
        if ( i.vfi == vfi )
        {
            for (SMplsL2PWInfo &p : i.lstPWs)
                if (p.pwid == vcid && p.neighbor == destino)
                    return &p;
            break;
        }
    return nullptr;
}

SMplsL2VFIInfo *MplsL2TransportInfo::buscarVFI(QString vfi)
{
    for ( SMplsL2VFIInfo &i : m_lstMplsL2VFIs )
        if ( i.vfi == vfi  )
            return &i;
    return nullptr;
}

SMplsL2PWInfo *MplsL2TransportInfo::buscarPW(QString destino, QString vcid)
{
    for (SMplsL2PWInfo &i : m_lstMplsL2PWs)
        if (i.pwid == vcid && i.neighbor == destino)
            return &i;
    return nullptr;
}

SMplsL2PWInfo* MplsL2TransportInfo::buscarPW(QString AC)
{
    for (SMplsL2PWInfo &i : m_lstMplsL2PWs)
        if (i.AC == AC)
            return &i;
    return nullptr;
}

SMplsL2PWInfo* MplsL2TransportInfo::mplsL2PWfromPreferredPath(QString pp)
{
    if ( pp.isEmpty() )
        return nullptr;

    for ( SMplsL2VFIInfo &i : m_lstMplsL2VFIs )
        for ( SMplsL2PWInfo &p : i.lstPWs )
            if ( p.preferredPath == pp )
                return &p;

    for ( SMplsL2PWInfo &i : m_lstMplsL2PWs )
        if ( i.preferredPath == pp )
            return &i;

    return nullptr;
}

QDataStream& operator<<(QDataStream& out, const MplsL2TransportInfo& info)
{
    out << info.m_lstMplsL2PWs;
    out << info.m_lstMplsL2VFIs;
    out << info.m_queryoption;
    out << info.m_queryName;
    return out;
}

QDataStream& operator>>(QDataStream& in, MplsL2TransportInfo& info)
{
    in >> info.m_lstMplsL2PWs;
    in >> info.m_lstMplsL2VFIs;
    in >> info.m_queryoption;
    in >> info.m_queryName;
    return in;
}

QDataStream& operator<<(QDataStream& out, const MplsL2TransportInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, MplsL2TransportInfo*& info)
{
    info = new MplsL2TransportInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const MplsL2TransportInfo &info)
{
    dbg.nospace() << "MplsL2TransportInfo:\n";

    dbg.nospace() << "PWs:\n";
    for (SMplsL2PWInfo i : info.m_lstMplsL2PWs)
        dbg.space() << "--- group" << i.group
                    << "xc" << i.xc
                    << "srvid" << i.srvid
                    << "AC" << i.AC
                    << "ACXEInterface" << i.ACXEInterfaz
                    << "ACXEServiceInstance" << i.ACXEServiceInstance
                    << "mtu" << i.mtu
                    << "descripcion" << i.descripcion
                    << "source" << i.source
                    << "pwid" << i.pwid
                    << "neighbor" << i.neighbor
                    << "pwclass" << i.pwclass
                    << "controlword" << i.controlword
                    << "state" << i.state
                    << "preferredPath" << i.preferredPath
                    << "remoteDescripcion" << i.remoteDescripcion
                    << "neighborbk" << i.neighborbk
                    << "pwidbk" << i.pwidbk
                    << "statebk" << i.statebk
                    << "pwclassbck" << i.pwclassbck
                    << "controlwordbck" << i.controlwordbck
                    << "FAT" << i.fat
                    << "FATBck" << i.fatbck
                    << "\n";

    dbg.nospace() << "\n";
    dbg.nospace() << "VFIs:\n";
    for ( SMplsL2VFIInfo i : info.m_lstMplsL2VFIs )
    {
        dbg.space() << i.vfi << i.bridge << i.descripcion << i.descripcion << "\n";
        dbg.space() << " PWs:\n";
        for ( SMplsL2PWInfo pw : i.lstPWs )
            dbg.space() << " " << pw << "\n";
    }

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
