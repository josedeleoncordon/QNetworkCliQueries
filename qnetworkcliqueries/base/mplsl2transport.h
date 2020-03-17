#ifndef MPLSL2TRANSPORT_H
#define MPLSL2TRANSPORT_H

#include "funcionbase.h"
#include <QObject>

struct SMplsL2PWInfo : InfoBase
{
    QString VCID;
    QString AC;
    QString destino;
    QString preferredPath;
    QString remoteDescripcion;
    QString estado;
    //los PW no tienen descripcion

    SMplsL2PWInfo() {}
    SMplsL2PWInfo(const SMplsL2PWInfo &other);
};

struct SMplsL2XconnectInfo : SMplsL2PWInfo
{
    QString xc;
    QString descripcion; //descripcion del xconnect
    QString interfaz;

    SMplsL2XconnectInfo() {}
    SMplsL2XconnectInfo(const SMplsL2XconnectInfo &other);
};

struct SMplsL2VFIInfo : InfoBase
{
    QString vfi;
    QString descripcion; //descripcion de la vfi
    QString bridge;
    QList<SMplsL2PWInfo*> lstPWs;

    SMplsL2VFIInfo() {}
    SMplsL2VFIInfo(const SMplsL2VFIInfo &other);
};

QDataStream& operator<<(QDataStream& out, const SMplsL2PWInfo* data);
QDataStream& operator>>(QDataStream& in, SMplsL2PWInfo*& data);

QDataStream& operator<<(QDataStream& out, const SMplsL2XconnectInfo* data);
QDataStream& operator>>(QDataStream& in, SMplsL2XconnectInfo*& data);

QDataStream& operator<<(QDataStream& out, const SMplsL2VFIInfo* data);
QDataStream& operator>>(QDataStream& in, SMplsL2VFIInfo*& data);

void updateInfoList(QList<SMplsL2XconnectInfo*> &lstDest, QList<SMplsL2XconnectInfo*> &lstOrigin );
void updateInfoList(QList<SMplsL2VFIInfo*> &lstDest, QList<SMplsL2VFIInfo*> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT MplsL2TransportInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SMplsL2XconnectInfo*> m_lstMplsL2Xconnects;
    QList<SMplsL2VFIInfo*> m_lstMplsL2VFIs;

public:
    MplsL2TransportInfo(QRemoteShell *terminal, QObject *parent);
    MplsL2TransportInfo(const MplsL2TransportInfo &other);
    ~MplsL2TransportInfo();
    void getMplsL2Transport();

    //
    QList<SMplsL2XconnectInfo*>& mplsL2XconnectsInfo() { return m_lstMplsL2Xconnects; }
    QList<SMplsL2VFIInfo*>& mplsL2VFIsInfo() { return m_lstMplsL2VFIs; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const MplsL2TransportInfo* info);
    friend QDataStream& operator>>(QDataStream& in, MplsL2TransportInfo*& info);
    friend QDebug operator<<(QDebug dbg, const MplsL2TransportInfo &info);

private slots:
    void on_term_receiveText_MplsL2Transport_IOS();
    void on_term_receiveText_MplsL2Transport_XR();
};

#endif // MPLSL2TRANSPORT_H
