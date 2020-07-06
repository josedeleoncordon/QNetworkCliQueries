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

QDataStream& operator<<(QDataStream& out, const SOSPFInfo& data);
QDataStream& operator>>(QDataStream& in, SOSPFInfo& data);

void updateInfoList(QList<SOSPFInfo> &lstDest, QList<SOSPFInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT OSPFInfo : public FuncionBase
{
    Q_OBJECT

friend class Queries; //TODO cambiar por el tema de merge en queries

protected:
    int m_process;
    bool m_abr;
    bool m_asbr;
    QList<SOSPFInfo> m_lstOSPFInfo;

public:
    OSPFInfo() {}
    OSPFInfo(QRemoteShell *terminal, QueryOpcion option=QueryOpcion::Null);
    OSPFInfo(const OSPFInfo &other);
    ~OSPFInfo();
    void setProcess(int p) { m_process = p; }
    virtual void getOSPFInfo();

    //
    QList<SOSPFInfo>& ospfInfo() { return m_lstOSPFInfo; }

    //
    bool interfaceHasNeighbor(QString);
    SOSPFInfo* interfaceOspfInfo(QString);
    bool isABR() { return m_abr; }
    bool isASBR() { return m_asbr; }

    //
    friend QDataStream& operator<<(QDataStream& out, const OSPFInfo& info);
    friend QDataStream& operator>>(QDataStream& in, OSPFInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const OSPFInfo* info);
    friend QDataStream& operator>>(QDataStream& in, OSPFInfo*& info);
    friend QDebug operator<<(QDebug dbg, const OSPFInfo &info);
};

Q_DECLARE_METATYPE(SOSPFInfo)

#endif // OSPFINFO_H
