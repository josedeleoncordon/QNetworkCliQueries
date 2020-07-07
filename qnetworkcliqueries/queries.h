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

    //accesos rapidos a la primera consulta de una funcion, que es la mayoria
    PlatformInfo *pi;
    EquipmentNeighborsInfo *equipmentNeighborsInfoQuery;
    InterfaceInfo *interfacesInfoQuery;
    InterfaceInfo *interfacesPermitedVlansQuery;
    InterfaceInfo *interfacesDescriptionsQuery;
    InterfaceInfo *interfacesIpAddressesQuery;
    OSPFInfo *ospfQuery;
    MplsTEtunnelsInfo *mplsTEtunnelsQuery;
    MplsL2TransportInfo *mplsL2TransportQuery;
    MplsLdpInfo *mplsLdpDiscoveryQuery;
    MplsLdpInfo *mplsLdpNeighborsQuery;
    MplsLdpInfo *mplsLdpInterfacesQuery;
    PIMInfo *pimInteracesQuery;
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
    QString& id() { return m_id; }
    QString& hostName() { return m_name; }
    QString& country() { return m_country; }
    QString& platform() { return m_platform; }
    QString& location() { return m_location; }
    QString& brand() { return m_brand; }
    QString& gw() { return m_gw; }
    void setConnectionProtocol(QRemoteShell::ConnectionProtocol cp) { m_connectionprotol = cp; }
    QDateTime& dateTime() { return m_datetime; }
    bool& operativo() { return m_operativo; }
    QString& ip() { return m_ip; }
    QString& ipOinterfazMismoDominioOSPFDondeSeViene() { return m_ipOinterfazMismoDominioOSPFdondeSeViene; }
    QueriesConfiguration queriesConfiguration() { return m_queriesconfiguration; }

    bool existsQueryInformation(QueryOpcion option,int i=1);
    FuncionBase *getQuery(QueryOpcion option,int i=1);

    QList<SEquipmentNeighborsInfo>& equipmentNeighborsInfo(int i=1);
    QList<SInterfaceInfo>& interfacesInfo(int i=1);
    QList<SInterfaceVlans>& interfacesPermitedVlansInfo(int i=1);
    QList<SInterfaceInfo>& interfacesDescriptionsInfo(int i=1);
    QList<SIpInfo>& interfacesIpAddressesInfo(int i=1);
    QList<SOSPFInfo>& ospfInfo(int i=1);
    QString& mplsLdpLocalIDInfo(int i=1);
    QList<SMplsTETunnelInfo>& mplsTETunnelsInfo(int i=1);
    QList<SMplsL2XconnectInfo>& mplsL2TransportXconnectsInfo(int i=1);
    QList<SMplsL2VFIInfo>& mplsL2TransportVFIsInfo(int i=1);
    QList<SMplsLdpInfo>& mplsLdpDiscoveryInfo(int i=1);
    QList<SMplsLdpInfo>& mplsLdpNeighborsInfo(int i=1);
    QList<SMplsLdpInfo>& mplsLdpInterfacesInfo(int i=1);
    QList<SPIMInfo>& pimInterfacesInfo(int i=1);
    QList<SMacInfo>& macInfo(int i=1);
    QList<SPortChannel>& portChannelInfo(int i=1);
    QStringList& vrfsFromVlansInfo(int i=1);
    QString& vrfFromRTInfo(int i=1);
    QList<SVrfInfo>& vrfsInfo(int i=1);
    QList<SIpInfo>& arpsInfo(int i=1);
    QList<SBGPNeighbor>& bgpNeighborsInfo(int i=1);
    QList<SBGPNetwork>& bgpNetworksInfo(int i=1);
    QList<SIpRouteInfo>& ipRoutesInfo(int i=1);
    QString funcionTxtInfo(int i=1);

    void setId(QString id) { m_id = id; }
    void setGW(QString GW);
    void updateDate() { m_datetime = QDateTime::currentDateTime(); }
    void setKeepAlive(bool enable);
    void setRemoteShell(QRemoteShell *remoteShell) { term = remoteShell; }
    void setOptions( QList<QueryOpcion> lst );
    void setOperativo(bool OPERATIVO) { m_operativo = OPERATIVO; }
    void setCountry(QString COUNTRY) { m_country = COUNTRY; }
    void setIP(QString IP) { m_ip = IP; }
    void setHostName(QString NAME) { m_name = NAME; }
    void setPlatform(QString PLATFORM) { m_platform=PLATFORM; }        
    void setIpOInterfazMismoDominioOSPFDondeSeViene( QString interfaz ) { m_ipOinterfazMismoDominioOSPFdondeSeViene=interfaz; }
    void setQueriesConfiguration(QueriesConfiguration configuration) { m_queriesconfiguration=configuration; }

    void setUser2(QString user) { m_user2=user; }
    void setPassword2(QString pwd) { m_pwd2=pwd; }
    void startSync();

    void createEmptyQueries();

    static QMap<QString,QString> queriesArgumentosAceptados();
    static void updateInfoQueries( QList<Queries> &lstDest, QList<Queries> &lstOrigin, QStringList lstIPsConsulta );

    bool operator==(Queries&);
    Queries& operator=(const Queries&);

    friend QDataStream& operator<<(QDataStream& out, const Queries& query);
    friend QDataStream& operator>>(QDataStream& in, Queries& query);
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
    void processConfigFinished();
    void processFinished();
    void processExit();
    void processKeepWorking();
    void on_query_lastCommand(QString);
    void on_queryTimer_timeout();

