#ifndef RPLINFO_H
#define RPLINFO_H

#include "funcionbase.h"

struct SRplRouteInfo : InfoBase
{
    enum SRplRouteInfoType
    {
        None,
        RouteMap,
        Rpl
    };

    QString nombre;
    QString txt;
    SRplRouteInfoType type;

    SRplRouteInfo() { type=None; }
};
struct SRplPrefixInfo : InfoBase
{
    enum SRplPrefixInfoType
    {
        None,
        PrefixList,
        Rpl
    };

    QString nombre;
    SRplPrefixInfoType type;
    short int ipversion;
    QStringList lstPrefixes;

    SRplPrefixInfo() { type=None; ipversion=0; }
};
struct SRplCommunityInfo : InfoBase
{
    QString nombre;
    QStringList lstComunities;
};
struct SRplASPathInfo : InfoBase
{
    QString nombre;
    QStringList lstASPath;
};

QDataStream& operator<<(QDataStream& out, const SRplRouteInfo &data);
QDataStream& operator>>(QDataStream& in, SRplRouteInfo& data);

QDataStream& operator<<(QDataStream& out, const SRplPrefixInfo &data);
QDataStream& operator>>(QDataStream& in, SRplPrefixInfo& data);

QDataStream& operator<<(QDataStream& out, const SRplCommunityInfo &data);
QDataStream& operator>>(QDataStream& in, SRplCommunityInfo& data);

QDataStream& operator<<(QDataStream& out, const SRplASPathInfo &data);
QDataStream& operator>>(QDataStream& in, SRplASPathInfo& data);

// void updateInfoList(QList<SRplRouteInfo> &lstDest, QList<SRplRouteInfo> &lstOrigin );
// void updateInfoList(QList<SRplPrefixInfo> &lstDest, QList<SRplPrefixInfo> &lstOrigin );
// void updateInfoList(QList<SRplCommunityInfo> &lstDest, QList<SRplCommunityInfo> &lstOrigin );
// void updateInfoList(QList<SRplASPathInfo> &lstDest, QList<SRplASPathInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT RplInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QMap<QString,SRplRouteInfo> m_lstRplRoutes;
    QMap<QString,SRplPrefixInfo> m_lstRplPrefixes;
    QMap<QString,SRplCommunityInfo> m_lstRplCommunities;
    QMap<QString,SRplASPathInfo> m_lstRplASPath;
public:
    RplInfo() {}
    RplInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    RplInfo(const RplInfo &other);
    ~RplInfo();
    virtual void getRplRouteInfo();
    virtual void getRplPrefixInfo();
    virtual void getRplCommunityInfo();
    virtual void getRplASPathInfo();

    //
    QMap<QString,SRplRouteInfo>& rplRouteInfo() { return m_lstRplRoutes; }
    QMap<QString,SRplPrefixInfo>& rplPrefixesInfo() { return m_lstRplPrefixes; }
    QMap<QString,SRplCommunityInfo>& rplCommunitiesInfo() { return m_lstRplCommunities; }
    QMap<QString,SRplASPathInfo>& rplASPathInfo() { return m_lstRplASPath; }
    //

    //
    friend QDataStream& operator<<(QDataStream& out, const RplInfo& info);
    friend QDataStream& operator>>(QDataStream& in, RplInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const RplInfo* info);
    friend QDataStream& operator>>(QDataStream& in, RplInfo*& info);
    friend QDebug operator<<(QDebug dbg, const RplInfo &info);

private slots:
    void on_term_receiveText_rplRoute();
    void on_term_receiveText_rplRouteMap();
    void on_term_receiveText_rplPrefix();
    void on_term_receiveText_prefix_list();
    void on_term_receiveText_rplCommunity();
    void on_term_receiveText_comunity_list();
    void on_term_receiveText_rplASPath();
    void on_term_receiveText_aspath_list();

private:
    bool vrp_xpl_ipv4_trabajado;
    bool _mikrotiksecondquery;
};

Q_DECLARE_METATYPE(RplInfo)

#endif // RPLINFO_H
