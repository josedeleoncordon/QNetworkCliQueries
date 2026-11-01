#ifndef MPLSL2TRANSPORT_H
#define MPLSL2TRANSPORT_H

#include "funcionbase.h"
#include <QObject>

struct SMplsL2PWInfo : InfoBase
{
    QString pwid;
    QString neighbor;
    QString state;
    QString preferredPath;
    QString remoteDescripcion;    
    QString source;
    QString neighborbk;
    QString pwidbk;
    QString statebk;
    QString srvid; //service id nokia
    QString sdpprimary; //nokia
    QString xc;   //xc name   ios XR  p2p QINQ_TGU336_TGU337_817
    QString descripcion; //descripcion del pw
    QString AC;
    QString ACXEInterfaz;
    QString ACXEServiceInstance;
    QString mtu;
    QString pwclass;
    QString pwclassbck;
    QString group;  //ios xr xconnect group
    bool controlword;
    bool controlwordbck;
    bool fat;
    bool fatbck;
    SMplsL2PWInfo() { controlword=false; controlwordbck=false; fat=false; fatbck=false; }
};

struct SMplsL2VFIInfo : InfoBase
{
    QString vfi;
    QString ACvfi;
    QString descripcion; //descripcion de la vfi
    QString bridge;
    QList<SMplsL2PWInfo> lstPWs;
};

QDataStream& operator<<(QDataStream& out, const SMplsL2PWInfo& data);
QDataStream& operator>>(QDataStream& in, SMplsL2PWInfo& data);

QDebug operator<<(QDebug dbg, const SMplsL2PWInfo &info);

QDataStream& operator<<(QDataStream& out, const SMplsL2VFIInfo& data);
QDataStream& operator>>(QDataStream& in, SMplsL2VFIInfo*& data);

void updateInfoList(QList<SMplsL2PWInfo> &lstDest, QList<SMplsL2PWInfo> &lstOrigin );
void updateInfoList(QList<SMplsL2VFIInfo> &lstDest, QList<SMplsL2VFIInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT MplsL2TransportInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SMplsL2PWInfo> m_lstMplsL2PWs;
    QList<SMplsL2VFIInfo> m_lstMplsL2VFIs;

    SMplsL2PWInfo *buscarVFI_PW(QString vfi, QString destino, QString vcid);
    SMplsL2VFIInfo *buscarVFI(QString vfi);
    SMplsL2PWInfo *buscarPW(QString destino, QString vcid);
    SMplsL2PWInfo *buscarPW(QString AC);

    QStringList lstIosInterfaces;
    void nextIOSrunInterfaces();

public:
    MplsL2TransportInfo() {}
    MplsL2TransportInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    MplsL2TransportInfo(const MplsL2TransportInfo &other);
    ~MplsL2TransportInfo();
    void getMplsL2Transport();

    //
    QList<SMplsL2PWInfo>& mplsL2PWsInfo() { return m_lstMplsL2PWs; }
    QList<SMplsL2VFIInfo>& mplsL2VFIsInfo() { return m_lstMplsL2VFIs; }

    //
    SMplsL2PWInfo *mplsL2PWfromPreferredPath(QString pp);

    //
    friend QDataStream& operator<<(QDataStream& out, const MplsL2TransportInfo& info);
    friend QDataStream& operator>>(QDataStream& in, MplsL2TransportInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const MplsL2TransportInfo* info);
    friend QDataStream& operator>>(QDataStream& in, MplsL2TransportInfo*& info);
    friend QDebug operator<<(QDebug dbg, const MplsL2TransportInfo &info);

private slots:

    void on_term_receiveText_IOS_VFI();
    void on_term_receiveText_IOS_L2Transport();
    void on_term_receiveText_IOS_Xconnect();
    void on_term_receiveText_IOS_runInterfaces();

    void on_term_receiveText_XR_BD();
    void on_term_receiveText_XR_Xconnect();

    void on_term_receiveText_VRP_VSI();
    void on_term_receiveText_VRP_VSI_SERVICES_ALL();
    void on_term_receiveText_VRP_L2Transport();
    void on_term_receiveText_VRP_BGP_EVPN_EVPL();

    void on_term_receiveText_SR_OS_SAP_USING();
    void on_term_receiveText_SR_OS_info_service_epipe();
    void on_term_receiveText_SR_OS_SDP_USING();

private:
    QHash<QString, int> index_nokia_services;
};

Q_DECLARE_METATYPE(SMplsL2PWInfo)
Q_DECLARE_METATYPE(SMplsL2VFIInfo)

#endif // MPLSL2TRANSPORT_H