signals:
    void lastCommand(Queries*,QString);
    void finished(Queries*);

protected:
    QRemoteShell *term;
    QList<QueryOpcion> m_lstOpciones;
    QueryOpcion m_opcionActual;
    QList<FuncionBase*> m_lstFunciones;
    FuncionBase *m_currentFuncion;

    QTimer *queryTimer;
    bool m_connected;
    bool m_error;
    bool m_ipreachable;
    bool m_contieneconsultas;

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
    QRemoteShell::ConnectionProtocol m_connectionprotol;
    bool m_operativo;
    QString m_ip;

    QString m_gw;
    QString m_user;
    QString m_user2;
    QString m_pwd;
    QString m_pwd2;
    QString m_linuxprompt;
    bool m_conectionSecondTriedOtherUserPassword;
    QueriesConfiguration m_queriesconfiguration;

    QString m_ipOinterfazMismoDominioOSPFdondeSeViene;

    //para regresar si un query no esta disponible
    QList<SEquipmentNeighborsInfo> _lstSEquipmentNeighborsInfo;
    QList<SInterfaceInfo> _lstSInterfaceInfo;
    QList<SInterfaceVlans> _lstSInterfaceVlans;
    QList<SIpInfo> _lstSIpInfo;
    QList<SOSPFInfo> _lstSOSPFInfo;
    QString _qstring;
    QList<SMplsTETunnelInfo> _lstSMplsTETunnelInfo;
    QList<SMplsL2XconnectInfo> _lstSMplsL2XconnectInfo;
    QList<SMplsL2VFIInfo> _lstSMplsL2VFIInfo;
    QList<SMplsLdpInfo> _lstSMplsLdpInfo;
    QList<SPIMInfo> _lstSPIMInfo;
    QList<SMacInfo> _lstSMacInfo;
    QList<SPortChannel> _lstSPortChannel;
    QStringList _lstQStringList;
    QList<SVrfInfo> _lstSVrfInfo;
    QList<SBGPNeighbor> _lstSBGPNeighbor;
    QList<SBGPNetwork> _lstSBGPNetwork;
    QList<SIpRouteInfo> _lstSIpRouteInfo;

    void iniciar();
    FuncionBase* createQuerie(QueryOpcion option);
    void conectarAequipo(QString ip, QString user, QString pwd, QString platform, QString linuxprompt);
    void nextProcess();    
    void borrarTerminal();
    void crearFuncionesFaltantes();

private:
    void clone(const Queries& other);
    void _finalizar();
};

class QNETWORKCLIQUERIES_EXPORT LstQueries
{
public:
    LstQueries() {}
    LstQueries(const LstQueries &lstOrigen);
    ~LstQueries();
    bool isEmpty();

    QString grupo;
    QList<QueryOpcion> opcionesConsulta;
    QString gw;
    QList<Queries> lstQueries;
    QStringList lstIPsAconsultadas;
    QStringList lstIPsConectadasPorGW;
    QMap<QString, QString> errorMap;
    QList<QueriesConfigurationValue> lstQueriesParameters;

    QDateTime dateTime;
    QString label;
    QString tipoconsulta;

    friend QDataStream& operator<<(QDataStream& out, const LstQueries& lstquery);
    friend QDataStream& operator>>(QDataStream& in, LstQueries& lstquery);
    friend QDebug operator<<(QDebug dbg, const LstQueries &info);
};

Q_DECLARE_METATYPE(Queries*);

#endif // QUERIES_H
