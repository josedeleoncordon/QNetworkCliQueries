#ifndef OSPFINFOHUAWEI_H
#define OSPFINFOHUAWEI_H

#include "ospfinfo.h"

class QNETWORKCLIQUERIES_EXPORT OSPFInfoHuawei : public OSPFInfo
{
    Q_OBJECT
public:
    OSPFInfoHuawei(QRemoteShell *terminal, int option=QueryOpcion::Null);
    OSPFInfoHuawei(const OSPFInfoHuawei &other);
    ~OSPFInfoHuawei();
    void getOSPFInfo();

private slots:
    void on_term_receiveTextNeighbors();
    void on_term_receiveTextInterfaces();
};

#endif // OSPFINFOHUAWEI_H
