#ifndef PLATFORMINFO_H
#define PLATFORMINFO_H

#include "funcionbase.h"

class QPRTLFUNCSHARED_EXPORT PlatformInfo : public FuncionBase
{
    Q_OBJECT
public:
    PlatformInfo(QRemoteShell *terminal, QObject *parent=0);
    PlatformInfo(const PlatformInfo &other);
    void getPlatformInfo();

    //
    QString& platformInfo() { return m_platform; }
    QString& xr_location() { return m_xr_location; }

private slots:
    void on_term_receiveText();
    void on_term_on_term_receiveText_xr_location();
};

#endif // PLATFORMINFO_H
