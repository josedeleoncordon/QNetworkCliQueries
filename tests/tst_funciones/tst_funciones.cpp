// Tests unitarios de las utilidades puras de funciones.h
// No necesitan conexión a ningún equipo.

#include <QtTest>
#include "funciones.h"

class TstFunciones : public QObject
{
    Q_OBJECT

private slots:
    // --- estandarizarInterfaz ---
    void estandarizarInterfaz_data();
    void estandarizarInterfaz();

    // --- máscaras ---
    void convertirMascaraOctetosAdos_data();
    void convertirMascaraOctetosAdos();
    void convertirMascaraDosAOctetos();
    void convertirMascaraMayorAmenor();

    // --- IPs ---
    void validarIPperteneceAsegmento_data();
    void validarIPperteneceAsegmento();
    void sonIPsParejaMascara30_31_data();
    void sonIPsParejaMascara30_31();

    // --- parseo de texto ---
    void numberRangeToLST();
    void txt2Columns();
};

// ---------------------------------------------------------------------------

void TstFunciones::estandarizarInterfaz_data()
{
    QTest::addColumn<QString>("entrada");
    QTest::addColumn<QString>("esperado");

    QTest::newRow("Cisco Gi")      << "GigabitEthernet0/0/1"     << "Gi0/0/1";
    QTest::newRow("Cisco Te")      << "TenGigabitEthernet0/1/0"  << "Te0/1/0";
    QTest::newRow("Cisco Hu")      << "HundredGigE0/0/0/1"       << "Hu0/0/0/1";
    QTest::newRow("XR Bundle")     << "Bundle-Ether100"          << "BE100";
    QTest::newRow("Port-channel")  << "Port-channel10"           << "Po10";
    QTest::newRow("Loopback")      << "Loopback0"                << "Lo0";
    QTest::newRow("Huawei GE")     << "GE0/0/1"                  << "Gi0/0/1";
    QTest::newRow("Huawei Vlanif") << "Vlanif100"                << "Vlan100";
    QTest::newRow("sufijo (xx)")   << "Gi0/0/1(10G)"             << "Gi0/0/1";
}

void TstFunciones::estandarizarInterfaz()
{
    QFETCH(QString, entrada);
    QFETCH(QString, esperado);
    QCOMPARE(::estandarizarInterfaz(entrada), esperado);
}

// ---------------------------------------------------------------------------

void TstFunciones::convertirMascaraOctetosAdos_data()
{
    QTest::addColumn<QString>("mascara");
    QTest::addColumn<QString>("esperado");

    QTest::newRow("/24") << "255.255.255.0"   << "24";
    QTest::newRow("/30") << "255.255.255.252" << "30";
    QTest::newRow("/8")  << "255.0.0.0"       << "8";
    QTest::newRow("/32") << "255.255.255.255" << "32";
}

void TstFunciones::convertirMascaraOctetosAdos()
{
    QFETCH(QString, mascara);
    QFETCH(QString, esperado);
    QCOMPARE(::convertirMascaraOctetosAdos(mascara), esperado);
}

void TstFunciones::convertirMascaraDosAOctetos()
{
    QCOMPARE(::convertirMascaraDosAOctetos("24"), QString("255.255.255.0"));
    QCOMPARE(::convertirMascaraDosAOctetos("30"), QString("255.255.255.252"));
    QCOMPARE(::convertirMascaraDosAOctetos("0"),  QString("0.0.0.0"));

    // Si una función tuviera un bug conocido, QEXPECT_FAIL lo documenta sin
    // romper la suite:
    //   QEXPECT_FAIL("", "motivo del bug", Continue);
    //   QCOMPARE(funcionConBug(...), valorCorrecto);
}

void TstFunciones::convertirMascaraMayorAmenor()
{
    QCOMPARE(::convertirMascaraMayorAmenor("10.1.2.3", 24), QString("10.1.2.0/24"));
    QCOMPARE(::convertirMascaraMayorAmenor("10.1.2.3/32", 16), QString("10.1.0.0/16"));
}

// ---------------------------------------------------------------------------

void TstFunciones::validarIPperteneceAsegmento_data()
{
    QTest::addColumn<QString>("ip");
    QTest::addColumn<QString>("segmento");
    QTest::addColumn<bool>("esperado");

    QTest::newRow("dentro /24")      << "192.168.1.10" << "192.168.1.0/24" << true;
    QTest::newRow("fuera /24")       << "192.168.2.10" << "192.168.1.0/24" << false;
    QTest::newRow("ip con mascara")  << "10.0.0.5/30"  << "10.0.0.4/30"    << true;
    QTest::newRow("sin mascara=/32") << "10.0.0.5"     << "10.0.0.5"       << true;
    QTest::newRow("v4 contra v6")    << "10.0.0.5"     << "2001:db8::/32"  << false;
}

void TstFunciones::validarIPperteneceAsegmento()
{
    QFETCH(QString, ip);
    QFETCH(QString, segmento);
    QFETCH(bool, esperado);
    QCOMPARE(::validarIPperteneceAsegmento(ip, segmento), esperado);
}

void TstFunciones::sonIPsParejaMascara30_31_data()
{
    QTest::addColumn<QString>("ip1");
    QTest::addColumn<QString>("ip2");
    QTest::addColumn<bool>("esperado");

    QTest::newRow("/30 pareja")   << "10.0.0.1" << "10.0.0.2" << true;
    QTest::newRow("/31 pareja")   << "10.0.0.4" << "10.0.0.5" << true;
    QTest::newRow("otra /30")     << "10.0.0.1" << "10.0.0.6" << false;
}

void TstFunciones::sonIPsParejaMascara30_31()
{
    QFETCH(QString, ip1);
    QFETCH(QString, ip2);
    QFETCH(bool, esperado);
    QCOMPARE(::sonIPsParejaMascara30_31(ip1, ip2), esperado);
}

// ---------------------------------------------------------------------------

void TstFunciones::numberRangeToLST()
{
    // Formato típico de listas de VLANs en la CLI
    QCOMPARE(::numberRangeToLST("10,20-23,30"),
             QStringList({"10", "20", "21", "22", "23", "30"}));
    QCOMPARE(::numberRangeToLST(""), QStringList());
}

void TstFunciones::txt2Columns()
{
    // Tabla con columnas de ancho fijo, como la salida de "show ip arp"
    const QString encabezado = "Protocol  Address          Age (min)  Hardware Addr   Type   Interface";
    const QString fila       = "Internet  10.1.1.1                 5   0011.2233.4455  ARPA   Vlan10";

    QList<short> pos = ::columnsPosFromTxt(encabezado,
        {"Protocol", "Address", "Age", "Hardware", "Type", "Interface"});

    QCOMPARE(::txt2Columns(fila, pos),
             QStringList({"Internet", "10.1.1.1", "5", "0011.2233.4455", "ARPA", "Vlan10"}));
}

QTEST_GUILESS_MAIN(TstFunciones)
#include "tst_funciones.moc"
