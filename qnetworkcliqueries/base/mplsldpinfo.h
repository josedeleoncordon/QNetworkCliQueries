#ifndef MPLSLDPINFO_H
#define MPLSLDPINFO_H

#include "funcionbase.h"

struct SMplsLdpInfo : InfoBase
{
    QString neighbor;
    QString interfaz;

    SMplsLdpInfo() {}
    SMplsLdpInfo(const SMplsLdpInfo &other);
};

QDataStream& operator<<(QDataStream& out, const SMplsLdpInfo& data);
QDataStream& operator>>(QDataStream& in, SMplsLdpInfo& data);

void updateInfoList(QList<SMplsLdpInfo> &lstDest, QList<SMplsLdpInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT MplsLdpInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QString m_localID;
    QList<SMplsLdpInfo> m_lstMplsLdpInfo;

public:
    MplsLdpInfo() {}
    MplsLdpInfo(QRemoteShell *terminal, QObject *parent=0);
    MplsLdpInfo(const MplsLdpInfo &other);
    ~MplsLdpInfo();
    virtual void getMplsLdpDiscovery();
    virtual void getMplsLdpNeighbors();
    virtual void getMplsLdpInterfaces();

    //
    QString& ldpLocalIDInfo() { return m_localID; }
    QList<SMplsLdpInfo>& mplsLdpNeighborsInfo() { return m_lstMplsLdpInfo; }
    QList<SMplsLdpInfo>& mplsLdpInterfacesInfo() { return m_lstMplsLdpInfo; }
    QList<SMplsLdpInfo>& mplsLdpDiscoveryInfo() { return m_lstMplsLdpInfo; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const MplsLdpInfo& info);
    friend QDataStream& operator>>(QDataStream& in, MplsLdpInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const MplsLdpInfo* info);
    friend QDataStream& operator>>(QDataStream& in, MplsLdpInfo*& info);
    friend QDebug operator<<(QDebug dbg, const MplsLdpInfo &info);

private slots:

    void on_term_receiveTextDiscovery();
    void on_term_receiveTextNeighbors();
    void on_term_receiveTextInterfaces();
};

Q_DECLARE_METATYPE(SMplsLdpInfo)

#endif // MPLSLDPINFO_H
