#ifndef MACINFOFACTORY_H
#define MACINFOFACTORY_H

#include "arpinfo.h"
#include "bgpneighborinfo.h"
#include "equipmentneighbors.h"
#include "interfaceinfo.h"
#include "iprouteinfo.h"
#include "macinfo.h"
#include "mplsldpinfo.h"
#include "ospfinfo.h"
#include "piminfo.h"
#include "portchannelsinfo.h"
#include "vrfinfo.h"
#include "config.h"
#include "exitinfo.h"

ArpInfo *factoryNewArpInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
BGPNeighborInfo *factoryNewBGPNeighborInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
EquipmentNeighborsInfo *factoryNewEquipmentNeighborsInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
InterfaceInfo *factoryNewInterfaceInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
IPRouteInfo *factoryNewIPRouteInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
MacInfo *factoryNewMacInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
MplsLdpInfo *factoryNewMplsLdpInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
OSPFInfo *factoryNewOSPFInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
PIMInfo *factoryNewPIMInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
PortChannelsInfo *factoryNewPortChannelsInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
VrfInfo *factoryNewVrfInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent);
Config *factoryNewConfig(QString brandName, QString type, QRemoteShell *term, QObject *parent);
ExitInfo *factoryNewExit(QString brandName, QString type, QRemoteShell *term, QObject *parent);

#endif // MACINFOFACTORY_H
