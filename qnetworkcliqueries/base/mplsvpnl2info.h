#ifndef MPLSVPNL2INFO_H
#define MPLSVPNL2INFO_H

#include "funcionbase.h"

struct SMplsVPNL2Info : public InfoBase
{

};

class QNETWORKCLIQUERIES_EXPORT MplsVPNL2Info : public FuncionBase
{
    Q_OBJECT
public:
    MplsVPNL2Info(QRemoteShell *terminal, QObject *parent);
};

#endif // MPLSVPNL2INFO_H
