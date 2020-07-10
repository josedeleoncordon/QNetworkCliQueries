#ifndef ARPINFOHUAWEI_H
#define ARPINFOHUAWEI_H

#include "funcionbase.h"
#include "arpinfo.h"

class QNETWORKCLIQUERIES_EXPORT ArpInfoHuawei : public ArpInfo
{
    Q_OBJECT
protected:
    void m_siguienteVRF();

public:
    ArpInfoHuawei(QRemoteShell *terminal, QueryOpcion option=QueryOpcion::Null);
    ArpInfoHuawei(const ArpInfoHuawei &other);
    ~ArpInfoHuawei();
    virtual void getArpInfo();

private slots:
    void on_term_receiveText();
};

#endif // ARPINFOHUAWEI_H
