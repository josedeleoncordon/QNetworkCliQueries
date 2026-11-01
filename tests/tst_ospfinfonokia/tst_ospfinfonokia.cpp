// Tests del parser de OSPF para Nokia SR OS usando salida CLI guardada
// en tests/fixtures/nokia/. No se conecta a ningún equipo.

#include <QtTest>
#include "Nokia/ospfinfonokia.h"

static QString leerFixture(const QString &ruta)
{
    QFile f(QStringLiteral(FIXTURES_DIR) + "/" + ruta);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "No se pudo abrir el fixture" << f.fileName();
        return {};
    }
    return QString::fromUtf8(f.readAll());
}

class TstOSPFInfoNokia : public QObject
{
    Q_OBJECT

private slots:
    void vecinos();
    void costosYRouterID();
    void abrConUnaSolaArea();
    void salidaVacia();
};

void TstOSPFInfoNokia::vecinos()
{
    OSPFInfoNokia ospf(nullptr);   // sin terminal: solo se usa el parser
    ospf.parsearVecinos(leerFixture("nokia/show_router_ospf_all_neighbor_detail.txt"));

    QList<SOSPFInfo> &lst = ospf.ospfInfo();
    QCOMPARE(lst.size(), 2);

    QCOMPARE(lst[0].id,            QString("10.255.0.2"));
    QCOMPARE(lst[0].interfaz,      QString("to-PE2"));
    QCOMPARE(lst[0].remoteaddress, QString("10.1.12.2"));
    QCOMPARE(lst[0].area,          QString("0"));
    QCOMPARE(lst[0].state,         QString("Full"));
    QCOMPARE(lst[0].process,       QString("0"));
    QVERIFY(lst[0].operativo);

    QCOMPARE(lst[1].id,            QString("10.255.0.3"));
    QCOMPARE(lst[1].interfaz,      QString("to-AGG3"));
    QCOMPARE(lst[1].area,          QString("10"));   // 0.0.0.10 -> "10"

    // Vecinos en áreas 0 y 10 -> es ABR. Una sola instancia -> no es ASBR.
    QVERIFY(ospf.isABR());
    QVERIFY(!ospf.isASBR());
}

void TstOSPFInfoNokia::costosYRouterID()
{
    OSPFInfoNokia ospf(nullptr);
    ospf.parsearVecinos(leerFixture("nokia/show_router_ospf_all_neighbor_detail.txt"));
    ospf.parsearInterfaces(leerFixture("nokia/show_router_ospf_all_interface_detail.txt"));

    QList<SOSPFInfo> &lst = ospf.ospfInfo();
    QCOMPARE(lst.size(), 2);
    QCOMPARE(lst[0].cost, QString("10"));
    QCOMPARE(lst[1].cost, QString("100"));

    // El router ID se toma de la IP de la interfaz "system"
    QCOMPARE(ospf.lstRouterIDs(), QStringList({"10.255.0.1"}));
}

void TstOSPFInfoNokia::abrConUnaSolaArea()
{
    const QString salida =
        "Rtr Base OSPFv2 Instance 0 Neighbors (detail)\n"
        "Neighbor Rtr Id : 10.255.0.2  Interface: to-PE2\n"
        "Neighbor IP Addr  : 10.1.12.2\n"
        "Area Id           : 0.0.0.0\n"
        "Neighbor State    : Full                     Priority          : 1\n"
        "Neighbor Rtr Id : 10.255.0.4  Interface: to-PE4\n"
        "Neighbor IP Addr  : 10.1.14.2\n"
        "Area Id           : 0.0.0.0\n"
        "Neighbor State    : Full                     Priority          : 1\n";

    OSPFInfoNokia ospf(nullptr);
    ospf.parsearVecinos(salida);
    QCOMPARE(ospf.ospfInfo().size(), 2);

    // Todos los vecinos en el área 0 -> no es ABR
    QVERIFY(!ospf.isABR());
}

void TstOSPFInfoNokia::salidaVacia()
{
    OSPFInfoNokia ospf(nullptr);
    ospf.parsearVecinos(QString());
    ospf.parsearInterfaces(QString());
    QVERIFY(ospf.ospfInfo().isEmpty());
    QVERIFY(ospf.lstRouterIDs().isEmpty());
    QVERIFY(!ospf.isABR());
}

QTEST_GUILESS_MAIN(TstOSPFInfoNokia)
#include "tst_ospfinfonokia.moc"
