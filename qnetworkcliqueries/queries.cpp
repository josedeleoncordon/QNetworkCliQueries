#include "queries.h"

#include <QThread>
#include <QEventLoop>

#include "funciones.h"
#include "factory.h"
#include "properties.h"
#include "qnetworkquerieslogging.h"

Queries::Queries(QString IP, QObject *parent) : QObject(parent)
{
    iniciar();
    m_ip = IP;
    m_user = Properties::Instance()->user;
    m_pwd = Properties::Instance()->password;
    m_linuxprompt = Properties::Instance()->linuxpromt;
}

Queries::Queries(QString IP, QString user, QString pwd, QString linuxprompt, QObject *parent) : QObject(parent)
{    
    iniciar();
    m_ip=IP;
    m_user=user;
    m_pwd=pwd;   
    m_linuxprompt=linuxprompt;
}

Queries::Queries()
{
    iniciar();
}

Queries::Queries(const Queries &other) : QObject(other.parent())
{
    iniciar();
    clone(other);
}

Queries::~Queries()
{
    qCDebug(queries) << m_ip << "Queries::~Queries()";
    qDeleteAll(m_lstFunciones);
}

void Queries::iniciar()
{    
    term = nullptr;
    pi = nullptr;
    equipmentNeighborsInfoQuery = nullptr;
    interfacesInfoQuery = nullptr;
    interfacesPermitedVlansQuery = nullptr;
    interfacesDescriptionsQuery = nullptr;
    interfacesIpAddressesQuery = nullptr;
    ospfQuery = nullptr;
    mplsTEtunnelsQuery = nullptr;
    mplsL2TransportQuery = nullptr;
    mplsLdpDiscoveryQuery = nullptr;
    mplsLdpNeighborsQuery = nullptr;
    mplsLdpInterfacesQuery = nullptr;
    pimInteracesQuery = nullptr;
    macsQuery = nullptr;
    portChannelInfoQuery = nullptr;
    vrfsFromVlansQuery = nullptr;
    vrfFromRTQuery = nullptr;
    vrfsQuery = nullptr;
    arpsQuery = nullptr;
    bgpNeighborsQuery = nullptr;
    bgpNetworksQuery = nullptr;
    ipRoutesQuery = nullptr;
    configQuery = nullptr;
    exitQuery = nullptr;
    funcionQuery = nullptr;
    m_connectionprotol = QRemoteShell::SSHTelnet;
    m_opcionActual = QueryOpcion::Null;
    m_connected=false;
    m_error=false;
    m_currentFuncion=nullptr;
    m_ipreachable=false;    
    m_conectionSecondTriedOtherUserPassword=false;    
    m_reintentandoConsulta=false;
    m_contieneconsultas=false;
    m_consultaIntentos=0;     
    m_operativo=true;
    m_datetime = QDateTime::currentDateTime();

    queryTimer = new QTimer(this);
    queryTimer->setInterval( 20000 );
    queryTimer->setSingleShot( true );
    connect(queryTimer,SIGNAL(timeout()),SLOT(on_queryTimer_timeout()));
}

void Queries::clone(const Queries& other)
{
    m_connected = other.m_connected;
    m_name = other.m_name;
    m_fullName = other.m_fullName;
    m_country = other.m_country;
    m_platform = other.m_platform;
    m_equipmenttype = other.m_equipmenttype;
    m_brand = other.m_brand;
    m_datetime = other.m_datetime;
    m_connectionprotol = other.m_connectionprotol;
    m_operativo = other.m_operativo;
    m_contieneconsultas = other.m_contieneconsultas;
    m_xr_location = other.m_xr_location;
    m_location = other.m_location;
    m_ip = other.m_ip;
    m_user = other.m_user;
    m_gw = other.m_gw;
    m_pwd = other.m_pwd;
    m_lstOpciones = other.m_lstOpciones;

    for ( FuncionBase *f : other.m_lstFunciones )
    {
        switch (f->queryOption()) {
        case (QueryOpcion::EquipmentNeighbors): {
            m_lstFunciones.append( new EquipmentNeighborsInfo(*dynamic_cast<EquipmentNeighborsInfo*>(f)) );
            break;
        }
        case (QueryOpcion::MacAddress): {
            m_lstFunciones.append( new MacInfo(*dynamic_cast<MacInfo*>(f)) );
            break;
        }
        case (QueryOpcion::InterfaceInformation):
        case (QueryOpcion::InterfacePermitedVlans):
        case (QueryOpcion::InterfaceDescription):
        case (QueryOpcion::InterfaceIpAddresses):
        {
            m_lstFunciones.append( new InterfaceInfo(*dynamic_cast<InterfaceInfo*>(f)) );
            break;
        }
        case (QueryOpcion::Ospf): {
            m_lstFunciones.append( new OSPFInfo(*dynamic_cast<OSPFInfo*>(f)) );
            break;
        }
        case (QueryOpcion::MplsTEtunnels): {
            m_lstFunciones.append( new MplsTEtunnelsInfo(*dynamic_cast<MplsTEtunnelsInfo*>(f)) );
            break;
        }
        case (QueryOpcion::MplsLdpDiscovery):
        case (QueryOpcion::MplsLdpNeighbors):
        case (QueryOpcion::MplsLdpInterfaces):
        {
            m_lstFunciones.append( new MplsLdpInfo(*dynamic_cast<MplsLdpInfo*>(f)) );
            break;
        }
        case (QueryOpcion::PimInterfaces): {
            m_lstFunciones.append( new PIMInfo(*dynamic_cast<PIMInfo*>(f)) );
            break;
        }
        case (QueryOpcion::PortChannel): {
            m_lstFunciones.append( new PortChannelsInfo(*dynamic_cast<PortChannelsInfo*>(f)) );
            break;
        }
        case (QueryOpcion::VRFfVlans):
        case (QueryOpcion::VRFfRT):
        case (QueryOpcion::VRFs):
        {
            m_lstFunciones.append( new VrfInfo(*dynamic_cast<VrfInfo*>(f)) );
            break;
        }
        case (QueryOpcion::Arp): {
            m_lstFunciones.append( new ArpInfo(*dynamic_cast<ArpInfo*>(f)) );
            break;
        }
        case (QueryOpcion::BGPNeig):

        case (QueryOpcion::BGPNetworks):
        {
            m_lstFunciones.append( new BGPInfo(*dynamic_cast<BGPInfo*>(f)) );
            break;
        }
        case (QueryOpcion::IpRoutes): {
            m_lstFunciones.append( new IPRouteInfo(*dynamic_cast<IPRouteInfo*>(f)) );
            break;
        }
        case (QueryOpcion::Mplsl2Transport): {
            m_lstFunciones.append( new MplsL2TransportInfo(*dynamic_cast<MplsL2TransportInfo*>(f)) );
            break;
        }
        case (QueryOpcion::Funcion): {
            m_lstFunciones.append( new FuncionInfo(*dynamic_cast<FuncionInfo*>(f)) );
            break;
        }
        default: {}
        }
    }
}

