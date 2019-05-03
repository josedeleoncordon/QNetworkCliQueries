QNetworkCliQueries
==========

QNetworkCliQueries is a Qt5 library to connect to network devices through Telnet and SSH

This project is based on Qt Creator's SSH and Nokia's Telnet libraries.
All the credits to them! 
(https://github.com/qt-creator/qt-creator/tree/master/src/libs/ssh)
(https://github.com/triochi/qttelnet-2.1-opensource)

QNetworkCliQueries aims to query thousands devices using CLI commands. It is useful when some specific information is not available through SNMP, the device does not support Netconf or is not enabled. The library supports:
 
   * Cisco IOS, IOS XE, IOS XR and Huawei VRP
   * multithreading for many simultaneous queries
   * at the moment OSPF, MAC tables, ARP, VRFs, MPLS, CDP/LLDP, Interfaces, VLANs, PIM, Portchannel, BGP and IP routing table information
   * Linux and Windows (Tested on Linux)
   * Synchronous and Asynchronous functions

### Compiling QNetworkCliQueries

Prerequisites:

   * Botan 2.9.0
   * Qt 5

Steps:
```bash
qmake QNetworkCliQueries.pro
make (or mingw32-make or nmake depending on your platform)
```

Libraries and testapps will be in build folder

### Examples

Directory testapps contains some examples on how to use the library

Single device:

```cpp
Queries *qry = new Queries( "IP address", "user", "Password",this );
qry->setConnectionProtocol( QRemoteShell::Telnet );
qry->setOptions( Queries::Connect |
                 Queries::Platform |
                 Queries::EquipmentNeighbors |  //CDP-LLDP
                 Queries::MacAddress |
                 Queries::PortChannel
                 Queries::Ospf );    
qry->startSync(); //Sync

for ( SOSPFInfo *oi : qry->ospfInfo() )
        qDebug() << oi->interfaz << oi->area << oi->process;       
```

Several devices:

Consult MAC table and ARP table from several VRFs

```cpp
QueriesThread queriesThread = new QueriesThread(this);
connect( queriesThread,SIGNAL(finished(bool)),SLOT(on_queriesThread_finished(bool)));

queriesThread->setConnectionProtocol( QRemoteShell::SSHTelnet ); // First SSH if it fails try Telnet
queriesThread->setUser( "user" );
queriesThread->setPassword( "password" );        
queriesThread->setLstIP( QStringList list with all IPs addresses  );

queriesThread->setInterval( 1000 );    //
queriesThread->setSimultaneos( 3 );    //  Every second connect to 3 devices until there are 75 simultaneous connections
queriesThread->setMaxParalelos( 75 );  //
   
queriesThread->setOpciones( Queries::Connect | 
                            Queries::Platform |
                            Queries::Arp |
                            Queries::MacAddress ); 
                            
QueriesConfiguration::instance()->mapConfigurations.insert("Arp_VRFs","VRF1,VRF2,VRF3,VRF4,VRF5,VRF6");
queriesThread->iniciar(); //Async


.....


void class::on_queriesThread_finished(bool ok)
{
    for ( Queries *qry : queriesThread->lstQueries() )
	for ( SIpInfo *i : qry->arpsInfo() )
	    qDebug() << i->ip << i->mac << i->vlan << i->vrf;
}

```

L2PathVerification:

This application connects to a device, it looks for which interface a mac is learned, it identifies the device connected in that interface and repeats the process.

When the last device is reached, it verifies all the interfaces in the path and shows if there are errors, drops, CRCs, etc.

#####Screenshots:
   ![alt tag](https://github.com/josedeleoncordon/QNetworkCliQueries/blob/master/images/L2PathVerification.png)
   ![alt tag](https://github.com/josedeleoncordon/QNetworkCliQueries/blob/master/images/L2PathVerification2.png)


---

