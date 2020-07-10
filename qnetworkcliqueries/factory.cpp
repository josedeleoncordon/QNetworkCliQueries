#include "factory.h"

#include "Cisco/macinfocisco.h"
#include "Cisco/ospfinfocisco.h"
#include "Cisco/arpinfocisco.h"
#include "Huawei/macinfohuawei.h"
#include "Huawei/ospfinfohuawei.h"
#include "Huawei/arpinfohuawei.h"
#include "RAD/macinforad.h"
#include "funciones.h"

ArpInfo *factoryNewArpInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    Q_UNUSED( type )

    if ( brandName == "Cisco" )
        return new ArpInfoCisco(term,option);
    else if ( brandName == "Huawei" )
        return new ArpInfoHuawei(term,option);
    else
        return new ArpInfo(term,option);
}

BGPInfo *factoryNewBGPNeighborInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new BGPInfo(term,option);
}

BGPInfo *factoryNewBGPNetworksInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new BGPInfo(term,option);
}

EquipmentNeighborsInfo *factoryNewEquipmentNeighborsInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new EquipmentNeighborsInfo(term,option);
}

InterfaceInfo *factoryNewInterfaceInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{  
    return new InterfaceInfo(term,option);
}

IPRouteInfo *factoryNewIPRouteInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new IPRouteInfo(term,option);
}

MacInfo *factoryNewMacInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    Q_UNUSED( type )

    if ( brandName == "Cisco" )           
        return new MacInfoCisco(term,option);
    else if ( brandName == "Huawei" )
        return new MacInfoHuawei(term,option);
    else if ( brandName == "RAD" )
        return new MacInfoRAD(term,option);
    else
        return new MacInfo(term,option);
}

MplsTEtunnelsInfo *factoryNewMplsTEtunnelsInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    Q_UNUSED( type )

    return new MplsTEtunnelsInfo(term,option);
}

MplsL2TransportInfo *factoryNewMplsL2TransportInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    Q_UNUSED( type )

    return new MplsL2TransportInfo(term,option);
}

MplsLdpInfo *factoryNewMplsLdpInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new MplsLdpInfo(term,option);
}

OSPFInfo *factoryNewOSPFInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    Q_UNUSED( type )

    if ( brandName == "Cisco" )
        return new OSPFInfoCisco(term,option);
    else if ( brandName == "Huawei" )
        return new OSPFInfoHuawei(term,option);
    else
        return new OSPFInfo(term,option);
}

PIMInfo *factoryNewPIMInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new PIMInfo(term,option);
}

PortChannelsInfo *factoryNewPortChannelsInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new PortChannelsInfo(term,option);
}

VrfInfo *factoryNewVrfInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new VrfInfo(term,option);
}

Config *factoryNewConfig(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new Config(term,option);
}

FuncionInfo *factoryNewFuncionInfo(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new FuncionInfo(term,option);
}

ExitInfo *factoryNewExit(QString brandName, QString type, QRemoteShell *term, QueryOpcion option)
{
    return new ExitInfo(term,option);
}
