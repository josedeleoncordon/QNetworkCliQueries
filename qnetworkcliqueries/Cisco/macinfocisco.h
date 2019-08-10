#ifndef MACINFOCISCO_H
#define MACINFOCISCO_H

#include "macinfo.h"

class QNETWORKCLIQUERIES_EXPORT MacInfoCisco : public MacInfo
{
    Q_OBJECT
public:
    MacInfoCisco(QRemoteShell *terminal, QObject *parent);
    MacInfoCisco(const MacInfoCisco &other);
    ~MacInfoCisco();
    void getMacInfo();

private:
    bool m_consultarXconnect;    

private slots:
    void on_term_receiveText();
    void on_term_receiveText_iosXconnect();
};

#endif // MACINFOCISCO_H