FuncionBase *Queries::createQuerie(QueryOpcion option)
{
    switch (option) {
    case (QueryOpcion::EquipmentNeighbors): { return factoryNewEquipmentNeighborsInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::MacAddress): { return factoryNewMacInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::InterfaceInformation):
    case (QueryOpcion::InterfacePermitedVlans):
    case (QueryOpcion::InterfaceDescription):
    case (QueryOpcion::InterfaceIpAddresses): { return factoryNewInterfaceInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::Ospf): { return factoryNewOSPFInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::MplsTEtunnels): { return factoryNewMplsTEtunnelsInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::MplsLdpDiscovery):
    case (QueryOpcion::MplsLdpNeighbors):
    case (QueryOpcion::MplsLdpInterfaces): { return factoryNewMplsLdpInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::PimInterfaces): { return factoryNewPIMInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::PortChannel): { return factoryNewPortChannelsInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::VRFfVlans):
    case (QueryOpcion::VRFfRT):
    case (QueryOpcion::VRFs): { return factoryNewVrfInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::Arp): { return factoryNewArpInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::BGPNeig): { return factoryNewBGPNeighborInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::BGPNetworks): { return factoryNewBGPNetworksInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::IpRoutes): { return factoryNewIPRouteInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::Configuration): { return factoryNewConfig(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::Mplsl2Transport): { return factoryNewMplsL2TransportInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::Funcion): { return factoryNewFuncionInfo(m_brand,m_equipmenttype,term); }
    case (QueryOpcion::Exit): { return factoryNewExit(m_brand,m_equipmenttype,term); }
    default: { return nullptr; }
    }
}

void Queries::addOption( QList<QueryOpcion> lst )
{
    lst.removeAll( QueryOpcion::Null ); //unicamente opciones validas
    m_lstOpciones.append( lst );
}

void Queries::setGW(QString GW)
{
    m_gw = GW;
}

void Queries::conectarAequipo(QString ip,QString user, QString pwd, QString platform, QString linuxprompt)
{
    m_consultaIntentos++;
    term = new QRemoteShell(ip,user,pwd,platform,linuxprompt,this);
    term->setConnectionProtocol( m_connectionprotol );
    term->setGW(m_gw);
    term->setUser2( m_user2 );
    term->setPassword2( m_pwd2 );
    connect(term,SIGNAL(reachable()),SLOT(processConnectToHostReachable()));
    connect(term,SIGNAL(connected()),SLOT(processConnectToHostConnected()));
    connect(term,SIGNAL(disconnected()),SLOT(processConnectToHostDisconnected()));
    connect(term,SIGNAL(working()),SLOT(processKeepWorking()));
    term->host_connect();
}

void Queries::borrarTerminal()
{
    qCDebug(queries) << m_ip << "Queries::borrarTerminal()" << term;

    if ( term )
    {
        qCDebug(queries) << m_ip  << "borrando terminal";

        term->disconnectReceiveTextSignalConnections();
        term->disconnect(); //para q no se emita la señal de desconectado
        term->host_disconnect();
        term->deleteLater();
        term = nullptr;
    }
}

bool Queries::existsQueryInformation(QueryOpcion option, int i)
{
    return getQuery(option,i);
}

FuncionBase *Queries::getQuery(QueryOpcion option,int i)
{
    int c=1;
    for ( FuncionBase *f : m_lstFunciones )
    {
        if ( f->queryOption() == option )
        {
            if ( c != i )
                c++;
            else
                return f;
        }
    }
    return nullptr;
}

QList<SEquipmentNeighborsInfo>& Queries::equipmentNeighborsInfo(int i)
{
    FuncionBase *f = getQuery(EquipmentNeighbors,i);
    if ( !f ) return _lstSEquipmentNeighborsInfo;
    return dynamic_cast<EquipmentNeighborsInfo*>(f)->equipmentNeighborsInfo();
}

QList<SInterfaceInfo>& Queries::interfacesInfo(int i)
{
    FuncionBase *f = getQuery(InterfaceInformation,i);
    if ( !f ) return _lstSInterfaceInfo;
    return dynamic_cast<InterfaceInfo*>(getQuery(InterfaceInformation,i))->interfacesInfo();
}

QList<SInterfaceVlans>& Queries::interfacesPermitedVlansInfo(int i)
{
    FuncionBase *f = getQuery(InterfacePermitedVlans,i);
    if ( !f ) return _lstSInterfaceVlans;
    return dynamic_cast<InterfaceInfo*>(getQuery(InterfacePermitedVlans,i))->interfacesPermitedVlansInfo();
}

QList<SInterfaceInfo>& Queries::interfacesDescriptionsInfo(int i)
{
    FuncionBase *f = getQuery(InterfaceDescription,i);
    if ( !f ) return _lstSInterfaceInfo;
    return dynamic_cast<InterfaceInfo*>(getQuery(InterfaceDescription,i))->interfacesInfo();
}

QList<SIpInfo>& Queries::interfacesIpAddressesInfo(int i)
{
    FuncionBase *f = getQuery(InterfaceIpAddresses,i);
    if ( !f ) return _lstSIpInfo;
    return dynamic_cast<InterfaceInfo*>(getQuery(InterfaceIpAddresses,i))->interfacesIpAddressesInfo();
}

QList<SOSPFInfo>& Queries::ospfInfo(int i)
{
    FuncionBase *f = getQuery(Ospf,i);
    if ( !f ) return _lstSOSPFInfo;
    return dynamic_cast<OSPFInfo*>(getQuery(Ospf,i))->ospfInfo();
}

QString& Queries::mplsLdpLocalIDInfo(int i)
{
    FuncionBase *f = getQuery(MplsLdpDiscovery,i);
    if ( !f ) return _qstring;
    return dynamic_cast<MplsLdpInfo*>(getQuery(MplsLdpDiscovery,i))->ldpLocalIDInfo();
}

QList<SMplsTETunnelInfo>& Queries::mplsTETunnelsInfo(int i)
{
    FuncionBase *f = getQuery(MplsTEtunnels,i);
    if ( !f ) return _lstSMplsTETunnelInfo;
    return dynamic_cast<MplsTEtunnelsInfo*>(getQuery(MplsTEtunnels,i))->mplsTEtunnelsInfo();
}

QList<SMplsL2XconnectInfo>& Queries::mplsL2TransportXconnectsInfo(int i)
{
    FuncionBase *f = getQuery(Mplsl2Transport,i);
    if ( !f ) return _lstSMplsL2XconnectInfo;
    return dynamic_cast<MplsL2TransportInfo*>(getQuery(Mplsl2Transport,i))->mplsL2XconnectsInfo();
}

QList<SMplsL2VFIInfo>& Queries::mplsL2TransportVFIsInfo(int i)
{
    FuncionBase *f = getQuery(Mplsl2Transport,i);
    if ( !f ) return _lstSMplsL2VFIInfo;
    return dynamic_cast<MplsL2TransportInfo*>(getQuery(Mplsl2Transport,i))->mplsL2VFIsInfo();
}

QList<SMplsLdpInfo>& Queries::mplsLdpDiscoveryInfo(int i)
{
    FuncionBase *f = getQuery(MplsLdpDiscovery,i);
    if ( !f ) return _lstSMplsLdpInfo;
    return dynamic_cast<MplsLdpInfo*>(getQuery(MplsLdpDiscovery,i))->mplsLdpDiscoveryInfo();
}

QList<SMplsLdpInfo>& Queries::mplsLdpNeighborsInfo(int i)
{
    FuncionBase *f = getQuery(MplsLdpNeighbors,i);
    if ( !f ) return _lstSMplsLdpInfo;
    return dynamic_cast<MplsLdpInfo*>(getQuery(MplsLdpNeighbors,i))->mplsLdpNeighborsInfo();
}

QList<SMplsLdpInfo>& Queries::mplsLdpInterfacesInfo(int i)
{
    FuncionBase *f = getQuery(MplsLdpInterfaces,i);
    if ( !f ) return _lstSMplsLdpInfo;
    return dynamic_cast<MplsLdpInfo*>(getQuery(MplsLdpInterfaces,i))->mplsLdpInterfacesInfo();
}

QList<SPIMInfo>& Queries::pimInterfacesInfo(int i)
{
    FuncionBase *f = getQuery(PimInterfaces,i);
    if ( !f ) return _lstSPIMInfo;
    return dynamic_cast<PIMInfo*>(getQuery(PimInterfaces,i))->pimIntefacesInfo();
}

QList<SMacInfo>& Queries::macInfo(int i)
{
    FuncionBase *f = getQuery(MacAddress,i);
    if ( !f ) return _lstSMacInfo;
    return dynamic_cast<MacInfo*>(getQuery(MacAddress,i))->macInfo();
}

QList<SPortChannel>& Queries::portChannelInfo(int i)
{
    FuncionBase *f = getQuery(PortChannel,i);
    if ( !f ) return _lstSPortChannel;
    return dynamic_cast<PortChannelsInfo*>(getQuery(PortChannel,i))->portChannelInfo();
}

QStringList& Queries::vrfsFromVlansInfo(int i)
{
    FuncionBase *f = getQuery(VRFfVlans,i);
    if ( !f ) return _lstQStringList;
    return dynamic_cast<VrfInfo*>(getQuery(VRFfVlans,i))->vrfsFromVlansInfo();
}

QString& Queries::vrfFromRTInfo(int i)
{
    FuncionBase *f = getQuery(VRFfRT,i);
    if ( !f ) return _qstring;
    return dynamic_cast<VrfInfo*>(getQuery(VRFfRT,i))->vrfFromRTInfo();
}

QList<SVrfInfo>& Queries::vrfsInfo(int i)
{
    FuncionBase *f = getQuery(VRFs,i);
    if ( !f ) return _lstSVrfInfo;
    return dynamic_cast<VrfInfo*>(getQuery(VRFs,i))->vrfsInfo();
}

QList<SIpInfo>& Queries::arpsInfo(int i)
{
    FuncionBase *f = getQuery(Arp,i);
    if ( !f ) return _lstSIpInfo;
    return dynamic_cast<ArpInfo*>(getQuery(Arp,i))->arpInfo();
}

QList<SBGPNeighbor>& Queries::bgpNeighborsInfo(int i)
{
    FuncionBase *f = getQuery(BGPNeig,i);
    if ( !f ) return _lstSBGPNeighbor;
    return dynamic_cast<BGPInfo*>(getQuery(BGPNeig,i))->bgpNeighborInfo();
}

QList<SBGPNetwork>& Queries::bgpNetworksInfo(int i)
{
    FuncionBase *f = getQuery(BGPNetworks,i);
    if ( !f ) return _lstSBGPNetwork;
    return dynamic_cast<BGPInfo*>(getQuery(BGPNetworks,i))->bgpNetworksInfo();
}

QList<SIpRouteInfo>& Queries::ipRoutesInfo(int i)
{
    FuncionBase *f = getQuery(IpRoutes,i);
    if ( !f ) return _lstSIpRouteInfo;
    return dynamic_cast<IPRouteInfo*>(getQuery(IpRoutes,i))->ipRouteInfo();
}

QString Queries::funcionTxtInfo(int i)
{
    FuncionBase *f = getQuery(Funcion,i);
    if ( !f ) return _qstring;
    return dynamic_cast<FuncionInfo*>(getQuery(Funcion,i))->txtReceived();
}

void Queries::nextProcess()
{
    qCDebug(queries) << m_ip  << "Queries::nextProcess()";

    if ( !m_reintentandoConsulta )
    {
        if ( m_lstOpciones.isEmpty() )
        {
            //no hay funciones a ejecutar, se finaliza

            //si hay informacion de OSPF se cambia la IP de la interfaz que se conoce por cdp/lldp
            //y se reemplaza por el ID            
            for ( SEquipmentNeighborsInfo &e : equipmentNeighborsInfo() )
                for ( SOSPFInfo &oi : ospfInfo() )
                {
                    if ( oi.interfaz == e.interfazestesalida )
                    {
                        e.ip = oi.id;
                        break;
                    }
                }

            emit finished(this);
            disconnect();

            return;
        }

        m_opcionActual = m_lstOpciones.takeFirst();
    }
    else
    {
        if ( m_currentFuncion ) delete m_currentFuncion;
        m_reintentandoConsulta = false;
    }

    qCDebug(queries) << m_ip  << "NextProcess:" << m_opcionActual;

    m_currentFuncion = createQuerie( m_opcionActual );
    if ( !m_currentFuncion )
    {
        qCDebug(queries) << m_ip  << "Queries::nextProcess() No se creo la funcion para m_opcionActual" << m_opcionActual;
        nextProcess();
        return;
    }

    queryTimer->setInterval( 20000 );
    queryTimer->start();

    //consultas CLI    

    if ( m_opcionActual & Connect )
    {        
        qCDebug(queries) << m_ip  << "creando term";
        conectarAequipo(m_ip,m_user,m_pwd,m_platform,m_linuxprompt);
        return;
    }

    if ( !m_connected )
        return;

    if ( m_opcionActual & Platform )
    {                
        if ( m_platform.isEmpty() )
        {
            PlatformInfo *f=dynamic_cast<PlatformInfo*>(m_currentFuncion);
            if ( !pi ) pi = f;
            f->setBrand(m_brand);
            f->setHostName(m_fullName);
            f->setIp(m_ip);
            connect(f,SIGNAL(processFinished()),SLOT(processPlatform()));
            connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
            connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
            f->getPlatformInfo();
        }
        else
        {
            if ( m_reintentandoConsulta )
                m_reintentandoConsulta=false;
            nextProcess();
        }
        return;
    }
    else if ( m_opcionActual & EquipmentNeighbors )
    {        
        EquipmentNeighborsInfo *f=dynamic_cast<EquipmentNeighborsInfo*>(m_currentFuncion);
        if ( !equipmentNeighborsInfoQuery ) equipmentNeighborsInfoQuery = f;
        f->setBrand(m_brand);
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getEquipmentNeighborsInfo();
        return;
    }
    else if ( m_opcionActual & InterfaceInformation )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") )
            queryTimer->setInterval( 200000 );

        InterfaceInfo *f = dynamic_cast<InterfaceInfo*>(m_currentFuncion);
        if ( !interfacesInfoQuery ) interfacesInfoQuery = f;
        f->setBrand(m_brand);
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getInterfacesInfo();
        return;
    }
    else if ( m_opcionActual & InterfaceDescription )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") )
            queryTimer->setInterval( 200000 );

        InterfaceInfo *f = dynamic_cast<InterfaceInfo*>(m_currentFuncion);
        if ( !interfacesDescriptionsQuery ) interfacesDescriptionsQuery = f;
        f->setBrand(m_brand);
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getInterfacesDescriptions();
        return;
    }
    else if ( m_opcionActual & InterfaceIpAddresses )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") )
            queryTimer->setInterval( 200000 );

        InterfaceInfo *f = dynamic_cast<InterfaceInfo*>(m_currentFuncion);
        if ( !interfacesIpAddressesQuery ) interfacesIpAddressesQuery = f;
        f->setBrand(m_brand);
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getInterfacesIpAddresses();
        return;
    }
    else if ( m_opcionActual & InterfacePermitedVlans )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") )
            queryTimer->setInterval( 200000 );

        InterfaceInfo *f = dynamic_cast<InterfaceInfo*>(m_currentFuncion);
        if (!interfacesPermitedVlansQuery) interfacesPermitedVlansQuery=f;
        f->setBrand(m_brand);
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getInterfacesPermitedVlans();
        return;
    }
    else if (  m_opcionActual & Ospf )
    {
        OSPFInfo *f = dynamic_cast<OSPFInfo*>(m_currentFuncion);
        if (!ospfQuery) ospfQuery=f;
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getOSPFInfo();
        return;
    }    
    else if (  m_opcionActual & MplsTEtunnels )
    {
        MplsTEtunnelsInfo *f = dynamic_cast<MplsTEtunnelsInfo*>(m_currentFuncion);
        if (!mplsTEtunnelsQuery) mplsTEtunnelsQuery=f;
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getMplsTETunnels();
        return;
    }
    else if (  m_opcionActual & Mplsl2Transport )
    {
        MplsL2TransportInfo *f = dynamic_cast<MplsL2TransportInfo*>(m_currentFuncion);
        if (!mplsL2TransportQuery) mplsL2TransportQuery=f;
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getMplsL2Transport();
        return;
    }
    else if (  m_opcionActual & MplsLdpDiscovery )
    {
        MplsLdpInfo *f = dynamic_cast<MplsLdpInfo*>(m_currentFuncion);
        if (!mplsLdpDiscoveryQuery) mplsLdpDiscoveryQuery=f;
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getMplsLdpDiscovery();
        return;
    }
    else if (  m_opcionActual & MplsLdpNeighbors )
    {
        MplsLdpInfo *f = dynamic_cast<MplsLdpInfo*>(m_currentFuncion);
        if (!mplsLdpNeighborsQuery) mplsLdpNeighborsQuery=f;
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getMplsLdpNeighbors();
        return;
    }
    else if (  m_opcionActual & MplsLdpInterfaces )
    {
        MplsLdpInfo *f = dynamic_cast<MplsLdpInfo*>(m_currentFuncion);
        if (!mplsLdpInterfacesQuery) mplsLdpInterfacesQuery=f;
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getMplsLdpInterfaces();
        return;
    }
