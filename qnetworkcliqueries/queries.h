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
#include "rplinfo.h"

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
    explicit Queries(QString IP, QString user, QString pwd, QObject *parent = nullptr);
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
    BGPInfo *bgpNetworksBGPAttrQuery;
    IPRouteInfo *ipRoutesQuery;
    Config *configQuery;
    FuncionInfo *funcionQuery;
    RplInfo *rplRoutesQuery;
    RplInfo *rplPrefixesQuery;
    RplInfo *rplCommunitiesQuery;
    ExitInfo *exitQuery;

    bool isConnected() { return m_connected; }
    bool successful() { return !m_error; }
    bool isReachable() { return m_ipreachable; }
    QString errorTxt() { return _errortxt; }
    QString& id() { return m_id; }
    QString& hostName() { return m_name; }
    QString& country() { return m_country; }
    QString& platform() { return m_platform; }
    bool xr64() { return m_xr64; }
    QString& os() { return m_equipmenttype; }    
    QString& location() { return m_location; }
    QString& brand() { return m_brand; }
    QString& model() { return m_model; }
    QString& gw() { return m_gw; }
    void setConnectionProtocol(QRemoteShell::ConnectionProtocol cp) { m_connectionprotol = cp; }
    QDateTime& dateTime() { return m_datetime; }
    bool& operativo() { return m_operativo; }
    QString& ip() { return m_ip; }
    QString& ipOinterfazMismoDominioOSPFDondeSeViene() { return m_ipOinterfazMismoDominioOSPFdondeSeViene; }
    QueriesConfiguration queriesConfiguration() { return m_queriesconfiguration; }
    QString conexionID() { return m_conexionID; }
    FuncionBase* createQuerieManual(int option);
    QString& type() { return m_type; }
    bool recorridoEnArbol() { return _recorridoEnArbol; }

    bool existsQueryInformation(int option,int i=0);
    //busqueda para un nombre de consulta nula e ir insertando nuevas consultas a la general
    //busqueda por valor numerico, el numero de consulta q coincida con el numero
    //muchas funciones por varias consulta de la misma funcion
    //para mantener compatibilidad
    FuncionBase *getQuery(int option,int i=0);
    //busqueda por nombre de consulta. Una funcion por consulta
    FuncionBase *getQuery(int option,QString queryName);

    QList<SEquipmentNeighborsInfo>& equipmentNeighborsInfo(int i=0);
    QList<SInterfaceInfo>& interfacesInfo(int i=0);
    QList<SInterfaceVlans>& interfacesPermitedVlansInfo(int i=0);
    QList<SInterfaceInfo>& interfacesDescriptionsInfo(int i=0);
    QList<SIpInfo>& interfacesIpAddressesInfo(int i=0);
    QList<SOSPFInfo>& ospfInfo(int i=0);
    QString& mplsLdpLocalIDInfo(int i=0);
    QList<SMplsTETunnelInfo>& mplsTETunnelsInfo(int i=0);
    QList<SMplsL2XconnectInfo>& mplsL2TransportXconnectsInfo(int i=0);
    QList<SMplsL2VFIInfo>& mplsL2TransportVFIsInfo(int i=0);
    QList<SMplsLdpInfo>& mplsLdpDiscoveryInfo(int i=0);
    QList<SMplsLdpInfo>& mplsLdpNeighborsInfo(int i=0);
    QList<SMplsLdpInfo>& mplsLdpInterfacesInfo(int i=0);
    QList<SPIMInfo>& pimInterfacesInfo(int i=0);
    QList<SMacInfo>& macInfo(int i=0);
    QList<SPortChannel>& portChannelInfo(int i=0);
    QStringList& vrfsFromVlansInfo(int i=0);
    QString& vrfFromRTInfo(int i=0);
    QList<SVrfInfo>& vrfsInfo(int i=0);
    QList<SIpInfo>& arpsInfo(int i=0);
    QList<SBGPNeighbor>& bgpNeighborsInfo(int i=0);
    QList<SBGPNeighbor>& bgpNeighborsInfo(QString name);
    QList<SBGPNetwork>& bgpNetworksInfo(int i=0);
    QMap<QString,QList<SBGPNetwork>>& bgpMapNetworksInfo(int i=0);
    QList<SBGPNetwork>& bgpNetworksBGPAttrInfo(int i=0);
    QList<SIpRouteInfo>& ipRoutesInfo(int i=0);
    QList<SRplRouteInfo>& rplRoutesInfo(int i=0);
    QList<SRplRouteInfo>& rplRoutesInfo(QString name);
    QList<SRplPrefixInfo>& rplPrefixesInfo(int i=0);
    QList<SRplPrefixInfo>& rplPrefixesInfo(QString name);
    QList<SRplCommunityInfo>& rplCommunitiesInfo(int i=0);
    QList<SRplCommunityInfo>& rplCommunitiesInfo(QString name);
    QString funcionTxtInfo(int i=0);
    QString funcionTxtInfo(QString name);
    QStringList funcionLstTxtInfo(int i=0);
    QStringList funcionLstTxtInfo(QString name);

    void setId(QString id) { m_id = id; }
    void setGW(QString GW);
    void updateDate() { m_datetime = QDateTime::currentDateTime(); }
    void setKeepAlive(bool enable);
    void setRemoteShell(QRemoteShell *remoteShell) { term = remoteShell; }
    void setRemoteShellUsersPasswords(QStringList lst) { lstRemoteShellUsersPasswords=lst; _userPwdusingList=true; }
    void setOptions( QList<int> lst );
    void setOperativo(bool OPERATIVO) { m_operativo = OPERATIVO; }
    void setCountry(QString COUNTRY) { m_country = COUNTRY; }
    void setIP(QString IP) { m_ip = IP; }
    void setHostName(QString NAME) { m_name = NAME; }
    void setPlatform(QString PLATFORM) { m_platform=PLATFORM; }        
    void setIpOInterfazMismoDominioOSPFDondeSeViene( QString interfaz ) { m_ipOinterfazMismoDominioOSPFdondeSeViene=interfaz; }
    void setQueriesConfiguration(QueriesConfiguration configuration) { m_queriesconfiguration=configuration; }
    void setConexionID(QString ID) { m_conexionID=ID; }
    void setQueryName(QString name) { m_queryname=name; }
    void setType(QString type) { m_type= type; }
    void setRecorridoEnArbol(bool recorrido) { _recorridoEnArbol=recorrido; }

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
    friend QDataStream& operator<<(QDataStream& out, const Queries*& query);
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
    bool _sshUseLibSsh;
    QList<int> m_lstOpciones;
    int m_opcionActual;
    QMultiMap<QString,FuncionBase*> m_lstFunciones;
    FuncionBase *m_currentFuncion;

    QTimer *queryTimer;
    bool m_connected;
    bool m_error;
    bool m_ipreachable;
    bool m_contieneconsultas;
    QString _errortxt;

    short int m_consultaIntentos;
    bool m_reintentandoConsulta;

    bool m_disconnectWhenFinished;

    QString m_name;
    QString m_fullName;
    QString m_id;
    QString m_country;
    QString m_platform;
    QString m_equipmenttype;
    QString m_model;
    QString m_brand;
    QDateTime m_datetime;
    QString m_xr_location;
    QString m_location;
    QString m_queryname;
    QString m_type;
    bool m_xr64;
    QRemoteShell::ConnectionProtocol m_connectionprotol;
    QStringList lstRemoteShellUsersPasswords;
    bool _userPwdusingList;
    bool m_operativo;
    QString m_ip;
    QString m_conexionID; //ID de conexion, para que las funciones puedan ubicar los parapemtros en m_queriesConfiguration

    QString m_gw;
    QString m_user;
    QString m_user2;
    QString m_pwd;
    QString m_pwd2;
    QString m_linuxprompt;
    bool m_conectionSecondTriedOtherUserPassword;
    QueriesConfiguration m_queriesconfiguration;

    QString m_ipOinterfazMismoDominioOSPFdondeSeViene;

    //Node helper
    bool _recorridoEnArbol;

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
    QMap<QString,QList<SBGPNetwork>> _mapNetworksInfo;
    QList<SBGPNetwork> _lstSBGPNetworkBGPAttr;
    QList<SIpRouteInfo> _lstSIpRouteInfo;
    QList<SRplRouteInfo> _lstRplRoutesInfo;
    QList<SRplPrefixInfo> _lstRplPrefixesInfo;
    QList<SRplCommunityInfo> _lstRplCommunitiesInfo;

    void iniciar();
    FuncionBase* createQuerie(int option);
    void conectarAequipo(QString ip, QString user, QString pwd, QString platform, QString linuxprompt, bool sshUseLibSsh);
    void nextProcess();    
    void borrarTerminal();
    void crearFuncionesFaltantes();

//    void connectNotify(const QMetaMethod &signal);
//    void disconnectNotify(const QMetaMethod &signal);

private:
    void clone(const Queries& other);
    void _finalizar();

private slots:

    void on_finished(Queries*);
};

class QNETWORKCLIQUERIES_EXPORT LstQueries
{
public:
    LstQueries() {}
    LstQueries(const LstQueries &lstOrigen);
    ~LstQueries();
    LstQueries& operator=(const LstQueries &lstOrigen);
    void clone(const LstQueries &lstOrigen);
    bool isEmpty();

    QString grupo;
    QList<int> opcionesConsulta;
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
