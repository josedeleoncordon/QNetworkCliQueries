#ifndef OSPFINFO_H
#define OSPFINFO_H

#include "funcionbase.h"

struct SOSPFInfo : InfoBase
{
   QString interfaz;
   QString cost;
   QString id;
   QString state;
   QString address;
   QString process;
   QString area;

   SOSPFInfo() {}
   SOSPFInfo(const SOSPFInfo &other);
};

QDataStream& operator<<(QDataStream& out, const SOSPFInfo* data);
QDataStream& operator>>(QDataStream& in, SOSPFInfo*& data);

void updateInfoList(QList<SOSPFInfo*> &lstDest, QList<SOSPFInfo*> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT OSPFInfo : public FuncionBase
{
    Q_OBJECT
protected:
    int m_process;
    QList<SOSPFInfo*> m_lstOSPFInfo;

public:
    OSPFInfo(QRemoteShell *terminal, QObject *parent=0);
    OSPFInfo(const OSPFInfo &other);
    ~OSPFInfo();
    void setProcess(int p) { m_process = p; }
    virtual void getOSPFInfo();

    //
    QList<SOSPFInfo*>& ospfInfo() { return m_lstOSPFInfo; }

    //
    bool interfaceHasNeighbor(QString);
    SOSPFInfo* interfaceOspfInfo(QString);

    //
    friend QDataStream& operator<<(QDataStream& out, const OSPFInfo* info);
    friend QDataStream& operator>>(QDataStream& in, OSPFInfo*& info);
    friend QDebug operator<<(QDebug dbg, const OSPFInfo &info);
};

#endif // OSPFINFO_H
