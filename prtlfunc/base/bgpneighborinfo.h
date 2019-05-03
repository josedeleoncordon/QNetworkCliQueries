#ifndef BGPNEIGHBORINFO_H
#define BGPNEIGHBORINFO_H

#include "funcionbase.h"
#include "macinfo.h"

class QPRTLFUNCSHARED_EXPORT BGPNeighborInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QStringList m_lstIPs;
    QString m_type;

public:
    BGPNeighborInfo(QRemoteShell *terminal, QObject *parent=0);
    BGPNeighborInfo(const BGPNeighborInfo &other);

    enum Type
    {
        IPV4,
        VPNV4
    };    
    virtual void getBGPNeighbors();

    //
    QStringList& bgpNeighborInfo() { return m_lstIPs; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const BGPNeighborInfo* info);
    friend QDataStream& operator>>(QDataStream& in, BGPNeighborInfo*& info);
    friend QDebug operator<<(QDebug dbg, const BGPNeighborInfo &info);

private slots:
    void on_term_receiveText();
};

#endif
