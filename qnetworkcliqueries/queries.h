#ifndef QUERIES_H
#define QUERIES_H

#include "equipmentneighbors.h"
#include "interfaceinfo.h"
#include "ospfinfo.h"
#include "mplsldpinfo.h"
#include "mplstetunnelsinfo.h"
#include "piminfo.h"
#include "macinfo.h"
#include "platforminfo.h"
#include "portchannelsinfo.h"
#include "arpinfo.h"
#include "vrfinfo.h"
#include "bgpneighborinfo.h"
#include "iprouteinfo.h"
#include "config.h"
#include "exitinfo.h"

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
    explicit Queries(QString IP, QString user, QString pwd, QString linuxprompt, QObject *parent = 0);
    explicit Queries();
    Queries(const Queries &other);
    ~Queries();

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
        Exit = 16777216
    };

    QString mac; //mie para debug de graficar l2 por mac

    PlatformInfo *pi;
    EquipmentNeighborsInfo *equipmentNeighborsInfoQuery;
    InterfaceInfo *interfacesInfoQuery;
    InterfaceInfo *interfacesPermitedVlansQuery;
    InterfaceInfo *interfacesDescriptionsQuery;
    InterfaceInfo *interfacesIpAddressesQuery;
    OSPFInfo* ospfQuery;
    MplsTEtunnelsInfo *mplsTEtunnelsQuery;
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
    BGPNeighborInfo *bgpNeighborsQuery;
    IPRouteInfo *ipRoutesQuery;
    Config *configQuery;
    ExitInfo *exitQuery;

    bool isConnected() { return m_connected; }
    bool successful() { return !m_error; }
    bool isReachable() { return m_ipreachable; }
    QString hostName() { return m_name; }
    QString country() { return m_country; }
    QString platform() { return m_platform; }
    QString location() { return m_location; }
    QString brand() { return m_brand; }
    void setConnectionProtocol(QRemoteShell::ConnectionProtocol cp) { m_connectionprotol = cp; }    
    QDateTime dateTime() { return m_datetime; }
    bool operativo() { return m_operativo; }
    QString ip() { return m_ip; }

    //se debio establecer m_lstIPsGeneralAconectar para poder verificar equipos que no se encuentran en el listado de equipos
    //a conectar
    QList<SEquipmentNeighborsInfo*> equipmentNeighborsNuevosEquipos() { return m_lstEquipmentNeighborsNuevosVecinos; }

    QList<SEquipmentNeighborsInfo*>& equipmentNeighborsInfo() { return equipmentNeighborsInfoQuery->equipmentNeighborsInfo(); }
    QList<SInterfaceInfo*>& interfacesInfo() { return interfacesInfoQuery->interfacesInfo(); }
    QList<SInterfaceVlans*>& interfacesPermitedVlansInfo() { return interfacesPermitedVlansQuery->interfacesPermitedVlansInfo(); }
    QList<SInterfaceInfo*>& interfacesDescriptionsInfo() { return interfacesDescriptionsQuery->interfacesInfo(); }
    QList<SIpInfo*>& interfacesIpAddressesInfo() { return interfacesIpAddressesQuery->interfacesIpAddressesInfo(); }
    QList<SOSPFInfo*>& ospfInfo() { return ospfQuery->ospfInfo(); } //
    QString& mplsLdpLocalIDInfo() { return mplsLdpDiscoveryQuery->ldpLocalIDInfo(); } //
    QList<SMplsTETunnelInfo*>& mplsLdpTunnelsInfo() { return mplsTEtunnelsQuery->mplsTEtunnelsInfo(); } //
    QList<SMplsLdpInfo*>& mplsLdpDiscoveryInfo() { return mplsLdpDiscoveryQuery->mplsLdpDiscoveryInfo(); } //
    QList<SMplsLdpInfo*>& mplsLdpNeighborsInfo() { return mplsLdpNeighborsQuery->mplsLdpNeighborsInfo(); }
    QList<SMplsLdpInfo*>& mplsLdpInterfacesInfo() { return mplsLdpInterfacesQuery->mplsLdpInterfacesInfo(); }
