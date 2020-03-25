#ifndef QUERIES_H
#define QUERIES_H

#include "equipmentneighbors.h"
#include "interfaceinfo.h"
#include "ospfinfo.h"
#include "mplsldpinfo.h"
#include "mplstetunnelsinfo.h"
#include "mplsl2transport.h"
#include "piminfo.h"
#include "macinfo.h"
#include "platforminfo.h"
#include "portchannelsinfo.h"
#include "arpinfo.h"
#include "vrfinfo.h"
#include "bgpinfo.h"
#include "iprouteinfo.h"
#include "config.h"
#include "exitinfo.h"
#include "funcioninfo.h"

#include <QFile>

class Host;

class QNETWORKCLIQUERIES_EXPORT Queries : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString hostName READ hostName )
    Q_PROPERTY(QString ip READ ip )
    Q_PROPERTY(QString platform READ platform)
    Q_PROPERTY(QString brand READ brand)

public:
    explicit Queries(QString IP, QObject *parent = nullptr);
    explicit Queries(QString IP, QString user, QString pwd, QString linuxprompt, QObject *parent = nullptr);
    explicit Queries();
    Queries(const Queries &other);
    ~Queries();

    //
    enum Opcion {
        Null = 0,
        Connect = 4,
        Platform = 8,                
        EquipmentNeighbors = 16,
        MacAddress = 32,
        InterfaceInformation = 64,
        InterfacePermitedVlans = 128,
        InterfaceDescription = 256,
        InterfaceIpAddresses = 512,
        Ospf = 1024,
        MplsTEtunnels = 2048,
        MplsLdpDiscovery = 4096,
        MplsLdpNeighbors = 8192,
        MplsLdpInterfaces = 16384,
        PimInterfaces = 32768,
        PortChannel = 65536,
        VRFfVlans = 131072,
        VRFfRT = 262144,
        VRFs = 524288,
        Arp = 1048576,
        BGPNeig = 2097152,
        IpRoutes = 4194304,
        Configuration = 8388608,
        Mplsl2Transport = 16777216,
        Funcion = 33554432,
        BGPNetworks = 67108864,
        Exit = 134217728
    };

    PlatformInfo *pi;
    EquipmentNeighborsInfo *equipmentNeighborsInfoQuery;
    InterfaceInfo *interfacesInfoQuery;
    InterfaceInfo *interfacesPermitedVlansQuery;
    InterfaceInfo *interfacesDescriptionsQuery;
    InterfaceInfo *interfacesIpAddressesQuery;
    OSPFInfo* ospfQuery;
    MplsTEtunnelsInfo *mplsTEtunnelsQuery;
    MplsL2TransportInfo *mplsL2TransportQuery;
    MplsLdpInfo* mplsLdpDiscoveryQuery;
    MplsLdpInfo* mplsLdpNeighborsQuery;
    MplsLdpInfo* mplsLdpInterfacesQuery;
    PIMInfo* pimInteracesQuery;
    MacInfo *macsQuery;
    PortChannelsInfo *portChannelInfoQuery;
    VrfInfo *vrfsFromVlansQuery;
    VrfInfo *vrfFromRTQuery;
    VrfInfo *vrfsQuery;
    ArpInfo *arpsQuery;
    BGPInfo *bgpNeighborsQuery;
    BGPInfo *bgpNetworksQuery;
    IPRouteInfo *ipRoutesQuery;
    Config *configQuery;
    FuncionInfo *funcionQuery;
    ExitInfo *exitQuery;

    bool isConnected() { return m_connected; }
    bool successful() { return !m_error; }
    bool isReachable() { return m_ipreachable; }
    QString id() { return m_id; }
    QString hostName() { return m_name; }
    QString country() { return m_country; }
    QString platform() { return m_platform; }
    QString location() { return m_location; }
    QString brand() { return m_brand; }
    QString gw() { return m_gw; }
    void setConnectionProtocol(QRemoteShell::ConnectionProtocol cp) { m_connectionprotol = cp; }   //
    QDateTime dateTime() { return m_datetime; }
    bool operativo() { return m_operativo; }
    QString ip() { return m_ip; }
    QString ipOinterfazMismoDominioOSPFDondeSeViene() { return m_ipOinterfazMismoDominioOSPFdondeSeViene; } //

    QList<SEquipmentNeighborsInfo*>& equipmentNeighborsInfo() { return equipmentNeighborsInfoQuery->equipmentNeighborsInfo(); }
    QList<SInterfaceInfo*>& interfacesInfo() { return interfacesInfoQuery->interfacesInfo(); }
    QList<SInterfaceVlans*>& interfacesPermitedVlansInfo() { return interfacesPermitedVlansQuery->interfacesPermitedVlansInfo(); }
    QList<SInterfaceInfo*>& interfacesDescriptionsInfo() { return interfacesDescriptionsQuery->interfacesInfo(); }
    QList<SIpInfo*>& interfacesIpAddressesInfo() { return interfacesIpAddressesQuery->interfacesIpAddressesInfo(); }
    QList<SOSPFInfo*>& ospfInfo() { return ospfQuery->ospfInfo(); }
    QString& mplsLdpLocalIDInfo() { return mplsLdpDiscoveryQuery->ldpLocalIDInfo(); }
    QList<SMplsTETunnelInfo*>& mplsTETunnelsInfo() { return mplsTEtunnelsQuery->mplsTEtunnelsInfo(); }
    QList<SMplsL2XconnectInfo*>& mplsL2TransportXconnectsInfo() { return mplsL2TransportQuery->mplsL2XconnectsInfo(); }
    QList<SMplsL2VFIInfo*>& mplsL2TransportVFIsInfo() { return mplsL2TransportQuery->mplsL2VFIsInfo(); }
    QList<SMplsLdpInfo*>& mplsLdpDiscoveryInfo() { return mplsLdpDiscoveryQuery->mplsLdpDiscoveryInfo(); }
    QList<SMplsLdpInfo*>& mplsLdpNeighborsInfo() { return mplsLdpNeighborsQuery->mplsLdpNeighborsInfo(); }
    QList<SMplsLdpInfo*>& mplsLdpInterfacesInfo() { return mplsLdpInterfacesQuery->mplsLdpInterfacesInfo(); }
