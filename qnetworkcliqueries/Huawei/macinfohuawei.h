#ifndef MACINFOHUAWEI_H
#define MACINFOHUAWEI_H

#include "macinfo.h"

class MacInfoHuawei : public MacInfo
{
    Q_OBJECT
public:
    MacInfoHuawei(QRemoteShell *terminal,QueryOpcion option=QueryOpcion::Null);
    MacInfoHuawei(const MacInfoHuawei &other);
    ~MacInfoHuawei();
    void getMacInfo();

private slots:
    void on_term_receiveText();
};

#endif // MACINFOHUAWEI_H