//    QList<SPIMInfo*>& pimNeighbors() { return pimNeighborsQuery->pimNeighbors(); }
    QList<SPIMInfo*>& pimInterfacesInfo() { return pimInteracesQuery->pimIntefacesInfo(); }
    QList<SMacInfo*>& macInfo() { return macsQuery->macInfo(); }
    QList<SPortChannel*>& portChannelInfo() { return portChannelInfoQuery->portChannelInfo(); } //
    QStringList& vrfsFromVlansInfo() { return vrfsFromVlansQuery->vrfsFromVlansInfo(); }
    QString& vrfFromRTInfo() { return vrfFromRTQuery->vrfFromRTInfo(); }
    QList<SVrfInfo*>& vrfsInfo() { return vrfsQuery->vrfsInfo(); } //
    QList<SIpInfo*>& arpsInfo() { return arpsQuery->arpInfo(); } //
    QStringList& bgpNeighborsInfo() { return bgpNeighborsQuery->bgpNeighborInfo(); } //
    QList<SIpRouteInfo*>& ipRoutesInfo() { return ipRoutesQuery->ipRouteInfo(); }

    void setGW(QString GW);
    void updateDate() { m_datetime = QDateTime::currentDateTime(); }
    void setKeepAlive(bool enable);
    void setRemoteShell(QRemoteShell *remoteShell) { term = remoteShell; }
    void setOptions(unsigned int);
//    void addAndCreateOptions(unsigned int);
    void setOperativo(bool OPERATIVO) { m_operativo = OPERATIVO; }
    void setCountry(QString COUNTRY) { m_country = COUNTRY; }
    void setIP(QString IP) { m_ip = IP; }
    void setHostName(QString NAME) { m_name = NAME; }
    void setPlatform(QString PLATFORM) { m_platform=PLATFORM; }    
    void setLstHostsGeneralAconectar( QList<Host*> lst ) { m_lstHostsGeneralAconectar = lst; }
    void createEmptyQueries();
    void clear();

    void startSync();

    static QMap<QString,QString> queriesArgumentosAceptados();
    static void updateInfoQueries( QList<Queries*> &lstDest, QList<Queries*> &lstOrigin, QStringList lstIPsConsulta );

    friend QDataStream& operator<<(QDataStream& out, const Queries* query);
    friend QDataStream& operator>>(QDataStream& in, Queries*& query);
    friend QDebug operator<<(QDebug dbg, const Queries &info);

public slots:
    virtual void start();    

protected slots:
    void processConnectToHostReachable();
    void processConnectToHostConnected();
    void processConnectToHostDisconnected();
    void processPlatform();
    void processEquipmentNeighbors();
    void processConfigFinished();
    void processFinished();
    void processKeepWorking();
    void on_m_keepAliveTimer_timeout();
    void on_query_lastCommand(QString);
    void on_queryTimer_timeout();
    void on_term_readyRead();

signals:
    void lastCommand(Queries*,QString);
    void finished(Queries*);

protected:
    QRemoteShell *term;
    unsigned int flags;    
    unsigned int opcionActual;
    QTimer *queryTimer;
    bool m_connected;
    bool m_error;
    bool m_ipreachable;
    bool m_queriescreated;
    bool m_contieneconsultas;

    short int m_consultaIntentos;
    bool m_reintentandoConsulta;

    QString m_name;
    QString m_fullName;
    QString m_country;
    QString m_platform;
    QString m_equipmenttype;
    QString m_brand;
    QDateTime m_datetime;
    QString m_xr_location;
    QString m_location;
    QRemoteShell::ConnectionProtocol m_connectionprotol;    
    bool m_operativo;
    QString m_ip;

    QTimer *m_keepAliveTimer;
    QString m_user;
    QString m_gw;
    QString m_pwd;       
    QString m_linuxprompt;
    bool m_conectionSecondTriedOtherUserPassword;

    //si se busca informacion de EquipmentNeighbors sirve para saber el listado de equipos al que nos conectaremos y devolver solo las
    //nuevas IPs. Esto para poder descubrir equipos nuevos a los que no nos conectariamos
    QList<Host*> m_lstHostsGeneralAconectar;
    QList<SEquipmentNeighborsInfo*> m_lstEquipmentNeighborsNuevosVecinos;

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
    QString grupo;
    int opcionesConsulta;
    QList<Queries*> lstQueries;
    QStringList lstIPsAconsultadas;
    QMap<QString, QString> errorMap;
    QMap<QString, QString> queriesParametrosMap;
    QDateTime dateTime;
    QString label;
    QString grupoconsulta;

    friend QDataStream& operator<<(QDataStream& out, const LstQueries* lstquery);
    friend QDataStream& operator>>(QDataStream& in, LstQueries*& lstquery);
    friend QDebug operator<<(QDebug dbg, const LstQueries &info);
};

Q_DECLARE_METATYPE(Queries*);

#endif // QUERIES_H
