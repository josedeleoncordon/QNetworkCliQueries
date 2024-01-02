#ifndef PLATFORMINFO_H
#define PLATFORMINFO_H

#include "funcionbase.h"

class QNETWORKCLIQUERIES_EXPORT PlatformInfo : public FuncionBase
{
    Q_OBJECT
public:
    PlatformInfo() {}
    PlatformInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    PlatformInfo(const PlatformInfo &other);
    void getPlatformInfo();

    //
    QString& platformInfo() { return m_platform; }
    QString& xr_location() { return m_xr_location; }
    QString& location() { return m_location; }
    QString& vendor() { return m_brand; }
    QString& model() { return m_model; }
    bool& xr64() { return m_xr64; }

private slots:
    void on_term_receiveText();
    void on_term_on_term_receiveText_xr_location();
    void on_term_on_term_receiveTextSnmpLocation();
};

#endif // PLATFORMINFO_H