//    QList<SPIMInfo*>& pimNeighbors() { return pimNeighborsQuery->pimNeighbors(); }
    QList<SPIMInfo*>& pimInterfacesInfo() { return pimInteracesQuery->pimIntefacesInfo(); }
    QList<SMacInfo*>& macInfo() { return macsQuery->macInfo(); }
    QList<SPortChannel*>& portChannelInfo() { return portChannelInfoQuery->portChannelInfo(); }
    QStringList& vrfsFromVlansInfo() { return vrfsFromVlansQuery->vrfsFromVlansInfo(); }
    QString& vrfFromRTInfo() { return vrfFromRTQuery->vrfFromRTInfo(); }
    QList<SVrfInfo*>& vrfsInfo() { return vrfsQuery->vrfsInfo(); } //
    QList<SIpInfo*>& arpsInfo() { return arpsQuery->arpInfo(); } //
    QStringList& bgpNeighborsInfo() { return bgpNeighborsQuery->bgpNeighborInfo(); }
    QList<SBGPNetwork*>& bgpNetworksInfo() { return bgpNetworksQuery->bgpNetworksInfo(); }
    QList<SIpRouteInfo*>& ipRoutesInfo() { return ipRoutesQuery->ipRouteInfo(); }
    QString& funcionTxtInfo() { return funcionQuery->txtReceived(); }

    void setId(QString id) { m_id = id; }
    void setGW(QString GW); //
    void updateDate() { m_datetime = QDateTime::currentDateTime(); }
    void setKeepAlive(bool enable); //
    void setRemoteShell(QRemoteShell *remoteShell) { term = remoteShell; } //
    void setOptions(unsigned int); //
    void setOperativo(bool OPERATIVO) { m_operativo = OPERATIVO; }
    void setCountry(QString COUNTRY) { m_country = COUNTRY; }
    void setIP(QString IP) { m_ip = IP; }
    void setHostName(QString NAME) { m_name = NAME; }
    void setPlatform(QString PLATFORM) { m_platform=PLATFORM; }    
    void setIpOInterfazMismoDominioOSPFDondeSeViene( QString interfaz ) { m_ipOinterfazMismoDominioOSPFdondeSeViene=interfaz; } //
    void createEmptyQueries(); //
    void clear(); //
    void setUser2(QString user) { m_user2=user; } //
    void setPassword2(QString pwd) { m_pwd2=pwd; } //
    void startSync(); //

    static QMap<QString,QString> queriesArgumentosAceptados(); //
    static void updateInfoQueries( QList<Queries*> &lstDest, QList<Queries*> &lstOrigin, QStringList lstIPsConsulta ); //

    friend QDataStream& operator<<(QDataStream& out, const Queries* query);
    friend QDataStream& operator>>(QDataStream& in, Queries*& query);
    friend QDebug operator<<(QDebug dbg, const Queries &info);

