#ifndef ARPINFO_H
#define ARPINFO_H

#include "funcionbase.h"
#include "macinfo.h"

struct SIpInfo : SMacInfo
{
    QString ip;
    QString vrf;
};

QDataStream& operator<<(QDataStream& out, const SIpInfo* data);
QDataStream& operator>>(QDataStream& in, SIpInfo*& data);

void updateInfoList(QList<SIpInfo*> &lstDest, QList<SIpInfo*> &lstOrigin );

class QPRTLFUNCSHARED_EXPORT ArpInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SIpInfo*> m_lstArp;
    QStringList m_vrfs;    
    QString m_vrf;
    int m_vrfsPos;
    QString m_macip; //mac o ip

public:
    ArpInfo(QRemoteShell *terminal, QObject *parent=0);
    ArpInfo(const ArpInfo &other);
    ~ArpInfo();    
    virtual void getArpInfo();

    //
    QList<SIpInfo*>& arpInfo() { return m_lstArp; }

    //
    QString macFromIP(QString ip);

    //
    friend QDataStream& operator<<(QDataStream& out, const ArpInfo* info);
    friend QDataStream& operator>>(QDataStream& in, ArpInfo*& info);
    friend QDebug operator<<(QDebug dbg, const ArpInfo &info);
};

#endif // ARPINFO_H
