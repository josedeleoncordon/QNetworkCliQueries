#ifndef INTERFACEINFO_H
#define INTERFACEINFO_H

#include "funcionbase.h"
#include "arpinfo.h"

struct SInterfaceInfo : InfoBase
{
    QString interfaz;
    QString status; //shut up down
    QString description;
    QString mac;
    QString ratein;
    QString rateout;
    QString dropsin;
    QString dropsout;
    QString errorsin;
    QString errorsout;
    QString CRC;
    QString overrun;
    QString collisions;
    QString bandwidth;
    QString mtu;
    QString duplex;
    QString speed;
    QString opticalRx;
    QString mediaType;

    SInterfaceInfo() {}
    SInterfaceInfo(const SInterfaceInfo &other);
};

struct SInterfaceVlans : InfoBase
{
    QString interfaz;
    QStringList vlans;

    SInterfaceVlans() {}
    SInterfaceVlans(const SInterfaceVlans &other);
};

struct SInterfaceIOSServiceInstanceInfo : InfoBase
{
    QString interfaz;
    QString serviceinstance;
    QString description;
    QString vlan;
    QString bridgedomain;

    SInterfaceIOSServiceInstanceInfo() {}
    SInterfaceIOSServiceInstanceInfo(const SInterfaceIOSServiceInstanceInfo &other);
};

QDataStream& operator<<(QDataStream& out, const SInterfaceInfo& data);
QDataStream& operator>>(QDataStream& in, SInterfaceInfo& data);

QDataStream& operator<<(QDataStream& out, const SInterfaceVlans& data);
QDataStream& operator>>(QDataStream& in, SInterfaceVlans& data);

QDataStream& operator<<(QDataStream& out, const SInterfaceIOSServiceInstanceInfo& data);
QDataStream& operator>>(QDataStream& in, SInterfaceIOSServiceInstanceInfo& data);

void updateInfoList(QList<SInterfaceInfo> &lstDest, QList<SInterfaceInfo> &lstOrigin );
void updateInfoList(QList<SInterfaceVlans> &lstDest, QList<SInterfaceVlans> &lstOrigin );
void updateInfoList(QList<SInterfaceIOSServiceInstanceInfo> &lstDest, QList<SInterfaceIOSServiceInstanceInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT InterfaceInfo : public FuncionBase
{
    Q_OBJECT
protected:
    bool m_InterfacesInfo_onlyPhysicalInterfaces;
    QList<SIpInfo> m_lstInterfacesIPAddresses;
    QList<SInterfaceInfo> m_lstInterfacesInfo;
    QList<SInterfaceVlans> m_lstInterfacesPermitedVlans;
    QList<SInterfaceIOSServiceInstanceInfo> m_lstInterfaceServiceInstance;
    bool m_continueShowVlan;
    bool m_continueShowVlanBridge;
    bool m_onlyphysicalinterfaces;

    QStringList m_interfaces; //interfaces a consultar, TODO completar el filtro para el resto de consultas
    QString m_interfaceCurrent;
    int m_interfacesPos;

    void _getInterfacesInfoNextInteface();

public:
    InterfaceInfo() {}
    InterfaceInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    InterfaceInfo(const InterfaceInfo &other);
    ~InterfaceInfo();
    void setInterfaces(QStringList interfaces) { m_interfaces = interfaces; }
    virtual void getInterfacesInfo();
    virtual void getInterfacesIpAddresses();
    virtual void getInterfacesPermitedVlans();
    virtual void getInterfacesDescriptions();
    virtual void getInterfacesServiceInstancesInfo();

    //
    QList<SInterfaceInfo>& interfacesInfo() { return m_lstInterfacesInfo; }
    QList<SIpInfo>& interfacesIpAddressesInfo() { return m_lstInterfacesIPAddresses; }
    QList<SInterfaceVlans>& interfacesPermitedVlansInfo() { return m_lstInterfacesPermitedVlans; }

    //
    QStringList interfacePermitedVlans(QString);
    SInterfaceInfo *interfaceInfo(QString interfaz);
    QString ipFromInterfaz(QString interfaz);

    //
    friend QDataStream& operator<<(QDataStream& out, const InterfaceInfo& ii);
    friend QDataStream& operator>>(QDataStream& in, InterfaceInfo& ii);
    friend QDataStream& operator<<(QDataStream& out, const InterfaceInfo* ii);
    friend QDataStream& operator>>(QDataStream& in, InterfaceInfo*& ii);
    friend QDebug operator<<(QDebug dbg, const InterfaceInfo &info);

private slots:
    void on_term_receiveText_Info();
    void on_term_receiveText_InfoTransceiver();
    void on_term_receiveText_IpAddresses();
    void on_term_receiveText_PermitedVlansTrunk();
    void on_term_receiveText_PermitedVlansTrunkHuawei();
    void on_term_receiveText_PermitedVlansBridge();
    void on_term_receiveText_PermitedVlansAccess();
    void on_term_receiveText_Descriptions();
    void on_term_receiveText_ServiceInstances();
};

Q_DECLARE_METATYPE(SInterfaceInfo)
Q_DECLARE_METATYPE(SInterfaceVlans)
Q_DECLARE_METATYPE(SInterfaceIOSServiceInstanceInfo)

#endif // INTERFACEINFO_H
