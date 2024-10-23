#ifndef MPLSL2TRANSPORT_H
#define MPLSL2TRANSPORT_H

#include "funcionbase.h"
#include <QObject>

struct SMplsL2PWInfo : InfoBase
{
    QString VCID;
    QString destino;
    QString preferredPath;
    QString remoteDescripcion;
    QString estado;
    //los PW no tienen descripcion
};

struct SMplsL2XconnectInfo : SMplsL2PWInfo
{
    QString xc;
    QString descripcion; //descripcion del xconnect
    QString AC;
};

struct SMplsL2VFIInfo : InfoBase
{
    QString vfi;
    QString AC;
    QString descripcion; //descripcion de la vfi
    QString bridge;
    QList<SMplsL2PWInfo> lstPWs;
};

QDataStream& operator<<(QDataStream& out, const SMplsL2PWInfo& data);
QDataStream& operator>>(QDataStream& in, SMplsL2PWInfo& data);

QDebug operator<<(QDebug dbg, const SMplsL2PWInfo &info);

QDataStream& operator<<(QDataStream& out, const SMplsL2XconnectInfo &data);
QDataStream& operator>>(QDataStream& in, SMplsL2XconnectInfo& data);

QDataStream& operator<<(QDataStream& out, const SMplsL2VFIInfo& data);
QDataStream& operator>>(QDataStream& in, SMplsL2VFIInfo*& data);

void updateInfoList(QList<SMplsL2XconnectInfo> &lstDest, QList<SMplsL2XconnectInfo> &lstOrigin );
void updateInfoList(QList<SMplsL2VFIInfo> &lstDest, QList<SMplsL2VFIInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT MplsL2TransportInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SMplsL2XconnectInfo> m_lstMplsL2Xconnects;
    QList<SMplsL2VFIInfo> m_lstMplsL2VFIs;

    SMplsL2PWInfo *buscarVFI_PW(QString vfi, QString destino, QString vcid);

    QStringList lstIosInterfaces;
    void nextIOSrunInterfaces();

public:
    MplsL2TransportInfo() {}
    MplsL2TransportInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    MplsL2TransportInfo(const MplsL2TransportInfo &other);
    ~MplsL2TransportInfo();
    void getMplsL2Transport();

    //
    QList<SMplsL2XconnectInfo>& mplsL2XconnectsInfo() { return m_lstMplsL2Xconnects; }
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
};

Q_DECLARE_METATYPE(SMplsL2PWInfo)
Q_DECLARE_METATYPE(SMplsL2XconnectInfo)
Q_DECLARE_METATYPE(SMplsL2VFIInfo)

#endif // MPLSL2TRANSPORT_H
