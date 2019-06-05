#include "queries.h"

#include <QThread>
#include <QEventLoop>

#include "funciones.h"
#include "bdhosts.h"
#include "factory.h"

Queries::Queries(QString IP, QString user, QString pwd, QObject *parent) : QObject(parent)
{    
    iniciar();
    m_ip=IP;
    m_user=user;
    m_pwd=pwd;   
}

Queries::Queries()
{
    iniciar();
}

Queries::Queries(const Queries &other) : QObject(other.parent())
{
    iniciar();
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
    m_ip = other.m_ip;
    m_user = other.m_user;
    m_gw = other.m_gw;
    m_pwd = other.m_pwd;

    if ( other.pi ) pi = new PlatformInfo( *other.pi );
    if ( other.equipmentNeighborsInfoQuery ) equipmentNeighborsInfoQuery = new EquipmentNeighborsInfo( *other.equipmentNeighborsInfoQuery );
    if ( other.interfacesInfoQuery ) interfacesInfoQuery = new InterfaceInfo( *other.interfacesInfoQuery );
    if ( other.interfacesPermitedVlansQuery ) interfacesPermitedVlansQuery = new InterfaceInfo( *other.interfacesPermitedVlansQuery );
    if ( other.interfacesDescriptionsQuery ) interfacesDescriptionsQuery = new InterfaceInfo( *other.interfacesDescriptionsQuery );
    if ( other.interfacesIpAddressesQuery ) interfacesIpAddressesQuery = new InterfaceInfo( *other.interfacesIpAddressesQuery );
    if ( other.ospfQuery ) ospfQuery = new OSPFInfo( *other.ospfQuery );
    if ( other.mplsLdpDiscoveryQuery ) mplsLdpDiscoveryQuery = new MplsLdpInfo( *other.mplsLdpDiscoveryQuery );
    if ( other.mplsLdpNeighborsQuery ) mplsLdpNeighborsQuery = new MplsLdpInfo( *other.mplsLdpNeighborsQuery );
    if ( other.mplsLdpInterfacesQuery ) mplsLdpInterfacesQuery = new MplsLdpInfo( *other.mplsLdpInterfacesQuery );
    if ( other.pimInteracesQuery ) pimInteracesQuery = new PIMInfo( *other.pimInteracesQuery );
    if ( other.macsQuery ) macsQuery = new MacInfo( *other.macsQuery );
    if ( other.portChannelInfoQuery ) portChannelInfoQuery = new PortChannelsInfo( *other.portChannelInfoQuery );
    if ( other.vrfsFromVlansQuery ) vrfsFromVlansQuery = new VrfInfo( *other.vrfsFromVlansQuery );
    if ( other.vrfFromRTQuery ) vrfFromRTQuery = new VrfInfo( *other.vrfFromRTQuery );
    if ( other.vrfsQuery ) vrfsQuery = new VrfInfo( *other.vrfsQuery );
    if ( other.arpsQuery ) arpsQuery = new ArpInfo( *other.arpsQuery );
    if ( other.bgpNeighborsQuery ) bgpNeighborsQuery = new BGPNeighborInfo( *other.bgpNeighborsQuery );
    if ( other.ipRoutesQuery ) ipRoutesQuery = new IPRouteInfo( *other.ipRoutesQuery );
}

Queries::~Queries()
{
    limpiarConsultas();
}

void Queries::iniciar()
{    
    term = nullptr;
    pi = nullptr;
    m_keepAliveTimer=nullptr;
    equipmentNeighborsInfoQuery = nullptr;
    interfacesInfoQuery = nullptr;
    interfacesPermitedVlansQuery = nullptr;
    interfacesDescriptionsQuery = nullptr;
    interfacesIpAddressesQuery = nullptr;
    ospfQuery = nullptr;
    mplsLdpDiscoveryQuery = nullptr;
    mplsLdpNeighborsQuery = nullptr;
    mplsLdpInterfacesQuery = nullptr;
//    pimNeighborsQuery = nullptr;
    pimInteracesQuery = nullptr;
    macsQuery = nullptr;
    portChannelInfoQuery = nullptr;
    vrfsFromVlansQuery = nullptr;
    vrfFromRTQuery = nullptr;
    vrfsQuery = nullptr;
    arpsQuery = nullptr;
    bgpNeighborsQuery = nullptr;
    ipRoutesQuery = nullptr;
    configQuery = nullptr;
    exitQuery = nullptr;
    m_connectionprotol = QRemoteShell::TelnetSSH;    

    m_connected=false;
    m_error=false;
    m_queriescreated=false;
    m_ipreachable=false;    
    m_conectionSecondTriedOtherUserPassword=false;    
    m_reintentandoConsulta=false;
    m_contieneconsultas=false;
    m_consultaIntentos=0;     
    opcionActual=0;
    flags=0;
    m_operativo=true;
    m_datetime = QDateTime::currentDateTime();

    queryTimer = new QTimer(this);
    queryTimer->setInterval( 20000 );
    queryTimer->setSingleShot( true );
    connect(queryTimer,SIGNAL(timeout()),SLOT(on_queryTimer_timeout()));
}

void Queries::limpiarConsultas()
{
    if (pi)
    {
        delete pi;
        pi=nullptr;
    }
    if (equipmentNeighborsInfoQuery)
    {
        delete equipmentNeighborsInfoQuery;
        equipmentNeighborsInfoQuery=nullptr;
    }
    if (interfacesInfoQuery)
    {
        delete interfacesInfoQuery;
        interfacesInfoQuery=nullptr;
    }
    if (interfacesPermitedVlansQuery)
    {
        delete interfacesPermitedVlansQuery;
        interfacesPermitedVlansQuery=nullptr;
    }
    if (interfacesDescriptionsQuery)
    {
        delete interfacesDescriptionsQuery;
        interfacesDescriptionsQuery=nullptr;
    }
    if (interfacesIpAddressesQuery)
    {
        delete interfacesIpAddressesQuery;
        interfacesIpAddressesQuery=nullptr;
    }
    if (ospfQuery)
    {
        delete ospfQuery;
        ospfQuery=nullptr;
    }
    if (mplsLdpDiscoveryQuery)
    {
        delete mplsLdpDiscoveryQuery;
        mplsLdpDiscoveryQuery=nullptr;
    }
    if (mplsLdpNeighborsQuery)
    {
        delete mplsLdpNeighborsQuery;
        mplsLdpNeighborsQuery=nullptr;
    }
    if (mplsLdpInterfacesQuery)
    {
        delete mplsLdpInterfacesQuery;
        mplsLdpInterfacesQuery=nullptr;
    }
//    if (pimNeighborsQuery)
//        delete pimNeighborsQuery;
    if (pimInteracesQuery)
    {
        delete pimInteracesQuery;
        pimInteracesQuery=nullptr;
    }
    if (macsQuery)
    {
        delete macsQuery;
        macsQuery=nullptr;
    }
    if (portChannelInfoQuery)
    {
        delete portChannelInfoQuery;
        portChannelInfoQuery=nullptr;
    }
    if (vrfsFromVlansQuery)
    {
        delete vrfsFromVlansQuery;
        vrfsFromVlansQuery=nullptr;
    }
    if (vrfFromRTQuery)
    {
        delete vrfFromRTQuery;
        vrfFromRTQuery=nullptr;
    }
    if (vrfsQuery)
    {
        delete vrfsQuery;
        vrfsQuery=nullptr;
    }
    if (arpsQuery)
    {
        delete arpsQuery;
        arpsQuery=nullptr;
    }
    if (bgpNeighborsQuery)
    {
        delete bgpNeighborsQuery;
        bgpNeighborsQuery=nullptr;
    }
    if (ipRoutesQuery)
    {
        delete ipRoutesQuery;
        ipRoutesQuery=nullptr;
    }
    if (configQuery)
    {
        delete configQuery;
        configQuery=nullptr;
    }
    if (exitQuery)
    {
        delete exitQuery;
        exitQuery=nullptr;
    }
}

