#include "mplsl2transport.h"

QDataStream& operator<<(QDataStream& out, const SMplsL2PWInfo& data)
{
    out << data.VCID;
    out << data.destino;
    out << data.preferredPath;
    out << data.remoteDescripcion;
    out << data.estado;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMplsL2PWInfo& data)
{
    in >> data.VCID;
    in >> data.destino;
    in >> data.preferredPath;
    in >> data.remoteDescripcion;
    in >> data.estado;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

QDebug operator<<(QDebug dbg, const SMplsL2PWInfo &info)
{
    dbg.space() << "(" << info.VCID << info.destino << info.estado
                << info.preferredPath << info.remoteDescripcion << ")";
    return dbg.maybeSpace();
}

QDataStream& operator<<(QDataStream& out, const SMplsL2XconnectInfo& data)
{
    out << data.VCID;
    out << data.AC;
    out << data.destino;
    out << data.preferredPath;
    out << data.remoteDescripcion;
    out << data.estado;
    out << data.xc;
    out << data.descripcion;
    out << data.AC;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SMplsL2XconnectInfo& data)
{
    in >> data.VCID;
    in >> data.AC;
    in >> data.destino;
    in >> data.preferredPath;
    in >> data.remoteDescripcion;
    in >> data.estado;
    in >> data.xc;
    in >> data.descripcion;
    in >> data.AC;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

QDataStream& operator<<(QDataStream& out, const SMplsL2VFIInfo& data)
{
    out << data.vfi;
    out << data.AC;
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
    in >> data.AC;
    in >> data.descripcion;
    in >> data.bridge;
    in >> data.lstPWs;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SMplsL2XconnectInfo> &lstDest, QList<SMplsL2XconnectInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SMplsL2XconnectInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SMplsL2XconnectInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SMplsL2XconnectInfo &dest : lstDest )
        {
            if ( origin.xc == dest.xc )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.VCID = origin.VCID;
                dest.AC = origin.AC;
                dest.destino = origin.destino;
                dest.preferredPath = origin.preferredPath;
                dest.remoteDescripcion = origin.remoteDescripcion;
                dest.estado = origin.estado;
                dest.descripcion = origin.descripcion;
                dest.AC = origin.AC;
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
                dest.AC = origin.AC;
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
    m_lstMplsL2Xconnects = other.m_lstMplsL2Xconnects;
    m_lstMplsL2VFIs = other.m_lstMplsL2VFIs;
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
            termSendText("sh l2vpn bridge-domain detail | i \"Bridge group|Description|AC:|PWs:|VFI|PW: neighbor|Interface|Preferred\"");
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
            i.VCID = match.captured(2);
            i.destino = match.captured(1);
            mvi->lstPWs.append( i );
            continue;
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_IOS_L2Transport()));
    termSendText("sh mpls l2transport vc detail | i Local interface|Destination|Preferred|Remote interface");
}

