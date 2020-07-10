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

ArpInfo *factoryNewArpInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
BGPInfo *factoryNewBGPNeighborInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
BGPInfo *factoryNewBGPNetworksInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
EquipmentNeighborsInfo *factoryNewEquipmentNeighborsInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
InterfaceInfo *factoryNewInterfaceInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
IPRouteInfo *factoryNewIPRouteInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
MacInfo *factoryNewMacInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
MplsTEtunnelsInfo *factoryNewMplsTEtunnelsInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
MplsL2TransportInfo *factoryNewMplsL2TransportInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
MplsLdpInfo *factoryNewMplsLdpInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
OSPFInfo *factoryNewOSPFInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
PIMInfo *factoryNewPIMInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
PortChannelsInfo *factoryNewPortChannelsInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
VrfInfo *factoryNewVrfInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
Config *factoryNewConfig(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
FuncionInfo *factoryNewFuncionInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);
ExitInfo *factoryNewExit(QString brandName, QString type, QRemoteShell *term, QueryOpcion option);

#endif // MACINFOFACTORY_H
