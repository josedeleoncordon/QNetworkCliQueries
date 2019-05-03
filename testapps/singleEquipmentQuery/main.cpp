#include <QCoreApplication>
#include "queries.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Queries *qry = new Queries("10.192.9.212","jideleon","JoC!sRael+mar+2019");
    qry->setConnectionProtocol(QRemoteShell::SSHTelnet); //SSh first if it fails then telnet
    qry->setOptions( Queries::Connect |
                     Queries::Platform |
                     Queries::EquipmentNeighbors | //CDP and LLDP
                     Queries::MacAddress |
                     Queries::Arp |
                     Queries::InterfaceDescription |
                     Queries::PortChannel |
                     Queries::Ospf );

    QueriesConfiguration::instance()->mapConfigurations.insert("Arp_VRFs","CR_ETX");

    qry->startSync();

    qDebug() << qry->platform();

    for ( SEquipmentNeighborsInfo *en : qry->equipmentNeighborsInfo() )
        qDebug() << en->nombreequipo << en->ip << en->interfazestesalida << en->interfazotroentrada << en->plataforma;

    for ( SMacInfo *mac : qry->macInfo() )
        qDebug() << mac->mac << mac->interfaz << mac->vlan;

    for ( SInterfaceInfo *ii : qry->interfacesDescriptionsInfo() )
        qDebug() << ii->interfaz << ii->status << ii->description;

    for ( SPortChannel *pi : qry->portChannelInfo() )
        qDebug() << pi->group << pi->interfaz;

    for ( SOSPFInfo *oi : qry->ospfInfo() )
        qDebug() << oi->interfaz << oi->area << oi->process;       

    qDebug() << *qry;       

    return a.exec();
}
