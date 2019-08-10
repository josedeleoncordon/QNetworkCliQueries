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
};

struct SInterfaceVlans : InfoBase
{
    QString interfaz;
    QStringList vlans;
};

QDataStream& operator<<(QDataStream& out, const SInterfaceInfo* data);
QDataStream& operator>>(QDataStream& in, SInterfaceInfo*& data);

QDataStream& operator<<(QDataStream& out, const SInterfaceVlans* data);
QDataStream& operator>>(QDataStream& in, SInterfaceVlans*& data);

void updateInfoList(QList<SInterfaceInfo*> &lstDest, QList<SInterfaceInfo*> &lstOrigin );
void updateInfoList(QList<SInterfaceVlans*> &lstDest, QList<SInterfaceVlans*> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT InterfaceInfo : public FuncionBase
{
    Q_OBJECT
protected:
    bool m_InterfacesInfo_onlyPhysicalInterfaces;
    QList<SIpInfo*> m_lstInterfacesIPAddresses;
    QList<SInterfaceInfo*> m_lstInterfacesInfo;
    QList<SInterfaceVlans*> m_lstInterfacesPermitedVlans;
    bool m_continueShowVlan;
    bool m_continueShowVlanBridge;

    QStringList m_interfaces; //interfaces a consultar, TODO completar el filtro para el resto de consultas
    QString m_interfaceCurrent;
    int m_interfacesPos;

    void getInterfacesInfoNextInteface();

public:
    InterfaceInfo(QRemoteShell *terminal, QObject *parent=0);
    InterfaceInfo(const InterfaceInfo &other);
    ~InterfaceInfo();
    void setInterfaces(QStringList interfaces) { m_interfaces = interfaces; }
    virtual void getInterfacesInfo();
    virtual void getInterfacesIpAddresses();
    virtual void getInterfacesPermitedVlans();
    virtual void getInterfacesDescriptions();

    //
    QList<SInterfaceInfo*>& interfacesInfo() { return m_lstInterfacesInfo; }
    QList<SIpInfo*>& interfacesIpAddressesInfo() { return m_lstInterfacesIPAddresses; }
    QList<SInterfaceVlans*>& interfacesPermitedVlansInfo() { return m_lstInterfacesPermitedVlans; }

    //
    QStringList interfacePermitedVlans(QString);
    SInterfaceInfo *interfaceInfo(QString interfaz);

    //
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
};

#endif // INTERFACEINFO_H
