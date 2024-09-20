#ifndef RPLINFO_H
#define RPLINFO_H

#include "funcionbase.h"

struct SRplRouteInfo : InfoBase
{
    QString nombre;
    QString txt;

    SRplRouteInfo() {}
    SRplRouteInfo(const SRplRouteInfo &other);
};

QDataStream& operator<<(QDataStream& out, const SRplRouteInfo &data);
QDataStream& operator>>(QDataStream& in, SRplRouteInfo& data);

void updateInfoList(QList<SRplRouteInfo> &lstDest, QList<SRplRouteInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT RplInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SRplRouteInfo> m_lstRplRoutes;
public:
    RplInfo() {}
    RplInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    RplInfo(const RplInfo &other);
    ~RplInfo();
    virtual void getRplRouteInfo();

    //
    QList<SRplRouteInfo>& rplRouteInfo() { return m_lstRplRoutes; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const RplInfo& info);
    friend QDataStream& operator>>(QDataStream& in, RplInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const RplInfo* info);
    friend QDataStream& operator>>(QDataStream& in, RplInfo*& info);
    friend QDebug operator<<(QDebug dbg, const RplInfo &info);

private slots:
    void on_term_receiveText_rplRoute();
};

Q_DECLARE_METATYPE(RplInfo)

#endif // RPLINFO_H
