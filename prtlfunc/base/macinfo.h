#ifndef MACINFO_H
#define MACINFO_H

#include "funcionbase.h"

struct SMacInfo : InfoBase
{
    QString mac;
    QString vlan;
    QString interfaz;
    QString age;
};

QDataStream& operator<<(QDataStream& out, const SMacInfo* data);
QDataStream& operator>>(QDataStream& in, SMacInfo*& data);

bool existMacAddress(QList<SMacInfo*>,SMacInfo*);

void updateInfoList( QList<SMacInfo*> &lstDest, QList<SMacInfo*> &lstOrigin );

class QPRTLFUNCSHARED_EXPORT MacInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QString m_mac;
    QString m_vlan;
    QList<SMacInfo*> m_lstMacs;
public:
    MacInfo(QRemoteShell *terminal, QObject *parent=0);
    MacInfo(const MacInfo &other);
    ~MacInfo();
    void setMac(QString m) { m_mac = m; }
    void setVlanBD(QString v) { m_vlan = v; }
    virtual void getMacInfo();

    //
    QList<SMacInfo*>& macInfo() { return m_lstMacs; }

    //
    QString macInterfaceLearnedFrom(QString mac);

    //
    friend QDataStream& operator<<(QDataStream& out, const MacInfo* info);
    friend QDataStream& operator>>(QDataStream& in, MacInfo*& info);
    friend QDebug operator<<(QDebug dbg, const MacInfo &info);
};

#endif // MACINFO_H