void Queries::clear()
{
    disconnect();    
    opcionActual=0;
    flags = 0;
    limpiarConsultas();
}

void Queries::setKeepAlive(bool enable)
{
    if ( enable )
    {
        if ( !m_keepAliveTimer )
        {
            m_keepAliveTimer = new QTimer();
            m_keepAliveTimer->setInterval( 300000 );
            connect(m_keepAliveTimer,SIGNAL(timeout()),SLOT(on_m_keepAliveTimer_timeout()));
            m_keepAliveTimer->start();
        }
    }
    else
        m_keepAliveTimer->stop();
}

void Queries::setOptions(unsigned int options)
{
    opcionActual=0;
    flags = options;

    if ( m_connected )
    {
        //despues de realizar unas consultas y se quiere realizar otras en la misma conexion
        //eliminando las anteriores.
        m_queriescreated=false;
        limpiarConsultas();
        createQueries();
    }
}

//void Queries::addAndCreateOptions(unsigned int options)
//{
//    flags = flags | options;
//    createOptions();
//}

void Queries::setGW(QString GW)
{
    m_gw = GW;
}

void Queries::createQueries(Queries::Opcion option)
{
    if ( m_queriescreated )
        return;

    unsigned int oActual = option;
    if ( !oActual )
        oActual=Platform;

    do
    {
        if ( flags & Platform & oActual )
        {
            if (pi)
                delete pi;

            pi = new PlatformInfo(term,this);
        }
        else if ( flags & EquipmentNeighbors & oActual )
        {
            if (equipmentNeighborsInfoQuery)
                delete equipmentNeighborsInfoQuery;

            equipmentNeighborsInfoQuery = factoryNewEquipmentNeighborsInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & InterfaceInformation & oActual )
        {
            if (interfacesInfoQuery)
                delete interfacesInfoQuery;

            interfacesInfoQuery = factoryNewInterfaceInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & InterfacePermitedVlans & oActual )
        {
            if (interfacesPermitedVlansQuery)
                delete interfacesPermitedVlansQuery;

            interfacesPermitedVlansQuery = factoryNewInterfaceInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & InterfaceDescription & oActual )
        {
            if (interfacesDescriptionsQuery)
                delete interfacesDescriptionsQuery;

            interfacesDescriptionsQuery = factoryNewInterfaceInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & InterfaceIpAddresses & oActual )
        {
            if (interfacesIpAddressesQuery)
                delete interfacesIpAddressesQuery;

            interfacesIpAddressesQuery = factoryNewInterfaceInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & Ospf & oActual )
        {
            if (ospfQuery)
                delete ospfQuery;

            ospfQuery = factoryNewOSPFInfo(m_brand,m_equipmenttype,term,this);
        }        
        else if ( flags & MplsLdpDiscovery & oActual )
        {
            if (mplsLdpDiscoveryQuery)
                delete mplsLdpDiscoveryQuery;

            mplsLdpDiscoveryQuery = factoryNewMplsLdpInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & MplsLdpNeighbors & oActual )
        {
            if (mplsLdpNeighborsQuery)
                delete mplsLdpNeighborsQuery;

            mplsLdpNeighborsQuery = factoryNewMplsLdpInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & MplsLdpInterfaces & oActual )
        {
            if (mplsLdpInterfacesQuery)
                delete mplsLdpInterfacesQuery;

            mplsLdpInterfacesQuery = factoryNewMplsLdpInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & PimInterfaces & oActual )
        {
            if (pimInteracesQuery)
                delete pimInteracesQuery;

            pimInteracesQuery = factoryNewPIMInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & MacAddress & oActual )
        {
            if (macsQuery)
                delete macsQuery;

            macsQuery = factoryNewMacInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & PortChannel & oActual )
        {
            if (portChannelInfoQuery)
                delete portChannelInfoQuery;

            portChannelInfoQuery = factoryNewPortChannelsInfo(m_brand,m_equipmenttype,term,this);
        }

        else if ( flags & VRFfVlans & oActual )
        {
            if (vrfsFromVlansQuery)
                delete vrfsFromVlansQuery;

            vrfsFromVlansQuery = factoryNewVrfInfo(m_brand,m_equipmenttype,term,this);
        }

        else if ( flags & VRFfRT & oActual )
        {
            if (vrfFromRTQuery)
                delete vrfFromRTQuery;

            vrfFromRTQuery = factoryNewVrfInfo(m_brand,m_equipmenttype,term,this);
        }

        else if ( flags & VRFs & oActual )
        {
            if (vrfsQuery)
                delete vrfsQuery;

            vrfsQuery = factoryNewVrfInfo(m_brand,m_equipmenttype,term,this);
        }

        else if ( flags & Arp & oActual )
        {
            if (arpsQuery)
                delete arpsQuery;

            arpsQuery = factoryNewArpInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & BGPNeig & oActual )
        {
            if (bgpNeighborsQuery)
                delete bgpNeighborsQuery;

            bgpNeighborsQuery = factoryNewBGPNeighborInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & IpRoutes & oActual )
        {
            if (ipRoutesQuery)
                delete ipRoutesQuery;

            ipRoutesQuery = factoryNewIPRouteInfo(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & Configuration & oActual )
        {
            if (configQuery)
                delete configQuery;

            configQuery = factoryNewConfig(m_brand,m_equipmenttype,term,this);
        }
        else if ( flags & Exit & oActual )
        {
            if (exitQuery)
                delete exitQuery;

            exitQuery = factoryNewExit(m_brand,m_equipmenttype,term,this);
        }

        oActual = oActual << 1;
    }
    while (oActual <= Exit);

    m_queriescreated=true;
}

void Queries::conectarAequipo(QString ip,QString user, QString pwd, QString platform)
{
    //borramos la terminal con conexion fallida
    if ( term )
    {
        qDebug() << m_ip  << "borramos la terminal con conexion fallida" << m_ip << m_name;

        term->disconnectReceiveTextSignalConnections();
        term->disconnect();
        term->host_disconnect();
        delete term;
    }

    term = new QRemoteShell(ip,user,pwd,platform,this);
    term->setConnectionProtocol( m_connectionprotol );
    term->setGW(m_gw);
    term->setLogPath( m_logPath );
    connect(term,SIGNAL(reachable()),SLOT(processConnectToHostReachable()));
    connect(term,SIGNAL(connected()),SLOT(processConnectToHostConnected()));
    connect(term,SIGNAL(disconnected()),SLOT(processConnectToHostDisconnected()));
    term->host_connect();
}

void Queries::nextProcess()
{
    qDebug() << m_ip  << "Queries::nextProcess()";

    if ( !m_reintentandoConsulta )
    {
        do
        {
            //estableciendo la siguiente opcion que se ejecutara
            if ( ! opcionActual )
                //primera vez que se entra, se establece Connect para ser evaluada primero
                opcionActual = Connect;
            else
                opcionActual = opcionActual << 1;

            if ( opcionActual > Exit )
            {
                qDebug() << m_ip  << "Mayor a Exit" << m_ip;

                if ( m_logFile.isOpen() )
                    m_logFile.close();

                emit finished(this);
                disconnect();
                return;
            }
        }
        while( (flags & opcionActual) == 0 );
    }
    else
        m_queriescreated=false; //para que se creen nuevamente las consultas desde donde se quedo

    qDebug() << m_ip  << "NextProcess:" << opcionActual;
    saveLog( "\n\nNextProcess: " + QString::number(opcionActual)+"\n\n" );

    queryTimer->setInterval( 20000 );
    queryTimer->start();

    if ( term )
        connect(term,SIGNAL(readyRead()),SLOT(on_term_readyRead()));

    //consultas CLI    

    if ( opcionActual & flags & Connect )
    {        
        qDebug() << m_ip  << "creando term";
        saveLog( "creando term\n" );        

        term = new QRemoteShell(m_ip,m_user,m_pwd,m_platform,this);
        term->setConnectionProtocol( m_connectionprotol );
        term->setGW(m_gw);
        term->setLogPath( m_logPath );
        connect(term,SIGNAL(reachable()),SLOT(processConnectToHostReachable()));
        connect(term,SIGNAL(connected()),SLOT(processConnectToHostConnected()));
        connect(term,SIGNAL(disconnected()),SLOT(processConnectToHostDisconnected()));
        term->host_connect();        
        return;
    }

    if ( !m_connected )
        return;

    if ( opcionActual & flags & Platform )
    {                
        if ( m_platform.isEmpty() )
        {
            if ( m_reintentandoConsulta )
            {
                m_reintentandoConsulta=false;
                createQueries( Platform );
            }

            pi->setBrand(m_brand);
            pi->setHostName(m_fullName);
            pi->setIp(m_ip);
            pi->setLogPath( m_logPath );
            connect(pi,SIGNAL(processFinished()),SLOT(processPlatform()));
            connect(pi,SIGNAL(working()),SLOT(processKeepWorking()));
            connect(pi,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
            pi->getPlatformInfo();
        }
        else
        {
            if ( m_reintentandoConsulta )
                m_reintentandoConsulta=false;
            nextProcess();
        }
        return;
    }
    else if ( opcionActual & flags & EquipmentNeighbors )
    {        
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( EquipmentNeighbors );
        }

        equipmentNeighborsInfoQuery->setBrand(m_brand);
        equipmentNeighborsInfoQuery->setPlatform(m_platform);
        equipmentNeighborsInfoQuery->setXRLocation(m_xr_location);
        equipmentNeighborsInfoQuery->setHostName(m_fullName);
        equipmentNeighborsInfoQuery->setIp(m_ip);
        equipmentNeighborsInfoQuery->setLogPath( m_logPath );
        connect(equipmentNeighborsInfoQuery,SIGNAL(processFinished()),SLOT(processEquipmentNeighbors()));
        connect(equipmentNeighborsInfoQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(equipmentNeighborsInfoQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        equipmentNeighborsInfoQuery->getEquipmentNeighborsInfo();
        return;
    }
    else if ( opcionActual & flags & InterfaceInformation )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") )
            queryTimer->setInterval( 200000 );

        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( InterfaceInformation );
        }

        interfacesInfoQuery->setBrand(m_brand);
        interfacesInfoQuery->setPlatform( m_platform );
        interfacesInfoQuery->setXRLocation(m_xr_location);
        interfacesInfoQuery->setHostName(m_fullName);
        interfacesInfoQuery->setIp(m_ip);
        interfacesInfoQuery->setLogPath( m_logPath );
        connect(interfacesInfoQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(interfacesInfoQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(interfacesInfoQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        interfacesInfoQuery->getInterfacesInfo();
        return;
    }
    else if ( opcionActual & flags & InterfaceDescription )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") )
            queryTimer->setInterval( 200000 );

        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( InterfaceDescription );
        }

        interfacesDescriptionsQuery->setBrand(m_brand);
        interfacesDescriptionsQuery->setPlatform( m_platform );
        interfacesDescriptionsQuery->setXRLocation(m_xr_location);
        interfacesDescriptionsQuery->setHostName(m_fullName);
        interfacesDescriptionsQuery->setIp(m_ip);
        interfacesDescriptionsQuery->setLogPath( m_logPath );
        connect(interfacesDescriptionsQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(interfacesDescriptionsQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(interfacesDescriptionsQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));        
        interfacesDescriptionsQuery->getInterfacesDescriptions();
        return;
    }
    else if ( opcionActual & flags & InterfaceIpAddresses )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") )
            queryTimer->setInterval( 200000 );

        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( InterfaceIpAddresses );
        }

        interfacesIpAddressesQuery->setBrand(m_brand);
        interfacesIpAddressesQuery->setPlatform( m_platform );
        interfacesIpAddressesQuery->setXRLocation(m_xr_location);
        interfacesIpAddressesQuery->setHostName(m_fullName);
        interfacesIpAddressesQuery->setIp(m_ip);
        interfacesIpAddressesQuery->setLogPath( m_logPath );
        connect(interfacesIpAddressesQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(interfacesIpAddressesQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(interfacesIpAddressesQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        interfacesIpAddressesQuery->getInterfacesIpAddresses();
        return;
    }
    else if ( opcionActual & flags & InterfacePermitedVlans )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") )
            queryTimer->setInterval( 200000 );

        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( InterfacePermitedVlans );
        }

        interfacesPermitedVlansQuery->setBrand(m_brand);
        interfacesPermitedVlansQuery->setPlatform( m_platform );
        interfacesPermitedVlansQuery->setXRLocation(m_xr_location);
        interfacesPermitedVlansQuery->setHostName(m_fullName);
        interfacesPermitedVlansQuery->setIp(m_ip);
        interfacesPermitedVlansQuery->setLogPath( m_logPath );
        connect(interfacesPermitedVlansQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(interfacesPermitedVlansQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(interfacesPermitedVlansQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        interfacesPermitedVlansQuery->getInterfacesPermitedVlans();
        return;
    }
    else if (  opcionActual & flags & Ospf )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( Ospf );
        }

        ospfQuery->setPlatform(m_platform);
        ospfQuery->setXRLocation(m_xr_location);
        ospfQuery->setBrand(m_brand);
        ospfQuery->setHostName(m_fullName);
        ospfQuery->setIp(m_ip);
        ospfQuery->setLogPath( m_logPath );
        connect(ospfQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(ospfQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(ospfQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        ospfQuery->getOSPFInfo();
        return;
    }    
    else if (  opcionActual & flags & MplsLdpDiscovery )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( MplsLdpDiscovery );
        }

        mplsLdpDiscoveryQuery->setPlatform(m_platform);
        mplsLdpDiscoveryQuery->setXRLocation(m_xr_location);
        mplsLdpDiscoveryQuery->setBrand(m_brand);
        mplsLdpDiscoveryQuery->setHostName(m_fullName);
        mplsLdpDiscoveryQuery->setIp(m_ip);
        mplsLdpDiscoveryQuery->setLogPath( m_logPath );
        connect(mplsLdpDiscoveryQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(mplsLdpDiscoveryQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(mplsLdpDiscoveryQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        mplsLdpDiscoveryQuery->getMplsLdpDiscovery();
        return;
    }
    else if (  opcionActual & flags & MplsLdpNeighbors )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( MplsLdpNeighbors );
        }

        mplsLdpNeighborsQuery->setPlatform(m_platform);
        mplsLdpNeighborsQuery->setXRLocation(m_xr_location);
        mplsLdpNeighborsQuery->setBrand(m_brand);
        mplsLdpNeighborsQuery->setHostName(m_fullName);
        mplsLdpNeighborsQuery->setIp(m_ip);
        mplsLdpNeighborsQuery->setLogPath( m_logPath );
        connect(mplsLdpNeighborsQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(mplsLdpNeighborsQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(mplsLdpNeighborsQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        mplsLdpNeighborsQuery->getMplsLdpNeighbors();
        return;
    }
    else if (  opcionActual & flags & MplsLdpInterfaces )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( MplsLdpInterfaces );
        }

        mplsLdpInterfacesQuery->setPlatform(m_platform);
        mplsLdpInterfacesQuery->setXRLocation(m_xr_location);
        mplsLdpInterfacesQuery->setBrand(m_brand);
        mplsLdpInterfacesQuery->setHostName(m_fullName);
        mplsLdpInterfacesQuery->setIp(m_ip);
        mplsLdpInterfacesQuery->setLogPath( m_logPath );
        connect(mplsLdpInterfacesQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(mplsLdpInterfacesQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(mplsLdpInterfacesQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        mplsLdpInterfacesQuery->getMplsLdpInterfaces();
        return;
    }
//    if (  opcionActual & flags & PimNeighbors )
//    {
//        connect(pimNeighborsQuery,SIGNAL(processFinished()),SLOT(processFinished()));
//        pimNeighborsQuery->getPIMNeighbors();
//        return;
//    }
    else if (  opcionActual & flags & PimInterfaces )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( PimInterfaces );
        }

        pimInteracesQuery->setPlatform(m_platform);
        pimInteracesQuery->setXRLocation(m_xr_location);
        pimInteracesQuery->setBrand(m_brand);
        pimInteracesQuery->setHostName(m_fullName);
        pimInteracesQuery->setIp(m_ip);
        pimInteracesQuery->setLogPath( m_logPath );
        connect(pimInteracesQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(pimInteracesQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(pimInteracesQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        pimInteracesQuery->getPIMInterfaces();
        return;
    }
    else if ( opcionActual & flags & MacAddress )
    {
        if ( m_platform.contains("ASR9K") ||
             m_platform.contains("CRS")  ||
             m_platform.contains("NCS-6000") ||
             m_platform.contains("760"))
            queryTimer->setInterval( 200000 );

        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( MacAddress );
        }

        qDebug() << m_ip  << "Next: MacAddress";

        macsQuery->setBrand( m_brand );
        macsQuery->setPlatform( m_platform );        
        macsQuery->setXRLocation(m_xr_location);
        macsQuery->setHostName(m_fullName);
        macsQuery->setIp(m_ip);
        macsQuery->setLogPath( m_logPath );
        connect(macsQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(macsQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(macsQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        macsQuery->getMacInfo();
        return;
    }
    else if ( opcionActual & flags & PortChannel )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( PortChannel );
        }

        portChannelInfoQuery->setBrand(m_brand);
        portChannelInfoQuery->setPlatform( m_platform );
        portChannelInfoQuery->setXRLocation(m_xr_location);
        portChannelInfoQuery->setHostName(m_fullName);
        portChannelInfoQuery->setIp(m_ip);
        portChannelInfoQuery->setLogPath( m_logPath );
        connect(portChannelInfoQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(portChannelInfoQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(portChannelInfoQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        portChannelInfoQuery->getPortChannelsInfo();
        return;
    }
    else if ( opcionActual & flags & VRFfVlans )
    {        
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( VRFfVlans );
        }

        vrfsFromVlansQuery->setBrand( m_brand );
        vrfsFromVlansQuery->setPlatform( m_platform );
        vrfsFromVlansQuery->setXRLocation(m_xr_location);
        vrfsFromVlansQuery->setHostName(m_fullName);
        vrfsFromVlansQuery->setIp(m_ip);
        vrfsFromVlansQuery->setLogPath( m_logPath );
        connect(vrfsFromVlansQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(vrfsFromVlansQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(vrfsFromVlansQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        vrfsFromVlansQuery->getVRFsFromVLans();
        return;
    }
    else if ( opcionActual & flags & VRFfRT )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( VRFfRT );
        }

        vrfFromRTQuery->setBrand( m_brand );
        vrfFromRTQuery->setPlatform( m_platform );
        vrfFromRTQuery->setXRLocation(m_xr_location);
        vrfFromRTQuery->setHostName(m_fullName);
        vrfFromRTQuery->setIp(m_ip);
        vrfFromRTQuery->setLogPath( m_logPath );
        connect(vrfFromRTQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(vrfFromRTQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(vrfFromRTQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        vrfFromRTQuery->getVRFfromRT();
        return;
    }
    else if ( opcionActual & flags & VRFs )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( VRFs );
        }

        vrfsQuery->setBrand( m_brand );
        vrfsQuery->setPlatform( m_platform );
        vrfsQuery->setXRLocation(m_xr_location);
        vrfsQuery->setHostName(m_fullName);
        vrfsQuery->setIp(m_ip);
        vrfsQuery->setLogPath( m_logPath );
        connect(vrfsQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(vrfsQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(vrfsQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        vrfsQuery->getVRFs();
        return;
    }
    else if ( opcionActual & flags & Arp )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( Arp );
        }

        qDebug() << m_ip  << "empezando ARP";

        arpsQuery->setBrand(m_brand);
        arpsQuery->setPlatform(m_platform);
        arpsQuery->setXRLocation(m_xr_location);
        arpsQuery->setHostName(m_fullName);
        arpsQuery->setIp(m_ip);
        arpsQuery->setLogPath( m_logPath );
        connect(arpsQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(arpsQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(arpsQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        arpsQuery->getArpInfo();
        return;
    }
    else if ( opcionActual & flags & BGPNeig )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( BGPNeig );
        }

        bgpNeighborsQuery->setPlatform( m_platform );
        bgpNeighborsQuery->setXRLocation(m_xr_location);
        bgpNeighborsQuery->setBrand(m_brand);
        bgpNeighborsQuery->setHostName(m_fullName);
        bgpNeighborsQuery->setIp(m_ip);
        bgpNeighborsQuery->setLogPath( m_logPath );
        connect(bgpNeighborsQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(bgpNeighborsQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(bgpNeighborsQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        bgpNeighborsQuery->getBGPNeighbors();
        return;
    }
    else if ( opcionActual & flags & IpRoutes )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( IpRoutes );
        }

        ipRoutesQuery->setPlatform( m_platform );
        ipRoutesQuery->setXRLocation(m_xr_location);
        ipRoutesQuery->setBrand(m_brand);
        ipRoutesQuery->setHostName(m_fullName);
        ipRoutesQuery->setIp(m_ip);
        ipRoutesQuery->setLogPath( m_logPath );
        connect(ipRoutesQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(ipRoutesQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(ipRoutesQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        ipRoutesQuery->getIPRouteInfo();
        return;
    }
    else if ( opcionActual & flags & Configuration )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( Configuration );
        }        

        configQuery->setPlatform(m_platform);
        configQuery->setXRLocation(m_xr_location);
        configQuery->setBrand(m_brand);
        configQuery->setHostName(m_fullName);
        configQuery->setIp(m_ip);
        configQuery->setLogPath( m_logPath );
        connect(configQuery,SIGNAL(processFinished()),SLOT(processConfigFinished()));
        connect(configQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(configQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        configQuery->configApply();
        return;
    }
    else if ( opcionActual & flags & Exit )
    {
        if ( m_reintentandoConsulta )
        {
            m_reintentandoConsulta=false;
            createQueries( Exit );
        }

        qDebug() << m_ip  << "exit" << exitQuery;

        exitQuery->setPlatform(m_platform);
        exitQuery->setBrand(m_brand);
        exitQuery->setHostName(m_fullName);
        exitQuery->setIp(m_ip);
        exitQuery->setLogPath( m_logPath );
        connect(exitQuery,SIGNAL(processFinished()),SLOT(processFinished()));
        connect(exitQuery,SIGNAL(working()),SLOT(processKeepWorking()));
        connect(exitQuery,SIGNAL(lastCommand(QString)),SLOT(on_query_lastCommand(QString)));
        exitQuery->exit();
        return;
    }
}

void Queries::startSync() //Sync
{
    disconnect(); //desconectamos todas las señales antes configuradas
    QEventLoop loop;
    connect(this, SIGNAL(finished(Queries*)), &loop, SLOT(quit()));
    nextProcess();
    loop.exec();
}

void Queries::start() //ASync
{
    nextProcess();
}

void Queries::processConnectToHostDisconnected()
{
    //si se desconecto el equipo porque finalizo exitosamente
    if ( opcionActual >= Exit )
        return;

    qDebug() << m_ip  << "Queries::processConnectToHostDisconnected()" << m_ip << m_name;

    if ( m_connected )
    {
        //se logro la conexion pero a media consulta se desconecto
        qDebug() << m_ip  << "**Equipo desconectado a media consulta**" << m_ip << m_name <<
                    "opcionActual" << opcionActual << "intentos" << m_consultaIntentos;

        if ( m_consultaIntentos <= 3 )
        {
            qDebug() << m_ip  << "reconectando y continuando consulta donde se quedo" << m_ip << m_name;
            m_connected=false;
            conectarAequipo(m_ip,m_user,m_pwd,m_platform);
        }
        else
        {
            qDebug() << m_ip  << "3 intentos de consulta, se finaliza" << m_ip << m_name;
            m_error=true;
            emit finished(this);
        }
    }
    else
    {
        if ( m_ipreachable )
        {
            if ( m_consultaIntentos <= 3 )
                conectarAequipo(m_ip,m_user,m_pwd,m_platform);
        }
        else
        {
            //no se logro la conexion
            saveLog( "Equipo desconectado\n" );
            qDebug() << m_ip  << "**NoConexion**" << m_ip;
            m_operativo=false;
            queryTimer->stop();

            //        //si tiene informacion de nombre o plataforma por snmp creamos los queries vacios para que este
            //        //disponible a la hora de graficar
            //        if ( !m_platform.isEmpty() )
            //            createQueries();

            emit finished(this);
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

    qDebug() << m_ip  << "Equipo conectado" << m_ip << m_name;
    saveLog( "Equipo conectado\n" );
    m_connected=true;
    term->disconnectReceiveTextSignalConnections();
    
    m_consultaIntentos++;
    queryTimer->stop();
    
    if ( m_consultaIntentos == 1 )
    {   
        //primer intento
        qDebug() << m_ip  << "Conectado. Primer intento" << m_ip << m_name;
        m_brand = term->brandName();
        
        if ( m_platform.isEmpty() )
        {
            m_platform = term->platform();
            m_equipmenttype=equipmentOSFromPlatform( m_platform );
        }
        
        createQueries();
        
        if ( m_name.isEmpty() )
        {
            m_fullName = term->hostName();
            m_name = simplicateName( term->hostName() );
        }
    }
    else    
    {
        //siguientes intentos    
        qDebug() << m_ip  << "Conectado. Siguiente intento de consulta despues de falla" << m_ip << m_name;
        m_reintentandoConsulta=true;
    }

    nextProcess();
}

void Queries::processPlatform()
{
    queryTimer->stop();
    m_contieneconsultas=true;
    if ( m_platform.isEmpty() )
    {
        m_platform=pi->platformInfo();
        m_xr_location=pi->xr_location();
        m_equipmenttype=equipmentOSFromPlatform( m_platform );
    }
    nextProcess();
}

void Queries::processEquipmentNeighbors()
{
    queryTimer->stop();
    m_contieneconsultas=true;

    if ( !m_lstHostsGeneralAconectar.isEmpty() )
    {
        foreach (SEquipmentNeighborsInfo *vecino, equipmentNeighborsInfo())
        {
            //se verifica por partes para no gastar recursos, si la plataforma y nombre del equipo no son validos
            //no recorremos todo el listado de equipos
            if ( equipmentNeighborsVecinoValidarEquipo( vecino ) )
            {
                QString equipmentNeighborsnombre = simplicateName(vecino->nombreequipo);
                QString equipmentNeighborspais = BDHosts::instance()->hostCountry( equipmentNeighborsnombre );
                if ( equipmentNeighborspais == m_country || equipmentNeighborspais.isEmpty() )
                {
                    //nombre, plataforma y pais son correctos

                    //verificamos si el equipo que tiene la IP ya esta agregado a la lista con otra IP
                    //casos de equipos con varias interfaces L3
                    bool encontrado=false;
                    foreach (Host *host, m_lstHostsGeneralAconectar)
                    {
                        if ( host->nombre == equipmentNeighborsnombre )
                        {
                            encontrado=true;
                            break;
                        }
                    }
                    if ( !encontrado )
                        m_lstEquipmentNeighborsNuevosVecinos.append( vecino );
                }
            }
        }
    }

    nextProcess();
}

void Queries::processConfigFinished()
{
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
    qDebug() << m_ip  << "Queries::processFinished()";
    m_contieneconsultas=true;
    queryTimer->stop();
    nextProcess();
}

void Queries::processKeepWorking()
{
    queryTimer->stop();
    queryTimer->start();
}

void Queries::on_m_keepAliveTimer_timeout()
{
    qDebug() << m_ip  << "Queries::on_m_keepAliveTimer_timeout()";
    term->sendCommand( " " );
}

void Queries::on_query_lastCommand(QString txt)
{
    emit lastCommand(this,txt);
}

void Queries::on_queryTimer_timeout()
{
    qDebug() << m_ip  << "Queries::on_queryTimer_timeout()" << m_ip << m_name;
    processConnectToHostDisconnected();
}

void Queries::setLogPath(QString path)
{
    if ( !path.isEmpty() )
    {
        m_logPath = path+"/"+m_ip+".txt";
        m_logFile.setFileName( m_logPath );
        m_logFile.open(QIODevice::Append | QIODevice::Text);
        m_out.setDevice(&m_logFile);
    }
}

void Queries::on_term_readyRead()
{
    saveLog( term->dataReceived() );
}

void Queries::saveLog(QString txt)
{
    if ( m_logFile.isOpen() )
    {
        m_out << txt;
        m_out.flush();
    }
}

void Queries::updateInfoQueries(QList<Queries*> &lstDest, QList<Queries *> &lstOrigin , QStringList lstIPsConsulta)
{
    //origen = nuevo
    //destino = anterior

    //borramos las consultas veijas y ponemos en false el operativo de todos los equipos que se consultaron, al
    //realizar la fusión si hay una nueva consulta se pondra en true
    for ( int c=0; c<lstDest.size(); )
    {
        Queries *dest = lstDest.at(c);
        if ( dest->m_datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest->m_operativo)
             lstDest.removeAt( c );
        else
        {
            if ( lstIPsConsulta.contains( dest->m_ip ) )
               dest->m_operativo=false;
            c++;
        }
    }

    foreach (Queries *origin, lstOrigin)
    {
        bool encontrado=false;
        foreach (Queries* dest, lstDest)
        {
            //Se busca coincidencia de IP o nombre, porque puede que uno haya cambiado
            //Esto para que no se pierda el equipo y se agregue uno nuevo duplicando asi los equipos.
            //Esto es pensando que nunca va a cambiar la IP y el nombre de equipo al mismo tiempo.
            if ( (dest->ip() == origin->ip() ||
                 dest->hostName() == origin->hostName()) && origin->operativo() )
            {
                dest->m_name = origin->m_name;
                dest->m_fullName = origin->m_fullName;
                dest->m_country = origin->m_country;
                dest->m_platform = origin->m_platform;
                dest->m_brand = origin->m_brand;
                dest->m_datetime = origin->m_datetime;
                dest->m_operativo = origin->m_operativo;
                dest->m_ip = origin->m_ip;

                if ( origin->equipmentNeighborsInfoQuery )
                {
                    if ( dest->equipmentNeighborsInfoQuery )
                        updateInfoList( dest->equipmentNeighborsInfo(), origin->equipmentNeighborsInfo() );
                    else
                        dest->equipmentNeighborsInfoQuery = origin->equipmentNeighborsInfoQuery;
                }
                if ( origin->interfacesInfoQuery )
                {
                    if ( dest->interfacesInfoQuery )
                        updateInfoList( dest->interfacesInfo(), origin->interfacesInfo() );
                    else
                        dest->interfacesInfoQuery = origin->interfacesInfoQuery;
                }
                if ( origin->interfacesPermitedVlansQuery )
                {
                    if ( dest->interfacesPermitedVlansQuery )
                        updateInfoList( dest->interfacesPermitedVlansInfo(), origin->interfacesPermitedVlansInfo() );
                    else
                        dest->interfacesPermitedVlansQuery = origin->interfacesPermitedVlansQuery;
                }
                if ( origin->interfacesDescriptionsQuery )
                {
                    if ( dest->interfacesDescriptionsQuery )
                        updateInfoList( dest->interfacesDescriptionsInfo(), origin->interfacesDescriptionsInfo() );
                    else
                        dest->interfacesDescriptionsQuery = origin->interfacesDescriptionsQuery;
                }
                if ( origin->interfacesIpAddressesQuery )
                {
                    if ( dest->interfacesIpAddressesQuery )
                        updateInfoList( dest->interfacesIpAddressesInfo(), origin->interfacesIpAddressesInfo() );
                    else
                        dest->interfacesIpAddressesQuery = origin->interfacesIpAddressesQuery;
                }
                if ( origin->ospfQuery )
                {
                    if ( dest->ospfQuery )
                        updateInfoList( dest->ospfInfo(), origin->ospfInfo() );
                    else
                        dest->ospfQuery = origin->ospfQuery ;
                }
                if ( origin->mplsLdpDiscoveryQuery )
                {
                    if ( dest->mplsLdpDiscoveryQuery )
                    {
                        dest->mplsLdpLocalIDInfo() = origin->mplsLdpLocalIDInfo();
                        updateInfoList( dest->mplsLdpDiscoveryInfo(), origin->mplsLdpDiscoveryInfo() );
                    }
                    else
                        dest->mplsLdpDiscoveryQuery = origin->mplsLdpDiscoveryQuery;
                }
                if ( origin->mplsLdpNeighborsQuery )
                {
                    if ( dest->mplsLdpNeighborsQuery )
                        updateInfoList( dest->mplsLdpNeighborsInfo(), origin->mplsLdpNeighborsInfo() );
                    else
                        dest->mplsLdpNeighborsQuery = origin->mplsLdpNeighborsQuery;
                }
                if ( origin->mplsLdpInterfacesQuery )
                {
                    if ( dest->mplsLdpInterfacesQuery )
                        updateInfoList( dest->mplsLdpInterfacesInfo(), origin->mplsLdpInterfacesInfo() );
                    else
                        dest->mplsLdpInterfacesQuery = origin->mplsLdpInterfacesQuery;
                }
                if ( origin->pimInteracesQuery )
                {
                    if ( dest->pimInteracesQuery )
                        updateInfoList( dest->pimInterfacesInfo(), origin->pimInterfacesInfo() );
                    else
                        dest->pimInteracesQuery = origin->pimInteracesQuery;
                }
                if ( origin->macsQuery )
                {
                    if ( dest->macsQuery )
                        updateInfoList( dest->macInfo(), origin->macInfo() );
                    else
                        dest->macsQuery = origin->macsQuery;
                }
                if ( origin->portChannelInfoQuery )
                {
                    if ( dest->portChannelInfoQuery )
                        updateInfoList( dest->portChannelInfo(), origin->portChannelInfo() );
                    else
                        dest->portChannelInfoQuery = origin->portChannelInfoQuery;
                }
                if ( origin->vrfsFromVlansQuery )
                {
                    if ( dest->vrfsFromVlansQuery )
                        dest->vrfsFromVlansInfo() = origin->vrfsFromVlansInfo();
                    else
                        dest->vrfsFromVlansQuery = origin->vrfsFromVlansQuery;
                }
                if ( origin->vrfFromRTQuery )
                {
                    if ( dest->vrfFromRTQuery )
                        dest->vrfFromRTInfo() = origin->vrfFromRTInfo();
                    else
                        dest->vrfFromRTQuery = origin->vrfFromRTQuery;
                }
                if ( origin->vrfsQuery )
                {
                    if ( dest->vrfsQuery )
                        updateInfoList( dest->vrfsInfo(), origin->vrfsInfo() );
                    else
                        dest->vrfsQuery = origin->vrfsQuery;
                }
                if ( origin->arpsQuery )
                {
                    if ( dest->arpsQuery )
                        updateInfoList( dest->arpsInfo(), origin->arpsInfo() );
                    else
                        dest->arpsQuery = origin->arpsQuery;
                }
                if ( origin->bgpNeighborsQuery )
                {
                    if ( dest->bgpNeighborsQuery )
                        dest->bgpNeighborsInfo() = origin->bgpNeighborsInfo();
                    else
                        dest->bgpNeighborsQuery = origin->bgpNeighborsQuery;
                }
                if ( origin->ipRoutesQuery )
                {
                    if ( dest->ipRoutesQuery )                        
                        dest->ipRoutesInfo() = origin->ipRoutesInfo();
                    else
                        dest->ipRoutesQuery = origin->ipRoutesQuery;
                }

                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            lstDest.append( origin );
    }
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const Queries* query)
{
    //guardar

    out << query->m_name;
    out << query->m_fullName;
    out << query->m_country;
    out << query->m_platform;
    out << query->m_xr_location;
    out << query->m_brand;
    out << query->m_datetime;
    out << query->m_operativo;
    out << query->m_ip;

    if ( query->equipmentNeighborsInfoQuery )
    {
        out << true;
        out << query->equipmentNeighborsInfoQuery;
    }
    else
        out << false;
    if ( query->interfacesInfoQuery )
    {
        out << true;
        out << query->interfacesInfoQuery;
    }
    else
        out << false;
    if ( query->interfacesDescriptionsQuery )
    {
        out << true;
        out << query->interfacesDescriptionsQuery;
    }
    else
        out << false;
    if ( query->interfacesIpAddressesQuery )
    {
        out << true;
        out << query->interfacesIpAddressesQuery;
    }
    else
        out << false;
    if ( query->interfacesPermitedVlansQuery )
    {
        out << true;
        out << query->interfacesPermitedVlansQuery;
    }
    else
        out << false;
    if ( query->ospfQuery )
    {
        out << true;
        out << query->ospfQuery;
    }
    else
        out << false;    
    if ( query->mplsLdpDiscoveryQuery )
    {
        out << true;
        out << query->mplsLdpDiscoveryQuery;
    }
    else
        out << false;
    if ( query->mplsLdpNeighborsQuery )
    {
        out << true;
        out << query->mplsLdpNeighborsQuery;
    }
    else
        out << false;
    if ( query->mplsLdpInterfacesQuery )
    {
        out << true;
        out << query->mplsLdpInterfacesQuery;
    }
    else
        out << false;
//    if ( query->pimNeighborsQuery )
//    {
//        out << true;
//        out << query->pimNeighborsQuery;
//    }
//    else
//        out << false;
    if ( query->pimInteracesQuery )
    {
        out << true;
        out << query->pimInteracesQuery;
    }
    else
        out << false;
    if ( query->macsQuery )
    {
        out << true;
        out << query->macsQuery;
    }
    else
        out << false;
    if ( query->portChannelInfoQuery )
    {
        out << true;
        out << query->portChannelInfoQuery;
    }
    else
        out << false;
    if ( query->vrfsFromVlansQuery )
    {
        out << true;
        out << query->vrfsFromVlansQuery;
    }
    else
        out << false;
    if ( query->vrfFromRTQuery )
    {
        out << true;
        out << query->vrfFromRTQuery;
    }
    else
        out << false;
    if ( query->vrfsQuery )
    {
        out << true;
        out << query->vrfsQuery;
    }
    else
        out << false;
    if ( query->arpsQuery )
    {
        out << true;
        out << query->arpsQuery;
    }
    else
        out << false;
    if ( query->bgpNeighborsQuery )
    {
        out << true;
        out << query->bgpNeighborsQuery;
    }
    else
        out << false;
    if ( query->ipRoutesQuery )
    {
        out << true;
        out << query->ipRoutesQuery;
    }
    else
        out << false;

    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, Queries*& query)
{
    //abrir

    query = new Queries;
    bool a;

    in >> query->m_name;
    in >> query->m_fullName;
    in >> query->m_country;
    in >> query->m_platform;
    in >> query->m_xr_location;
    in >> query->m_brand;
    in >> query->m_datetime;
    in >> query->m_operativo;
    in >> query->m_ip;

    in >> a;
    if ( a )
    {
        in >> query->equipmentNeighborsInfoQuery;
        foreach (SEquipmentNeighborsInfo *i, query->equipmentNeighborsInfo())
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->interfacesInfoQuery;
        foreach (SInterfaceInfo *i, query->interfacesInfo())
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->interfacesDescriptionsQuery;
        foreach (SInterfaceInfo *i, query->interfacesDescriptionsInfo())
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->interfacesIpAddressesQuery;
        foreach (SIpInfo *i, query->interfacesIpAddressesInfo() )
            i->queryParent = query;
    }
    in >> a;    
    if ( a )
    {
        in >> query->interfacesPermitedVlansQuery;
        foreach (SInterfaceVlans *i, query->interfacesPermitedVlansInfo() )
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->ospfQuery;
        foreach (SOSPFInfo *i, query->ospfInfo() )
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->mplsLdpDiscoveryQuery;
        foreach (SMplsLdpInfo *i, query->mplsLdpDiscoveryInfo() )
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->mplsLdpNeighborsQuery;
        foreach (SMplsLdpInfo *i, query->mplsLdpNeighborsInfo() )
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->mplsLdpInterfacesQuery;
        foreach (SMplsLdpInfo *i, query->mplsLdpInterfacesInfo() )
            i->queryParent = query;
    }
//    in >> a;
//    if ( a )
//    {
//        in >> query->pimNeighborsQuery;
//    }
    in >> a;
    if ( a )
    {
        in >> query->pimInteracesQuery;
        foreach (SPIMInfo *i, query->pimInterfacesInfo() )
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->macsQuery;
        foreach (SMacInfo *i, query->macInfo() )
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->portChannelInfoQuery;
        foreach (SPortChannel *i, query->portChannelInfo() )
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->vrfsFromVlansQuery;
        //no se puede establecer el queryParent, no es una estructura
    }
    in >> a;
    if ( a )
    {
        in >> query->vrfFromRTQuery;
        //no se puede establecer el queryParent, no es una estructura
    }
    in >> a;
    if ( a )
    {
        in >> query->vrfsQuery;
        foreach (SVrfInfo *i, query->vrfsInfo() )
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->arpsQuery;
        foreach (SIpInfo *i, query->arpsInfo() )
            i->queryParent = query;
    }
    in >> a;
    if ( a )
    {
        in >> query->bgpNeighborsQuery;
        //no se puede establecer el queryParent, no es una estructura
    }
    in >> a;
    if ( a )
    {
        in >> query->ipRoutesQuery;
        foreach (SIpRouteInfo *i, query->ipRoutesInfo() )
            i->queryParent = query;
    }

    return in;
}

QNETWORKCLIQUERIES_EXPORT QDebug operator<<(QDebug dbg, const Queries &info)
{
    dbg.space() << "QueryInfo Thread: " << QThread::currentThread() << "\n";
    dbg.space() << "QueryInfo: " << info.m_name << info.m_ip << info.m_brand << info.m_platform;
    dbg.space() << "XR location: " << info.m_xr_location;
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

    if ( info.ipRoutesQuery )
        dbg.space() << "ipRoutesQuery" << *info.ipRoutesQuery;

    if ( info.configQuery )
        dbg.space() << "Configuration" << *info.configQuery;

    dbg.nospace() << "--\n\n";

    return dbg.maybeSpace();
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator<<(QDataStream& out, const LstQueries* lstquery)
{
    //guardar
    out << lstquery->pais;
    out << lstquery->opcionesConsulta;
    out << lstquery->lstQueries;
    out << lstquery->errorMap;
    out << lstquery->dateTime;
    out << lstquery->label;
    out << lstquery->grupoconsulta;
    out << lstquery->queriesParametrosMap;
    out << lstquery->lstIPsAconsultadas;
    return out;
}

QNETWORKCLIQUERIES_EXPORT QDataStream& operator>>(QDataStream& in, LstQueries*& lstquery)
{
    //abrir
    lstquery = new LstQueries;
    in >> lstquery->pais;
    in >> lstquery->opcionesConsulta;
    in >> lstquery->lstQueries;
    in >> lstquery->errorMap;
    in >> lstquery->dateTime;
    in >> lstquery->label;
    in >> lstquery->grupoconsulta;
    in >> lstquery->queriesParametrosMap;
    in >> lstquery->lstIPsAconsultadas;
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
    dbg.space() << "Pais" << info.pais << "\n";
    dbg.space() << "Fecha" << info.dateTime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";
    dbg.space() << "Label" << info.label << "\n";
    dbg.space() << "Grupo" << info.grupoconsulta << "\n";
    dbg.space() << "OpcionesConsulta" << info.opcionesConsulta << "\n\n";
    dbg.space() << "ParametrosMap:\n";
    QMapIterator<QString, QString> ipar(info.queriesParametrosMap);
    while (ipar.hasNext())
    {
        ipar.next();
        dbg.space() << ipar.key() << ipar.value() << "\n";
    }
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
    dbg.space() << "\nIPs consultadas:\n";
    dbg.space() << info.lstIPsAconsultadas;
    return dbg.maybeSpace();
}


