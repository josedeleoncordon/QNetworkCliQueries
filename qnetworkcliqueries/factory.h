#ifndef MACINFOFACTORY_H
#define MACINFOFACTORY_H

#include "arpinfo.h"
#include "bgpinfo.h"
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
#include "funcioninfo.h"
#include "exitinfo.h"
#include "rplinfo.h"

ArpInfo *factoryNewArpInfo(QString brandName, QString type, QRemoteShell *term, int option);
BGPInfo *factoryNewBGPNeighborInfo(QString brandName, QString type, QRemoteShell *term, int option);
BGPInfo *factoryNewBGPNeighborDetailInfo(QString brandName, QString type, QRemoteShell *term, int option);
BGPInfo *factoryNewBGPNetworksInfo(QString brandName, QString type, QRemoteShell *term, int option);
BGPInfo *factoryNewBGPNetworksBGPAttrInfo(QString brandName, QString type, QRemoteShell *term, int option);
EquipmentNeighborsInfo *factoryNewEquipmentNeighborsInfo(QString brandName, QString type, QRemoteShell *term, int option);
InterfaceInfo *factoryNewInterfaceInfo(QString brandName, QString type, QRemoteShell *term, int option);
IPRouteInfo *factoryNewIPRouteInfo(QString brandName, QString type, QRemoteShell *term, int option);
MacInfo *factoryNewMacInfo(QString brandName, QString type, QRemoteShell *term, int option);
MplsTEtunnelsInfo *factoryNewMplsTEtunnelsInfo(QString brandName, QString type, QRemoteShell *term, int option);
MplsL2TransportInfo *factoryNewMplsL2TransportInfo(QString brandName, QString type, QRemoteShell *term, int option);
MplsLdpInfo *factoryNewMplsLdpInfo(QString brandName, QString type, QRemoteShell *term, int option);
OSPFInfo *factoryNewOSPFInfo(QString brandName, QString type, QRemoteShell *term, int option);
PIMInfo *factoryNewPIMInfo(QString brandName, QString type, QRemoteShell *term, int option);
PortChannelsInfo *factoryNewPortChannelsInfo(QString brandName, QString type, QRemoteShell *term, int option);
VrfInfo *factoryNewVrfInfo(QString brandName, QString type, QRemoteShell *term, int option);
Config *factoryNewConfig(QString brandName, QString type, QRemoteShell *term, int option);
FuncionInfo *factoryNewFuncionInfo(QString brandName, QString type, QRemoteShell *term, int option);
RplInfo *factoryNewRplInfo(QString brandName, QString type, QRemoteShell *term, int option);
ExitInfo *factoryNewExit(QString brandName, QString type, QRemoteShell *term, int option);

#endif // MACINFOFACTORY_H
