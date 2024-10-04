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
    SRplPrefixInfo(const SRplPrefixInfo &other);
};
struct SRplCommunityInfo : InfoBase
{
    QString nombre;
    QStringList lstComunities;

    SRplCommunityInfo() {}
    SRplCommunityInfo(const SRplCommunityInfo &other);
};

QDataStream& operator<<(QDataStream& out, const SRplRouteInfo &data);
QDataStream& operator>>(QDataStream& in, SRplRouteInfo& data);

QDataStream& operator<<(QDataStream& out, const SRplPrefixInfo &data);
QDataStream& operator>>(QDataStream& in, SRplPrefixInfo& data);

QDataStream& operator<<(QDataStream& out, const SRplCommunityInfo &data);
QDataStream& operator>>(QDataStream& in, SRplCommunityInfo& data);

void updateInfoList(QList<SRplRouteInfo> &lstDest, QList<SRplRouteInfo> &lstOrigin );
void updateInfoList(QList<SRplPrefixInfo> &lstDest, QList<SRplPrefixInfo> &lstOrigin );
void updateInfoList(QList<SRplCommunityInfo> &lstDest, QList<SRplCommunityInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT RplInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SRplRouteInfo> m_lstRplRoutes;
    QList<SRplPrefixInfo> m_lstRplPrefixes;
    QList<SRplCommunityInfo> m_lstRplCommunities;
public:
    RplInfo() {}
    RplInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    RplInfo(const RplInfo &other);
    ~RplInfo();
    virtual void getRplRouteInfo();
    virtual void getRplPrefixInfo();
    virtual void getRplCommunityInfo();

    //
    QList<SRplRouteInfo>& rplRouteInfo() { return m_lstRplRoutes; }
    QList<SRplPrefixInfo>& rplPrefixesInfo() { return m_lstRplPrefixes; }
    QList<SRplCommunityInfo>& rplCommunitiesInfo() { return m_lstRplCommunities; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const RplInfo& info);
    friend QDataStream& operator>>(QDataStream& in, RplInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const RplInfo* info);
    friend QDataStream& operator>>(QDataStream& in, RplInfo*& info);
    friend QDebug operator<<(QDebug dbg, const RplInfo &info);

private slots:
    void on_term_receiveText_rplRoute();
    void on_term_receiveText_rplPrefix();
    void on_term_receiveText_prefix_list();
    void on_term_receiveText_rplCommunity();
    void on_term_receiveText_comunity_list();

private:
    bool vrp_xpl_ipv4_trabajado;
};

Q_DECLARE_METATYPE(RplInfo)

#endif // RPLINFO_H