//    if (  m_opcionActual & PimNeighbors )
//    {
//        connect(pimNeighborsQuery,SIGNAL(processFinished()),SLOT(processFinished()));
//        pimNeighborsQuery->getPIMNeighbors();
//        return;
//    }
    else if (  m_opcionActual & PimInterfaces )
    {
        PIMInfo *f = dynamic_cast<PIMInfo*>(m_currentFuncion);
        if (!pimInteracesQuery) pimInteracesQuery=f;
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getPIMInterfaces();
        return;
    }
    else if ( m_opcionActual & MacAddress )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") ||
             m_platform.contains("760"))
            queryTimer->setInterval( 200000 );

        qCDebug(queries) << m_ip  << "Next: MacAddress";

        MacInfo *f = dynamic_cast<MacInfo*>(m_currentFuncion);
        if (!macsQuery) macsQuery=f;
        f->setBrand( m_brand );
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getMacInfo();
        return;
    }
    else if ( m_opcionActual & PortChannel )
    {
        PortChannelsInfo *f = dynamic_cast<PortChannelsInfo*>(m_currentFuncion);
        if (!portChannelInfoQuery) portChannelInfoQuery=f;
        f->setBrand(m_brand);
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getPortChannelsInfo();
        return;
    }
    else if ( m_opcionActual & VRFfVlans )
    {        
        VrfInfo *f = dynamic_cast<VrfInfo*>(m_currentFuncion);
        if (!vrfsFromVlansQuery) vrfsFromVlansQuery=f;
        f->setBrand( m_brand );
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getVRFsFromVLans();
        return;
    }
    else if ( m_opcionActual & VRFfRT )
    {
        VrfInfo *f = dynamic_cast<VrfInfo*>(m_currentFuncion);
        if (!vrfFromRTQuery) vrfFromRTQuery=f;
        f->setBrand( m_brand );
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getVRFfromRT();
        return;
    }
    else if ( m_opcionActual & VRFs )
    {
        VrfInfo *f = dynamic_cast<VrfInfo*>(m_currentFuncion);
        if (!vrfsQuery) vrfsQuery=f;
        f->setBrand( m_brand );
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getVRFs();
        return;
    }
    else if ( m_opcionActual & Arp )
    {
        qCDebug(queries) << m_ip  << "empezando ARP";

        ArpInfo *f = dynamic_cast<ArpInfo*>(m_currentFuncion);
        if (!arpsQuery) arpsQuery=f;
        f->setBrand(m_brand);
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getArpInfo();
        return;
    }
    else if ( m_opcionActual & BGPNeig )
    {
        BGPInfo *f = dynamic_cast<BGPInfo*>(m_currentFuncion);
        if (!bgpNeighborsQuery) bgpNeighborsQuery=f;
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getBGPNeighbors();
        return;
    }
    else if ( m_opcionActual & BGPNetworks )
    {
        BGPInfo *f = dynamic_cast<BGPInfo*>(m_currentFuncion);
        if (!bgpNetworksQuery) bgpNetworksQuery=f;
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getNetworks();
        return;
    }
    else if ( m_opcionActual & IpRoutes )
    {
        IPRouteInfo *f = dynamic_cast<IPRouteInfo*>(m_currentFuncion);
        if (!ipRoutesQuery) ipRoutesQuery=f;
        f->setPlatform( m_platform );
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getIPRouteInfo();
        return;
    }
    else if ( m_opcionActual & Configuration )
    {
        Config *f = dynamic_cast<Config*>(m_currentFuncion);
        if (!configQuery) configQuery=f;
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processConfigFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->configApply();
        return;
    }
    else if ( m_opcionActual & Funcion )
    {
        FuncionInfo *f = dynamic_cast<FuncionInfo*>(m_currentFuncion);
        if (!funcionQuery) funcionQuery=f;
        f->setPlatform(m_platform);
        f->setXRLocation(m_xr_location);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->getTXT();
        return;
    }
    else if ( m_opcionActual & Exit )
    {
        queryTimer->setInterval( 3000 );

        ExitInfo *f = dynamic_cast<ExitInfo*>(m_currentFuncion);
        if (!exitQuery) exitQuery=f;
        f->setPlatform(m_platform);
        f->setBrand(m_brand);
        f->setHostName(m_fullName);
        f->setIp(m_ip);
        f->setConnectedByGW( !m_gw.isEmpty() );
        f->setParentQuery(this);
        connect(f,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(f,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(f,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        f->exit();
        return;
    }
}

void Queries::startSync() //Sync
{
    disconnect(); //desconectamos todas las señales antes configuradas
    QEventLoop loop;
    connect(this, SIGNAL(finished(Queries*)), &loop, SLOT(quit()));
    start();
    loop.exec();
}

void Queries::start() //ASync
{
    nextProcess();
}

void Queries::processConnectToHostDisconnected()
{
    //si se desconecto el equipo porque finalizo exitosamente
    if ( m_opcionActual >= Exit )
        return;

    qCDebug(queries) << m_ip  << "Queries::processConnectToHostDisconnected()" << m_ip << m_name;

    borrarTerminal();

    if ( m_connected )
    {
        //se logro la conexion pero a media consulta se desconecto
        qCDebug(queries) << m_ip  << "**Equipo desconectado a media consulta**" << m_ip << m_name <<
                    "opcionActual" << m_opcionActual << "intentos" << m_consultaIntentos;

        if ( m_consultaIntentos <= 3 )
        {
            qCDebug(queries) << m_ip  << "reconectando y continuando consulta donde se quedo" << m_ip << m_name;
            m_connected=false;
            conectarAequipo(m_ip,m_user,m_pwd,m_platform,m_linuxprompt);
        }
        else
        {
            qCDebug(queries) << m_ip  << "3 intentos de consulta, se finaliza" << m_ip << m_name;
            m_error=true;
            emit finished(this);
        }
    }
    else
    {
        qCDebug(queries) << m_ip << "**No se ha logrado autenticar en el equipo: intentos" << m_consultaIntentos;

        if ( m_ipreachable )
        {
            if ( m_consultaIntentos <= 3 )
            {
                qCDebug(queries) << m_ip  << "intentando nuevamente conectarse al equipo" << m_ip << m_name;
                conectarAequipo(m_ip,m_user,m_pwd,m_platform,m_linuxprompt);
            }
            else
            {
                qCDebug(queries) << m_ip  << "3 intentos de consulta, se finaliza" << m_ip << m_name;
                m_error=true;
                emit finished(this);
            }
        }
        else
        {
            //no se logro la conexion
            qCDebug(queries) << m_ip  << "**NoConexion**" << m_ip;
            m_operativo=false;
            queryTimer->stop();

            //        //si tiene informacion de nombre o plataforma por snmp creamos los queries vacios para que este
            //        //disponible a la hora de graficar
            //        if ( !m_platform.isEmpty() )
            //            createQueries();

            emit finished(this);
            qCDebug(queries) << m_ip << "Queries_finished"; //No eliminar. Cierra el archivo de log
        }
    }
}

void Queries::processConnectToHostReachable()
{
    m_ipreachable=true;
}

void Queries::processConnectToHostConnected()
{
    if ( m_connected )
        return;

    qCDebug(queries) << m_ip  << "Equipo conectado" << m_ip << m_name;
    term->disconnectReceiveTextSignalConnections();
        
    queryTimer->stop();

    if ( m_opcionActual == Connect )
    {   
        //no se ha logrado conectar
        qCDebug(queries) << m_ip  << "Conectado. Primer intento" << m_ip << m_name;
        m_brand = term->brandName();
        
        if ( m_platform.isEmpty() )
        {
            m_platform = term->platform();
            m_equipmenttype=equipmentOSFromPlatform( m_platform );
        }

        if ( m_name.isEmpty() )
        {
            m_fullName = term->hostName();
            m_name = simplicateName( term->hostName() );
        }
    }
    else    
    {
        //siguientes intentos    
        qCDebug(queries) << m_ip  << "Conectado. Siguiente intento de consulta despues de falla" << m_ip << m_name;
        m_reintentandoConsulta=true;
    }

    m_connected = true;
    nextProcess();
}

void Queries::processPlatform()
{
    queryTimer->stop();
    m_contieneconsultas=true;
    if ( m_platform.isEmpty() )
    {
        PlatformInfo *pi=dynamic_cast<PlatformInfo*>(m_currentFuncion);
        m_platform=pi->platformInfo();
        m_xr_location=pi->xr_location();
        m_location=pi->location();
        m_equipmenttype=equipmentOSFromPlatform( m_platform );
    }
    nextProcess();
}

void Queries::processConfigFinished()
{
    Config *configQuery=dynamic_cast<Config*>(m_currentFuncion);
    if ( configQuery->error() )
    {
        m_error = true;
        finished(this);
        return;
    }

    queryTimer->stop();
    nextProcess();
}

void Queries::processFinished()
{
    qCDebug(queries) << m_ip  << "Queries::processFinished()";
    m_contieneconsultas=true;
    queryTimer->stop();
    nextProcess();
}

void Queries::processExit()
{
    qCDebug(queries) << m_ip  << "Queries::processExit()";
    borrarTerminal();
    queryTimer->stop();
    nextProcess();
}

void Queries::processKeepWorking()
{
    queryTimer->stop();
    queryTimer->start();
}

void Queries::on_query_lastCommand(QString txt)
{
    emit lastCommand(this,txt);
}

void Queries::on_queryTimer_timeout()
{
    qCDebug(queries) << m_ip  << "Queries::on_queryTimer_timeout()" << m_ip << m_name;

    //caso equipos que al salir de ellos no regresan al prompt del SO, simulamos que regreso para que se
    //envie la señal de finalizacion
    if ( m_opcionActual >= Exit )
    {
        nextProcess();
        return;
    }

    //timeout consultas normales
    processConnectToHostDisconnected();
}

QMap<QString,QString> Queries::queriesArgumentosAceptados()
{
    QMap<QString,QString> map;
    map.insert("ARP_MacIP","Indicar la MAC o IP en una consulta de ARP: 192.168.1.1 o 044e.0676.12bc");
    map.insert("Arp_VRFs","Indicar las VRFs en una consulta de ARP, VRFs separadas por comas: VRF1,VRF2,VRF3");
    map.insert("BGPNeig_Type","Indicar la familia: VPNV4");
    map.insert("BGPNetworks_Community","Indicar la comunidad de las redes a consultar");
    map.insert("BGPNetworks_NeighborIP","Indicar la IP del vecino BGP. No utilizar BGPNetworks_Community");
    map.insert("BGPNetworks_VRF","Indicar la VRF en la consulta de redes en un vecino BGP. No utilizar BGPNetworks_Community");
    map.insert("BGPNetworks_NeighborIn_Out","Indicar si se consulta las redes entrantes o salientes del vecino.");
    map.insert("IPRoutes_protocol","Indicar el protocolo para una consulta de tabla de enrutamiento: ospf static bgp");
    map.insert("IPRoutes_VRFs","Indicar las VRFs en una consulta de tablas de enrutamiento: VRF1,VRF2,VRF3");
    map.insert("MAC_MAC","Indicar la MAC en una consulta de tabla de MAC: 044e.0676.12bc");
    map.insert("VRFfRT_RT","Indicar la RT para una consulta donde se quiere saber la VRF desde la RT: 6458:17350");
    map.insert("VRFfVlans_Vlans","Indicar las Vlans para una consulta donde se quiere saber las VRFs a las que pertenecen: 136,3019,456,122");
    map.insert("Funcion_txt","Indicar el comando a ejecutar. Se devuelve el texto obtenido del equipo");
    return map;
}

void Queries::updateInfoQueries(QList<Queries> &lstDest, QList<Queries> &lstOrigin , QStringList lstIPsConsulta)
{
    //origen = nuevo
    //destino = anterior

    //borramos las consultas veijas y ponemos en false el operativo de todos los equipos que se consultaron, al
    //realizar la fusión si hay una nueva consulta se pondra en true
    for ( int c=0; c<lstDest.size(); )
    {
        Queries &dest = lstDest[c];
        if ( dest.m_datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.m_operativo)
            lstDest.removeAt( c );
        else
        {
            if ( lstIPsConsulta.contains( dest.m_ip ) )
               dest.m_operativo=false;
            c++;
        }
    }

    for (Queries &origin : lstOrigin)
    {
        bool encontrado=false;
        for (Queries &dest : lstDest)
        {
            //Se busca coincidencia de IP o nombre, porque puede que uno haya cambiado
            //Esto para que no se pierda el equipo y se agregue uno nuevo duplicando asi los equipos.
            //Esto es pensando que nunca va a cambiar la IP y el nombre de equipo al mismo tiempo.
            if ( (dest.ip() == origin.ip() ||
                 dest.hostName() == origin.hostName()) && origin.operativo() )
            {
                dest.m_name = origin.m_name;
                dest.m_fullName = origin.m_fullName;
                dest.m_country = origin.m_country;
                dest.m_platform = origin.m_platform;
                dest.m_brand = origin.m_brand;
                dest.m_datetime = origin.m_datetime;
                dest.m_operativo = origin.m_operativo;
                dest.m_xr_location = origin.m_xr_location;
                dest.m_location = origin.m_location;
                dest.m_ip = origin.m_ip;

                if ( origin.equipmentNeighborsInfoQuery )
                {
                    if ( dest.equipmentNeighborsInfoQuery )
                        updateInfoList( dest.equipmentNeighborsInfo(), origin.equipmentNeighborsInfo() );
                    else
                        dest.equipmentNeighborsInfoQuery = origin.equipmentNeighborsInfoQuery;
                }
                if ( origin.interfacesInfoQuery )
                {
                    if ( dest.interfacesInfoQuery )
                        updateInfoList( dest.interfacesInfo(), origin.interfacesInfo() );
                    else
                        dest.interfacesInfoQuery = origin.interfacesInfoQuery;
                }
                if ( origin.interfacesPermitedVlansQuery )
                {
                    if ( dest.interfacesPermitedVlansQuery )
                        updateInfoList( dest.interfacesPermitedVlansInfo(), origin.interfacesPermitedVlansInfo() );
                    else
                        dest.interfacesPermitedVlansQuery = origin.interfacesPermitedVlansQuery;
                }
                if ( origin.interfacesDescriptionsQuery )
                {
                    if ( dest.interfacesDescriptionsQuery )
                        updateInfoList( dest.interfacesDescriptionsInfo(), origin.interfacesDescriptionsInfo() );
                    else
                        dest.interfacesDescriptionsQuery = origin.interfacesDescriptionsQuery;
                }
                if ( origin.interfacesIpAddressesQuery )
                {
                    if ( dest.interfacesIpAddressesQuery )
                        updateInfoList( dest.interfacesIpAddressesInfo(), origin.interfacesIpAddressesInfo() );
                    else
                        dest.interfacesIpAddressesQuery = origin.interfacesIpAddressesQuery;
                }
                if ( origin.ospfQuery )
                {
                    if ( dest.ospfQuery )
                    {
                        dest.ospfQuery->m_abr = origin.ospfQuery->m_abr; //TODO cambiar a algo final en cada clase
                        dest.ospfQuery->m_asbr = origin.ospfQuery->m_asbr; //TODO cambiar a algo final en cada clase
                        updateInfoList( dest.ospfInfo(), origin.ospfInfo() );
                    }
                    else
                        dest.ospfQuery = origin.ospfQuery ;
                }
                if ( origin.mplsTEtunnelsQuery )
                {
                    if ( dest.mplsTEtunnelsQuery )
                        updateInfoList( dest.mplsTETunnelsInfo(), origin.mplsTETunnelsInfo() );
                    else
                        dest.mplsTEtunnelsQuery = origin.mplsTEtunnelsQuery;
                }
                if ( origin.mplsL2TransportQuery )
                {
                    if ( dest.mplsL2TransportQuery )
                    {
                        updateInfoList( dest.mplsL2TransportXconnectsInfo(), origin.mplsL2TransportXconnectsInfo() );
                        updateInfoList( dest.mplsL2TransportVFIsInfo(), origin.mplsL2TransportVFIsInfo() );
                    }
                    else
                        dest.mplsTEtunnelsQuery = origin.mplsTEtunnelsQuery;
                }
                if ( origin.mplsLdpDiscoveryQuery )
                {
                    if ( dest.mplsLdpDiscoveryQuery )
                    {
                        dest.mplsLdpLocalIDInfo() = origin.mplsLdpLocalIDInfo();
                        updateInfoList( dest.mplsLdpDiscoveryInfo(), origin.mplsLdpDiscoveryInfo() );
                    }
                    else
                        dest.mplsLdpDiscoveryQuery = origin.mplsLdpDiscoveryQuery;
                }
                if ( origin.mplsLdpNeighborsQuery )
                {
                    if ( dest.mplsLdpNeighborsQuery )
                        updateInfoList( dest.mplsLdpNeighborsInfo(), origin.mplsLdpNeighborsInfo() );
                    else
                        dest.mplsLdpNeighborsQuery = origin.mplsLdpNeighborsQuery;
                }
                if ( origin.mplsLdpInterfacesQuery )
                {
                    if ( dest.mplsLdpInterfacesQuery )
                        updateInfoList( dest.mplsLdpInterfacesInfo(), origin.mplsLdpInterfacesInfo() );
                    else
                        dest.mplsLdpInterfacesQuery = origin.mplsLdpInterfacesQuery;
                }
                if ( origin.pimInteracesQuery )
                {
                    if ( dest.pimInteracesQuery )
                        updateInfoList( dest.pimInterfacesInfo(), origin.pimInterfacesInfo() );
                    else
                        dest.pimInteracesQuery = origin.pimInteracesQuery;
                }
                if ( origin.macsQuery )
                {
                    if ( dest.macsQuery )
                        updateInfoList( dest.macInfo(), origin.macInfo() );
                    else
                        dest.macsQuery = origin.macsQuery;
                }
                if ( origin.portChannelInfoQuery )
                {
                    if ( dest.portChannelInfoQuery )
                        updateInfoList( dest.portChannelInfo(), origin.portChannelInfo() );
                    else
                        dest.portChannelInfoQuery = origin.portChannelInfoQuery;
                }
                if ( origin.vrfsFromVlansQuery )
                {
                    if ( dest.vrfsFromVlansQuery )
                        dest.vrfsFromVlansInfo() = origin.vrfsFromVlansInfo();
                    else
                        dest.vrfsFromVlansQuery = origin.vrfsFromVlansQuery;
                }
                if ( origin.vrfFromRTQuery )
                {
                    if ( dest.vrfFromRTQuery )
                        dest.vrfFromRTInfo() = origin.vrfFromRTInfo();
                    else
                        dest.vrfFromRTQuery = origin.vrfFromRTQuery;
                }
                if ( origin.vrfsQuery )
                {
                    if ( dest.vrfsQuery )
                        updateInfoList( dest.vrfsInfo(), origin.vrfsInfo() );
                    else
                        dest.vrfsQuery = origin.vrfsQuery;
                }
                if ( origin.arpsQuery )
                {
                    if ( dest.arpsQuery )
                        updateInfoList( dest.arpsInfo(), origin.arpsInfo() );
                    else
                        dest.arpsQuery = origin.arpsQuery;
                }
                if ( origin.bgpNeighborsQuery )
                {
                    if ( dest.bgpNeighborsQuery )
                        dest.bgpNeighborsInfo() = origin.bgpNeighborsInfo();
                    else
                        dest.bgpNeighborsQuery = origin.bgpNeighborsQuery;
                }
                if ( origin.bgpNetworksQuery )
                {
                    if ( dest.bgpNetworksQuery )
                        dest.bgpNetworksInfo() = origin.bgpNetworksInfo();
                    else
                        dest.bgpNetworksQuery = origin.bgpNetworksQuery;
                }
                if ( origin.ipRoutesQuery )
                {
                    if ( dest.ipRoutesQuery )
                        dest.ipRoutesInfo() = origin.ipRoutesInfo();
                    else
                        dest.ipRoutesQuery = origin.ipRoutesQuery;
                }
                if ( origin.funcionQuery )
                {
                    if ( dest.funcionQuery )
                        dest.funcionTxtInfo() = origin.funcionTxtInfo();
                    else
                        dest.funcionQuery = origin.funcionQuery;
                }

                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            lstDest.append( origin );
    }
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const Queries& query)
{
    //guardar

    out << query.m_name;
    out << query.m_fullName;
    out << query.m_country;
    out << query.m_platform;
    out << query.m_xr_location;
    out << query.m_location;
    out << query.m_brand;
    out << query.m_datetime;
    out << query.m_operativo;
    out << query.m_ip;

    if ( query.equipmentNeighborsInfoQuery )
    {
        out << true;
        out << query.equipmentNeighborsInfoQuery;
    }
    else
        out << false;
    if ( query.interfacesInfoQuery )
    {
        out << true;
        out << query.interfacesInfoQuery;
    }
    else
        out << false;
    if ( query.interfacesDescriptionsQuery )
    {
        out << true;
        out << query.interfacesDescriptionsQuery;
    }
    else
        out << false;
    if ( query.interfacesIpAddressesQuery )
    {
        out << true;
        out << query.interfacesIpAddressesQuery;
    }
    else
        out << false;
    if ( query.interfacesPermitedVlansQuery )
    {
        out << true;
        out << query.interfacesPermitedVlansQuery;
    }
    else
        out << false;
    if ( query.ospfQuery )
    {
        out << true;
        out << query.ospfQuery;
    }
    else
        out << false;    
    if ( query.mplsTEtunnelsQuery )
    {
        out << true;
        out << query.mplsTEtunnelsQuery;
    }
    else
        out << false;
    if ( query.mplsL2TransportQuery )
    {
        out << true;
        out << query.mplsL2TransportQuery;
    }
    else
        out << false;
    if ( query.mplsLdpDiscoveryQuery )
    {
        out << true;
        out << query.mplsLdpDiscoveryQuery;
    }
    else
        out << false;
    if ( query.mplsLdpNeighborsQuery )
    {
        out << true;
        out << query.mplsLdpNeighborsQuery;
    }
    else
        out << false;
    if ( query.mplsLdpInterfacesQuery )
    {
        out << true;
        out << query.mplsLdpInterfacesQuery;
    }
    else
        out << false;
//    if ( query.pimNeighborsQuery )
//    {
//        out << true;
//        out << query.pimNeighborsQuery;
//    }
//    else
//        out << false;
    if ( query.pimInteracesQuery )
    {
        out << true;
        out << query.pimInteracesQuery;
    }
    else
        out << false;
    if ( query.macsQuery )
    {
        out << true;
        out << query.macsQuery;
    }
    else
        out << false;
    if ( query.portChannelInfoQuery )
    {
        out << true;
        out << query.portChannelInfoQuery;
    }
    else
        out << false;
    if ( query.vrfsFromVlansQuery )
    {
        out << true;
        out << query.vrfsFromVlansQuery;
    }
    else
        out << false;
    if ( query.vrfFromRTQuery )
    {
        out << true;
        out << query.vrfFromRTQuery;
    }
    else
        out << false;
    if ( query.vrfsQuery )
    {
        out << true;
        out << query.vrfsQuery;
    }
    else
        out << false;
    if ( query.arpsQuery )
    {
        out << true;
        out << query.arpsQuery;
    }
    else
        out << false;
    if ( query.bgpNeighborsQuery )
    {
        out << true;
        out << query.bgpNeighborsQuery;
    }
    else
        out << false;
    if ( query.bgpNetworksQuery )
    {
        out << true;
        out << query.bgpNetworksQuery;
    }
    else
        out << false;
    if ( query.ipRoutesQuery )
    {
        out << true;
        out << query.ipRoutesQuery;
    }
    else
        out << false;
    if ( query.funcionQuery )
    {
        out << true;
        out << query.funcionQuery;
    }
    else
        out << false;

    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, Queries& query)
{
    //abrir

    bool a;

    in >> query.m_name;
    in >> query.m_fullName;
    in >> query.m_country;
    in >> query.m_platform;
    in >> query.m_xr_location;
    in >> query.m_location;
    in >> query.m_brand;
    in >> query.m_datetime;
    in >> query.m_operativo;
    in >> query.m_ip;

    in >> a;
    if ( a )
        in >> query.equipmentNeighborsInfoQuery;
    in >> a;
    if ( a )
        in >> query.interfacesInfoQuery;
    in >> a;
    if ( a )
        in >> query.interfacesDescriptionsQuery;
    in >> a;
    if ( a )
        in >> query.interfacesIpAddressesQuery;
    in >> a;    
    if ( a )
        in >> query.interfacesPermitedVlansQuery;
    in >> a;
    if ( a )
        in >> query.ospfQuery;
    in >> a;
    if ( a )
        in >> query.mplsTEtunnelsQuery;
    in >> a;
    if ( a )
        in >> query.mplsL2TransportQuery;
    in >> a;
    if ( a )
        in >> query.mplsLdpDiscoveryQuery;
    in >> a;
    if ( a )
        in >> query.mplsLdpNeighborsQuery;
    in >> a;
    if ( a )
        in >> query.mplsLdpInterfacesQuery;
//    in >> a;
//    if ( a )
//    {
//        in >> query.pimNeighborsQuery;
//    }
    in >> a;
    if ( a )
        in >> query.pimInteracesQuery;
    in >> a;
    if ( a )
        in >> query.macsQuery;
    in >> a;
    if ( a )
        in >> query.portChannelInfoQuery;
    in >> a;
    if ( a )
        in >> query.vrfsFromVlansQuery;
    in >> a;
    if ( a )
        in >> query.vrfFromRTQuery;
    in >> a;
    if ( a )
        in >> query.vrfsQuery;
    in >> a;
    if ( a )
        in >> query.arpsQuery;
    in >> a;
    if ( a )
        in >> query.bgpNeighborsQuery;
    in >> a;
    if ( a )
        in >> query.bgpNetworksQuery;
    in >> a;
    if ( a )
        in >> query.ipRoutesQuery;
    in >> a;
    if ( a )
        in >> query.funcionQuery;

    return in;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const Queries*& query)
{
    out << *query;
    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, Queries* query)
{
    query = new Queries;
    in >> *query;
    return in;
}

QNETWORKCLIQUERIES_EXPORT QDebug operator<<(QDebug dbg, const Queries &info)
{
    dbg.space() << "QueryInfo Thread: " << QThread::currentThread() << "\n";
    dbg.space() << "QueryInfo: " << info.m_name << info.m_ip << info.m_brand << info.m_platform;
    dbg.space() << "Location: " << info.m_location;
    dbg.space() << "XR Active RP location: " << info.m_xr_location;
    dbg.space() << "\nQueryDate: " << info.m_datetime.toString("yyyy-MM-dd_hh:mm:ss");
    dbg.space() << "\nConectado: " << info.m_operativo;
    dbg.space() << "\nError: " << info.m_error;    
    dbg.space() << "\n\n";

    if ( info.equipmentNeighborsInfoQuery )
        dbg.space() << "equipmentNeighborsInfoQuery" << *info.equipmentNeighborsInfoQuery;

    if ( info.interfacesInfoQuery )
        dbg.space() << "interfacesInfoQuery" << *info.interfacesInfoQuery;

    if ( info.interfacesPermitedVlansQuery )
        dbg.space() << "interfacesPermitedVlansQuery" << *info.interfacesPermitedVlansQuery;

    if ( info.interfacesDescriptionsQuery )
        dbg.space() << "interfacesDescriptionsQuery" << *info.interfacesDescriptionsQuery;

    if ( info.interfacesIpAddressesQuery )
        dbg.space() << "interfacesIpAddressesQuery" << *info.interfacesIpAddressesQuery;

    if ( info.ospfQuery )
        dbg.space() << "ospfQuery" << *info.ospfQuery;

    if ( info.mplsTEtunnelsQuery )
        dbg.space() << "mplsTEtunnelsQuery" << *info.mplsTEtunnelsQuery;

    if ( info.mplsL2TransportQuery )
        dbg.space() << "mplsL2TransportQuery" << *info.mplsL2TransportQuery;

    if ( info.mplsLdpDiscoveryQuery )
        dbg.space() << "mplsLdpDiscoveryQuery" << *info.mplsLdpDiscoveryQuery;

    if ( info.mplsLdpNeighborsQuery )
        dbg.space() << "mplsLdpNeighborsQuery" << *info.mplsLdpNeighborsQuery;

    if ( info.mplsLdpInterfacesQuery )
        dbg.space() << "mplsLdpInterfacesQuery" << *info.mplsLdpInterfacesQuery;

    if ( info.pimInteracesQuery )
        dbg.space() << "pimInteracesQuery" << *info.pimInteracesQuery;

    if ( info.macsQuery )
        dbg.space() << "macsQuery" << *info.macsQuery;

    if ( info.portChannelInfoQuery )
        dbg.space() << "portChannelInfoQuery" << *info.portChannelInfoQuery;

    if ( info.vrfsFromVlansQuery )
        dbg.space() << "vrfsFromVlansQuery" << *info.vrfsFromVlansQuery;

    if ( info.vrfFromRTQuery )
        dbg.space() << "vrfFromRTQuery" << *info.vrfFromRTQuery;

    if ( info.vrfsQuery )
        dbg.space() << "vrfsQuery" << *info.vrfsQuery;

    if ( info.arpsQuery )
        dbg.space() << "arpsQuery" << *info.arpsQuery;

    if ( info.bgpNeighborsQuery )
        dbg.space() << "bgpNeighborsQuery" << *info.bgpNeighborsQuery;

    if ( info.bgpNetworksQuery )
        dbg.space() << "bgpNetworksQuery" << *info.bgpNetworksQuery;

    if ( info.ipRoutesQuery )
        dbg.space() << "ipRoutesQuery" << *info.ipRoutesQuery;

    if ( info.configQuery )
        dbg.space() << "Configuration" << *info.configQuery;

    if ( info.funcionQuery )
        dbg.space() << "Funcion" << *info.funcionQuery;

    dbg.nospace() << "--\n\n";

    return dbg.maybeSpace();
}

QNETWORKCLIQUERIES_EXPORT bool Queries::operator==(Queries& other)
{
    return ( m_ip == other.m_ip &&
             m_name == other.m_name &&
             m_id == other.m_id &&
             m_brand == other.m_brand &&
             m_platform == other.m_platform &&
             m_country == other.m_country &&
             m_datetime == other.m_datetime &&
             m_ipreachable == other.m_ipreachable);
}

QNETWORKCLIQUERIES_EXPORT  Queries& Queries::operator=(const Queries& other)
{
    iniciar();
    clone(other);
    return *this;
}

LstQueries::LstQueries(const LstQueries &lstOrigen)
{
    grupo = lstOrigen.grupo;
    opcionesConsulta = lstOrigen.opcionesConsulta;
    gw = lstOrigen.gw;
    lstQueries = lstOrigen.lstQueries;
    lstIPsAconsultadas = lstOrigen.lstIPsAconsultadas;
    lstIPsConectadasPorGW = lstOrigen.lstIPsConectadasPorGW;
    errorMap = lstOrigen.errorMap;
    lstQueriesParameters = lstOrigen.lstQueriesParameters;
    dateTime = lstOrigen.dateTime;
    label = lstOrigen.label;
    tipoconsulta = lstOrigen.tipoconsulta;
}

LstQueries::~LstQueries()
{}

bool LstQueries::isEmpty()
{
    if ( opcionesConsulta.isEmpty() && lstQueries.isEmpty() )
        return false;

    return true;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const LstQueries& lstquery)
{
    //guardar
    out << lstquery.grupo;
    out << lstquery.opcionesConsulta;
    out << lstquery.lstQueries;
    out << lstquery.errorMap;
    out << lstquery.dateTime;
    out << lstquery.label;
    out << lstquery.tipoconsulta;
    out << lstquery.lstQueriesParameters;
    out << lstquery.lstIPsAconsultadas;
    out << lstquery.gw;
    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, LstQueries& lstquery)
{
    //abrir
    in >> lstquery.grupo;
    in >> lstquery.opcionesConsulta;
    in >> lstquery.lstQueries;
    in >> lstquery.errorMap;
    in >> lstquery.dateTime;
    in >> lstquery.label;
    in >> lstquery.tipoconsulta;
    in >> lstquery.lstQueriesParameters;
    in >> lstquery.lstIPsAconsultadas;
    in >> lstquery.gw;
    return in;
}

QNETWORKCLIQUERIES_EXPORT QDebug operator<<(QDebug dbg, const LstQueries &info)
{
    QStringList lstNoConexion;
    QStringList lstConexionError;
    QStringList lstErrores;

    QMapIterator<QString, QString> i(info.errorMap);
    while (i.hasNext())
    {
        i.next();
        if ( i.value() == "No conexion" )
            lstNoConexion.append( i.key() );
        else if ( i.value() == "Conexion Error" )
            lstConexionError.append( i.key() );
        else if ( i.value() == "Error" )
            lstErrores.append( i.key() );
    }

    dbg.nospace() << "LstQueries\n\n";
    dbg.space() << "Grupo" << info.grupo << "\n";
    dbg.space() << "Fecha" << info.dateTime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";
    dbg.space() << "Label" << info.label << "\n";
    dbg.space() << "Grupo" << info.tipoconsulta << "\n";
    dbg.space() << "OpcionesConsulta" << info.opcionesConsulta << "\n";
    dbg.space() << "GW" << info.gw << "\n\n";
    dbg.space() << "ParametrosMap:\n";

    for ( QueriesConfigurationValue qcv : info.lstQueriesParameters )
        dbg.space() << qcv;

    dbg.space() << "\n";
    dbg.space() << "errorMap:\n";
    dbg.space() << "No Conexion:\n";
    foreach (QString s, lstNoConexion)
        dbg.space() << s << "\n";
    dbg.space() << "--\n";
    dbg.space() << "Conexion Error:\n";
    foreach (QString s, lstConexionError)
        dbg.space() << s << "\n";
    dbg.space() << "--\n";
    dbg.space() << "Errores:\n";
    foreach (QString s, lstErrores)
        dbg.space() << s << "\n";
    dbg.space() << "--\n";
    dbg.space() << "\nIPs consultadas:" << info.lstIPsAconsultadas.size() << "\n";
    dbg.space() << info.lstIPsAconsultadas;
    dbg.space() << "--\n";
    dbg.space() << "\nIPs consultadas por medio de GW:" << info.lstIPsConectadasPorGW.size() << "\n";
    dbg.space() << info.lstIPsConectadasPorGW;
    return dbg.maybeSpace();
}


