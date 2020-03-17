#ifndef MACINFOFACTORY_H
#define MACINFOFACTORY_H

#include "arpinfo.h"
#include "bgpneighborinfo.h"
#include "equipmentneighbors.h"
#include "interfaceinfo.h"
#include "iprouteinfo.h"
#include "macinfo.h"
#include "mplstetunnelsinfo.h"
#include "mplsl2transport.h"
#include "mplsldpinfo.h"
#include "ospfinfo.h"
#include "piminfo.h"
#include "portchannelsinfo.h"
#include "vrfinfo.h"
#include "config.h"
#include "exitinfo.h"

ArpInfo *factoryNewArpInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
BGPNeighborInfo *factoryNewBGPNeighborInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
EquipmentNeighborsInfo *factoryNewEquipmentNeighborsInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
InterfaceInfo *factoryNewInterfaceInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
IPRouteInfo *factoryNewIPRouteInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
MacInfo *factoryNewMacInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
MplsTEtunnelsInfo *factoryNewMplsTEtunnelsInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
MplsL2TransportInfo *factoryNewMplsL2TransportInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
MplsLdpInfo *factoryNewMplsLdpInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
OSPFInfo *factoryNewOSPFInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
PIMInfo *factoryNewPIMInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
PortChannelsInfo *factoryNewPortChannelsInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
VrfInfo *factoryNewVrfInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
Config *factoryNewConfig(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);
ExitInfo *factoryNewExit(QString brandName, QString type, QRemoteShell *term, QObject *parent=nullptr);

#endif // MACINFOFACTORY_H