void MplsL2TransportInfo::on_term_receiveText_IOS_L2Transport()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QString lastVFI;
    QString lastDestino;
    QString lastVCID;
    SMplsL2XconnectInfo *mxi = nullptr;
    foreach (QString line, lines)
    {
        line = line.simplified();

//        Local interface: Te0/0/4 up, line protocol up, Eth VLAN 2500 up
//          Destination address: 172.16.30.11, VC ID: 57250001, VC status: up
//            Preferred path: Tunnel5701,  active
//            Remote interface description: COUS09820001|PGM|CALLZILLA_BOGOTA

        QRegularExpression exp("Local interface: VFI (\\S+) \\S+");
        if ( line.contains(exp,&match) )
        {
            lastVFI = match.captured(1);
            continue;
        }

        exp.setPattern("Local interface: (\\S+) \\S+");
        if ( line.contains(exp,&match) )
        {
            lastVFI.clear();
            lastVCID.clear();
            lastDestino.clear();

            SMplsL2XconnectInfo i;
            i.AC = match.captured(1);
            m_lstMplsL2Xconnects.append(i);
            mxi = &m_lstMplsL2Xconnects.last();

            if ( !lstIosInterfaces.contains(i.AC) )
                lstIosInterfaces.append(i.AC);
        }

        if ( !mxi && lastVFI.isEmpty() )
            continue;



        exp.setPattern("Destination address: (\\S+), VC ID: (\\d+), VC status: (\\S+)");
        if ( line.contains(exp,&match) )
        {
            if ( lastVFI.isEmpty() )
            {
                //normal
                mxi->destino = match.captured(1);
                mxi->VCID = match.captured(2);
                mxi->estado = match.captured(3);
            }
            else
            {
                //vfi
                lastDestino = match.captured(1);
                lastVCID = match.captured(2);
                SMplsL2PWInfo *pw = buscarVFI_PW( lastVFI, lastDestino, lastVCID );
                if ( pw )
                    pw->estado = match.captured(3);
            }
            continue;
        }

        exp.setPattern("Preferred path: (\\S+), \\S+");
        if ( line.contains(exp,&match) )
        {
            if ( lastVFI.isEmpty() )
                //normal
                mxi->preferredPath = match.captured(1);
            else
            {
                //vfi
                SMplsL2PWInfo *pw = buscarVFI_PW( lastVFI, lastDestino, lastVCID );
                if ( pw )
                    pw->estado = match.captured(1);
            }
            continue;
        }

        exp.setPattern("Remote interface description: (.+)$");
        if ( line.contains(exp,&match) )
        {
            if ( lastVFI.isEmpty() )
                //normal
                mxi->remoteDescripcion = match.captured(1);
            else
            {
                //vfi
                SMplsL2PWInfo *pw = buscarVFI_PW( lastVFI, lastDestino, lastVCID );
                if ( pw )
                    pw->remoteDescripcion = match.captured(1);
            }
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

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        //UP pri ac Te0/0/4:307(Eth VLAN) UP mpls 172.17.22.79:502307 UP
        QRegularExpression exp("\\w{2} \\S+ ac (\\S+):(\\d+)\\(.+\\) \\w{2} mpls (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}):(\\d+) \\w{2}");
        if ( line.contains(exp,&match) )
        {
            for ( SMplsL2XconnectInfo &i : m_lstMplsL2Xconnects )
            {
                if ( i.AC == match.captured(1) && i.VCID == match.captured(4) && i.destino == match.captured(3) )
                {
                    i.AC.append( ":"+match.captured(2) );
                    break;
                }
            }
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
//            Preferred path Active : tunnel-te50601, Statically configured, fallback enabled
//              Interface    VLAN1885                       CRCRCAB10039792I|CABLE_CENTRO|CIUDAD_NEILY
//          PW: neighbor 172.19.24.88, PW ID 506188523, state is down ( all ready ) (Segment-down)
//              Interface    VLAN1885                       CRCRCAB10039792I|CABLE_CENTRO|CIUDAD_NEILY
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
            mvi->AC = match.captured(1);
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
            i.destino = match.captured(1);
            i.VCID = match.captured(2);
            i.estado = match.captured(3);
            mvi->lstPWs.append(i);
            pw = &mvi->lstPWs.last();
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
            pw->remoteDescripcion = match.captured(1);
            continue;
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_XR_Xconnect()));
    termSendText("sh l2vpn xconnect detail | i \"Group|AC:|PW:|Interface\"");
}

