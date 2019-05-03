#include "factory.h"

#include "Cisco/macinfocisco.h"
#include "Cisco/ospfinfocisco.h"
#include "Cisco/arpinfocisco.h"
#include "Huawei/macinfohuawei.h"
#include "Huawei/ospfinfohuawei.h"
#include "Huawei/arpinfohuawei.h"
#include "RAD/macinforad.h"
#include "funciones.h"

ArpInfo *factoryNewArpInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    Q_UNUSED( type )

    if ( brandName == "Cisco" )
        return new ArpInfoCisco(term,parent);
    else if ( brandName == "Huawei" )
        return new ArpInfoHuawei(term,parent);
    else
        return new ArpInfo(term,parent);
}

BGPNeighborInfo *factoryNewBGPNeighborInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    return new BGPNeighborInfo(term,parent);
}

EquipmentNeighborsInfo *factoryNewEquipmentNeighborsInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    return new EquipmentNeighborsInfo(term,parent);
}

InterfaceInfo *factoryNewInterfaceInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{  
    return new InterfaceInfo(term,parent);
}

IPRouteInfo *factoryNewIPRouteInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    return new IPRouteInfo(term,parent);
}

MacInfo *factoryNewMacInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    Q_UNUSED( type )

    if ( brandName == "Cisco" )           
        return new MacInfoCisco(term,parent);
    else if ( brandName == "Huawei" )
        return new MacInfoHuawei(term,parent);
    else if ( brandName == "RAD" )
        return new MacInfoRAD(term,parent);
    else
        return new MacInfo(term,parent);
}

MplsLdpInfo *factoryNewMplsLdpInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    return new MplsLdpInfo(term,parent);
}

OSPFInfo *factoryNewOSPFInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    Q_UNUSED( type )

    if ( brandName == "Cisco" )
        return new OSPFInfoCisco(term,parent);
    else if ( brandName == "Huawei" )
        return new OSPFInfoHuawei(term,parent);
    else
        return new OSPFInfo(term,parent);
}

PIMInfo *factoryNewPIMInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    return new PIMInfo(term,parent);
}

PortChannelsInfo *factoryNewPortChannelsInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    return new PortChannelsInfo(term,parent);
}

VrfInfo *factoryNewVrfInfo(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    return new VrfInfo(term,parent);
}

Config *factoryNewConfig(QString brandName, QString type, QRemoteShell *term, QObject *parent)
{
    return new Config(term,parent);
}