public slots:
    virtual void start();    

protected slots:
    void processConnectToHostReachable(); //
    void processConnectToHostConnected(); //
    void processConnectToHostDisconnected(); //
    void processPlatform(); //
    void processConfigFinished(); //
    void processFinished(); //
    void processKeepWorking(); //
    void on_m_keepAliveTimer_timeout(); //
    void on_query_lastCommand(QString); //
    void on_queryTimer_timeout(); //

signals:
    void lastCommand(Queries*,QString); //
    void finished(Queries*); //

protected:
    QRemoteShell *term; //
    unsigned int flags;     //
    unsigned int opcionActual; //
    QTimer *queryTimer; //
    bool m_connected;
    bool m_error;
    bool m_ipreachable;
    bool m_queriescreated; //
    bool m_contieneconsultas; //

    short int m_consultaIntentos;
    bool m_reintentandoConsulta;

    QString m_name;
    QString m_fullName;
    QString m_id;
    QString m_country;
    QString m_platform;
    QString m_equipmenttype;
    QString m_brand;
    QDateTime m_datetime;
    QString m_xr_location;
    QString m_location;
    QRemoteShell::ConnectionProtocol m_connectionprotol;   //
    bool m_operativo;
    QString m_ip;

    QTimer *m_keepAliveTimer; //
    QString m_gw; //
    QString m_user; //
    QString m_user2; //
    QString m_pwd; //
    QString m_pwd2; //
    QString m_linuxprompt; //
    bool m_conectionSecondTriedOtherUserPassword; //

    QString m_ipOinterfazMismoDominioOSPFdondeSeViene; //

    void iniciar();
    void createQueries(Queries::Opcion option = Queries::Null);    
    void conectarAequipo(QString ip, QString user, QString pwd, QString platform, QString linuxprompt);
    void nextProcess();    
    void borrarTerminal();
    void limpiarConsultas();

    //despues de terminar una consulta se crean las funciones que no se configuraron
    //por si el usuario trata a accesder a ellas tenga consultas vacias
    void crearFuncionesFaltantes();
};

class QNETWORKCLIQUERIES_EXPORT LstQueries
{
public:
    LstQueries() {}
    LstQueries(LstQueries* lstOrigen);
    ~LstQueries();

    QString grupo;
    int opcionesConsulta;
    QString gw;
    QList<Queries*> lstQueries;
    QStringList lstIPsAconsultadas;
    QStringList lstIPsConectadasPorGW;
    QMap<QString, QString> errorMap;
    QList<QueriesConfigurationValue> lstQueriesParameters;

    QDateTime dateTime;
    QString label;
    QString tipoconsulta;

    friend QDataStream& operator<<(QDataStream& out, const LstQueries* lstquery);
    friend QDataStream& operator>>(QDataStream& in, LstQueries*& lstquery);
    friend QDebug operator<<(QDebug dbg, const LstQueries &info);
};

Q_DECLARE_METATYPE(Queries*);

#endif // QUERIES_H
