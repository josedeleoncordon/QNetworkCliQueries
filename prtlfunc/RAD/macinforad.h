#ifndef MACINFORAD_H
#define MACINFORAD_H

#include "macinfo.h"

class MacInfoRAD : public MacInfo
{
    Q_OBJECT
private:
    QStringList ext201menus;
    short int pageMacCounter;
    int totalMacs;
    bool etxAgregadaMAC;
    QTimer *etxTimer;    
    short int menuintentos;

public:
    MacInfoRAD(QRemoteShell *terminal, QObject *parent);
    MacInfoRAD(const MacInfoRAD &other);
    ~MacInfoRAD();
    void getMacInfo();

private slots:
    void on_term_receiveTextETX1();

    void on_term_receiveTextETX1002001300();
    void on_ext_timer_timeout();
};

#endif // MACINFORAD_H
