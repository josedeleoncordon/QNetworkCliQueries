#ifndef VRFINFO_H
#define VRFINFO_H

#include "funcionbase.h"

struct SVrfInfo : InfoBase
{
    QString interfaz;
    QString vrf;
};

QDataStream& operator<<(QDataStream& out, const SVrfInfo& data);
QDataStream& operator>>(QDataStream& in, SVrfInfo& data);

void updateInfoList(QList<SVrfInfo> &lstDest, QList<SVrfInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT VrfInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QStringList m_vlans;
    QStringList m_lstVrf;
    QList<SVrfInfo> m_lstVrfInfo;

    QString m_vrf;
    QString m_rt;

public:
    VrfInfo() {}
    VrfInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    VrfInfo(const VrfInfo &other);
    ~VrfInfo();
    virtual void getVRFsFromVLans();
    virtual void getVRFfromRT();
    virtual void getVRFs();

    //
    QStringList& vrfsFromVlansInfo() { return m_lstVrf; }
    QString& vrfFromRTInfo() { return m_vrf; }
    QList<SVrfInfo>& vrfsInfo() { return m_lstVrfInfo; }

    //
    QString vrfFromInterface(QString);

    //
    friend QDataStream& operator<<(QDataStream& out, const VrfInfo& info);
    friend QDataStream& operator>>(QDataStream& in, VrfInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const VrfInfo* info);
    friend QDataStream& operator>>(QDataStream& in, VrfInfo*& info);
    friend QDebug operator<<(QDebug dbg, const VrfInfo &info);

private slots:
    void on_term_receiveTextFromVlans();
    void on_term_receiveTextFromRT();
    void on_term_receiveTextVRFs();
};

Q_DECLARE_METATYPE(SVrfInfo)

#endif // VRFINFO_H
