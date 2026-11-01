#ifndef OSPFINFONOKIA_H
#define OSPFINFONOKIA_H

#include "ospfinfo.h"

class QNETWORKCLIQUERIES_EXPORT OSPFInfoNokia : public OSPFInfo
{
    Q_OBJECT
public:
    OSPFInfoNokia(QRemoteShell *terminal, int option=QueryOpcion::Null);
    OSPFInfoNokia(const OSPFInfoNokia &other);
    ~OSPFInfoNokia();
    void getOSPFInfo();

    // Parseo separado de la comunicación con el terminal, para poder
    // probarlo con salida guardada (ver tests/tst_ospfinfonokia).
    // Se deben llamar en este orden: parsearInterfaces() usa el índice
    // de interfaces que arma parsearVecinos().
    void parsearVecinos(const QString &salida);           // show router ospf all neighbor detail
    void parsearInterfaces(const QString &salida);        // show router ospf all interface detail

private slots:
    void on_term_receiveTextNeighbors();
    void on_term_receiveTextNeighborsInterface();

private:
    QHash<QString, int> index;

signals:
};

#endif // OSPFINFONOKIA_H
