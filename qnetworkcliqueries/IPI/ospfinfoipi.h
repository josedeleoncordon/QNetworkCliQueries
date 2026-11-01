#ifndef OSPFINFOIPI_H
#define OSPFINFOIPI_H

#include "ospfinfo.h"

class QNETWORKCLIQUERIES_EXPORT OSPFInfoIPI : public OSPFInfo
{
     Q_OBJECT
public:
    OSPFInfoIPI(QRemoteShell *terminal, int option=QueryOpcion::Null);
    OSPFInfoIPI(const OSPFInfoIPI &other);
    ~OSPFInfoIPI();
    void getOSPFInfo();

private slots:
    void on_term_receiveTextNeighbors();
    void on_term_receiveTextInterfaces();
    void on_term_receiveTextIRun();
};

#endif // OSPFINFOIPI_H