void MplsL2TransportInfo::on_term_receiveText_XR_Xconnect()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SMplsL2XconnectInfo *xi = nullptr;
    foreach (QString line, lines)
    {
        line = line.simplified();

//        Group L2VPN_SERVICES, XC Cabletica_CRCR04100019, state is up; Interworking none
//          AC: TenGigE0/6/0/5.1001, state is up
//          PW: neighbor 172.16.30.247, PW ID 506100101, state is up ( established )
//              Group ID     0x10000500                     0x4000340
//              Interface    TenGigE0/6/0/5.1001            TenGigE0/0/0/4.1001

        QRegularExpression exp("Group \\S+, XC (\\S+), state");
        if ( line.contains(exp,&match) )
        {
            SMplsL2XconnectInfo i;
            i.xc = match.captured(1);
            m_lstMplsL2Xconnects.append(i);
            xi = &m_lstMplsL2Xconnects.last();
            continue;
        }

        if ( !xi  )
            continue;

        exp.setPattern("AC: (\\S+), state");
        if ( line.contains(exp,&match) )
        {
            xi->AC = match.captured(1);
            continue;
        }

        exp.setPattern("PW: neighbor (\\S+), PW ID (\\d+), state is (\\S+) ");
        if ( line.contains(exp,&match) )
        {
            xi->destino = match.captured(1);
            xi->VCID = match.captured(2);
            xi->estado = match.captured(3);
            continue;
        }

        exp.setPattern("Interface \\S+ (\\S+)");
        if ( line.contains(exp,&match) )
        {
            xi->remoteDescripcion = match.captured(1);
            continue;
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
    foreach (QString line, lines)
    {
        line = line.simplified();

        //VSI Name: BRBRESP10097311C                            Signaling: ldp
        //--------------------------------------------------------------------
        //Peer                Transport  Local      Remote      VC
        //Addr                VC ID      VC Label   VC Label    State
        //--------------------------------------------------------------------
        //172.16.31.129       55248903   49804      18481       up
        //172.17.38.18        55248903   49801      48065       up

        QRegularExpression exp("VSI Name: (\\S+) ");
        if ( line.contains(exp,&match) )
        {
            SMplsL2VFIInfo i;
            i.vfi = match.captured( 1 );
            m_lstMplsL2VFIs.append( i );
            mvi = &m_lstMplsL2VFIs[m_lstMplsL2VFIs.size()-1];
        }

        if ( !mvi )
            continue;

        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3} ");
        if ( line.contains(exp,&match) )
        {
            QStringList data = line.split(" ",Qt::SkipEmptyParts);
            if ( data.size() < 5 )
                continue;
            SMplsL2PWInfo i;
            i.destino = data.at(0);
            i.VCID = data.at(1);
            i.estado = data.at(4);
            mvi->lstPWs.append( i );
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
        QRegularExpression expbridge("Bridge-domain (\\d+)");
        line.replace(expbridge,"Vbdif\\1");

        QStringList data=line.split(" ",Qt::SkipEmptyParts);
        if ( data.size() < 4 )
            continue;

        for ( SMplsL2VFIInfo& i : m_lstMplsL2VFIs )
        {
            if ( i.vfi == data.at(1) )
            {
                i.AC=estandarizarInterfaz(data.at(0));
                break;
            }
        }
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_VRP_L2Transport()));
    termSendText("display mpls l2vc brief");
}

void MplsL2TransportInfo::on_term_receiveText_VRP_L2Transport()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SMplsL2XconnectInfo *xi = nullptr;
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

        QRegularExpression exp("Client Interface : (.*)$");
        if ( line.contains(exp,&match) )
        {
            SMplsL2XconnectInfo i;
            i.AC = match.captured(1);
            m_lstMplsL2Xconnects.append(i);
            xi = &m_lstMplsL2Xconnects.last();
            continue;
        }

        if ( !xi  )
            continue;

        exp.setPattern("VC state : (.+)$");
        if ( line.contains(exp,&match) )
        {
            xi->estado = match.captured(1);
            continue;
        }

        exp.setPattern("VC ID : (.+)$");
        if ( line.contains(exp,&match) )
        {
            xi->VCID = match.captured(1);
            continue;
        }

        exp.setPattern("Destination : (.+)$");
        if ( line.contains(exp,&match) )
        {
            xi->destino = match.captured(1);
            continue;
        }
    }
    finished();
}

SMplsL2PWInfo* MplsL2TransportInfo::buscarVFI_PW(QString vfi, QString destino, QString vcid)
{
    for ( SMplsL2VFIInfo &i : m_lstMplsL2VFIs )
    {
        if ( i.vfi == vfi  )
        {
           for ( SMplsL2PWInfo &p : i.lstPWs )
           {
               if ( p.VCID == vcid && p.destino == destino )
                   return &p;
           }
        }
    }
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

    for ( SMplsL2XconnectInfo &i : m_lstMplsL2Xconnects )
        if ( i.preferredPath == pp )
            return &i;

    return nullptr;
}

QDataStream& operator<<(QDataStream& out, const MplsL2TransportInfo& info)
{
    out << info.m_lstMplsL2Xconnects;
    out << info.m_lstMplsL2VFIs;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, MplsL2TransportInfo& info)
{
    in >> info.m_lstMplsL2Xconnects;
    in >> info.m_lstMplsL2VFIs;
    in >> info.m_queryoption;
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

    dbg.nospace() << "XConnects:\n";
    for (SMplsL2XconnectInfo i : info.m_lstMplsL2Xconnects)
        dbg.space() << i.xc << i.AC << i.descripcion << i.VCID <<
                       i.estado << i.destino << i.preferredPath << i.remoteDescripcion << "\n";

    dbg.nospace() << "\n";
    dbg.nospace() << "VFIs:\n";
    for ( SMplsL2VFIInfo i : info.m_lstMplsL2VFIs )
    {
        dbg.space() << i.vfi << i.bridge << i.descripcion << "\n";
        dbg.space() << " PWs:\n";
        for ( SMplsL2PWInfo pw : i.lstPWs )
            dbg.space() << " " << pw << "\n";
    }

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
