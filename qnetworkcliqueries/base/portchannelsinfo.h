#ifndef PORTCHANNELSINFO_H
#define PORTCHANNELSINFO_H

#include "funcionbase.h"

struct SPortChannel : InfoBase
{
    QString interfaz;
    QString group;

    SPortChannel() {}
    SPortChannel(const SPortChannel &other);
};

QDataStream& operator<<(QDataStream& out, const SPortChannel& data);
QDataStream& operator>>(QDataStream& in, SPortChannel& data);

void updateInfoList(QList<SPortChannel> &lstDest, QList<SPortChannel> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT PortChannelsInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SPortChannel> lstInfo;
public:
    PortChannelsInfo() {}
    PortChannelsInfo(QRemoteShell *terminal, QObject *parent=0);
    PortChannelsInfo(const PortChannelsInfo &other);
    ~PortChannelsInfo();
    virtual void getPortChannelsInfo();

    //
    QList<SPortChannel>& portChannelInfo() { return lstInfo; }

    //
    QStringList portChannelToInterfaces(QString);
    QString interfaceToPortChannel(QString);

    //
    friend QDataStream& operator<<(QDataStream& out, const PortChannelsInfo& info);
    friend QDataStream& operator>>(QDataStream& in, PortChannelsInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const PortChannelsInfo* info);
    friend QDataStream& operator>>(QDataStream& in, PortChannelsInfo*& info);
    friend QDebug operator<<(QDebug dbg, const PortChannelsInfo &info);

private slots:
    void on_term_receiveText();
    void on_term_receiveTextHuawei();
};

Q_DECLARE_METATYPE(SPortChannel)

#endif // PORTCHANNELSINFO_H
