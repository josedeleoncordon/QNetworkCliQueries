#ifndef ARPINFOCISCO_H
#define ARPINFOCISCO_H

#include "funcionbase.h"
#include "arpinfo.h"

class QNETWORKCLIQUERIES_EXPORT ArpInfoCisco : public ArpInfo
{
    Q_OBJECT
private:
    void m_siguienteVRF();

public:
    ArpInfoCisco(QRemoteShell *terminal, QueryOpcion option=QueryOpcion::Null);
    ArpInfoCisco(const ArpInfoCisco &other);
    ~ArpInfoCisco();
    virtual void getArpInfo();      

private slots:
    void on_term_receiveText();
};

#endif // ARPINFOCISCO_H
