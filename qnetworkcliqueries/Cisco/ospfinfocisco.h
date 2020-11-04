#ifndef OSPFINFOCISCO_H
#define OSPFINFOCISCO_H

#include "ospfinfo.h"

class QNETWORKCLIQUERIES_EXPORT OSPFInfoCisco : public OSPFInfo
{
    Q_OBJECT
private:
    bool m_XRconsultaVRFs;
public:
    OSPFInfoCisco(QRemoteShell *terminal, int option=QueryOpcion::Null);
    OSPFInfoCisco(const OSPFInfoCisco &other);
    ~OSPFInfoCisco();
    void getOSPFInfo();

private slots:    
    void on_term_receiveTextNeighbors();
    void on_term_receiveTextInterfaces();
};

#endif // OSPFINFOCISCO_H
