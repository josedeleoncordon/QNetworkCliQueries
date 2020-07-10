#ifndef EXITINFO_H
#define EXITINFO_H

#include "funcionbase.h"

class QNETWORKCLIQUERIES_EXPORT ExitInfo : public FuncionBase
{
    Q_OBJECT
protected:
    bool m_gw;

public:
    ExitInfo() {}
    ExitInfo(QRemoteShell *terminal, QueryOpcion option=QueryOpcion::Null);
    void setConnectedByGW(bool enable) { m_gw = enable; }
    void exit();

    //

    //
    QString macFromIP(QString ip);

    //
private slots:
    void on_term_receiveText();
    void on_term_disconnected();
};

#endif // EXITINFO_H
