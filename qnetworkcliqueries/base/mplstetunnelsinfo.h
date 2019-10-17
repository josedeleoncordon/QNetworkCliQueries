#ifndef MPLSTETUNNELSINFO_H
#define MPLSTETUNNELSINFO_H

#include "funcionbase.h"
#include <QObject>

struct SMplsTETunnelInfo : InfoBase
{
    QString name;
    QString origen;
    QString destino;
    QString TuID;
    QString route;
    QString role;
    QString InterfaceIn;
    QString InterfaceOut;
    QString status;

    SMplsTETunnelInfo() {}
    SMplsTETunnelInfo(const SMplsTETunnelInfo &other);
};

QDataStream& operator<<(QDataStream& out, const SMplsTETunnelInfo* data);
QDataStream& operator>>(QDataStream& in, SMplsTETunnelInfo*& data);

void updateInfoList(QList<SMplsTETunnelInfo*> &lstDest, QList<SMplsTETunnelInfo*> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT MplsTEtunnelsInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SMplsTETunnelInfo*> m_lstMplsTEtunnels;

public:
    MplsTEtunnelsInfo(QRemoteShell *terminal, QObject *parent);
    MplsTEtunnelsInfo(const MplsTEtunnelsInfo &other);
    ~MplsTEtunnelsInfo();
    void getMplsTETunnels();

    //
    QList<SMplsTETunnelInfo*>& mplsTEtunnelsInfo() { return m_lstMplsTEtunnels; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const MplsTEtunnelsInfo* info);
    friend QDataStream& operator>>(QDataStream& in, MplsTEtunnelsInfo*& info);
    friend QDebug operator<<(QDebug dbg, const MplsTEtunnelsInfo &info);

private slots:
    void on_term_receiveText_MplsTETunnels();

};

#endif // MPLSTETUNNELSINFO_H