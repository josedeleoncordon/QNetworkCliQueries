#include "funciones.h"
#include "queries.h"
#include "equipmentneighbors.h"
#include "interfaceinfo.h"
#include "portchannelsinfo.h"
#include "arpinfo.h"
#include "properties.h"

#include <QDir>
#include <QDirIterator>

#include "qnetworkquerieslogging.h"

QStringList lstGrupos()
{
    QStringList lstGrupos;

    QMapIterator<QString, QVariant> i( Properties::Instance()->mapGrupoRaizIP );
    while (i.hasNext())
    {
        i.next();
        lstGrupos.append( i.key() );
    }
    return lstGrupos;
}

QString equipmentOSFromPlatform(QString platform)
{
    QRegExp expNCS("NCS-[5-6]\\d{3,}");
    if ( platform.contains("ASR9K") ||
         platform.contains("CRS") ||
         platform.contains(expNCS) )
        return "IOS XR";
    else if ( platform.contains("mwr") ||
              platform.contains("760") ||
              platform.contains("3400") ||
              platform.contains("3750") ||
              platform.contains("A90") ||
              platform.contains("ASR-") ||
              platform.contains("6524") ||
              platform.contains("4503") ||
              platform.contains("3560") ||
              platform.contains("3550") ||
              platform.contains("360") ||
              platform.contains("380") )
        return "IOS";
    else if ( platform.contains("N5K") ||
              platform.contains("N7K") )
        return "IOS NX";
    else if ( platform.contains("ATN") ||
              platform.contains("NE40") ||
              platform.contains("NE9000") )
        return "VRP";
    else
        return "";
}

QString openFile(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QTextStream in(&file);
    QString txt = in.readAll();
    file.close();
    return txt;
}

QStringList openFile2List( QString path, bool eliminarDuplicados )
{
    QStringList lst;
    QStringList lstmie = openFile( path ).split("\n",QString::SkipEmptyParts);
    if ( eliminarDuplicados )
    {
        for ( QString line : lstmie )
            if ( !lst.contains( line ) )
                lst.append(line);
    }
    else
        lst = lstmie;

    return lst;
}

void saveFile(QString txt, QString path, QIODevice::OpenModeFlag openmode)
{
    QFile file(path);
    if (!file.open( openmode | QIODevice::Text|QIODevice::WriteOnly ))
        return;

    QTextStream out(&file);
    out << txt;
    file.close();
}

LstQueries openLstQueries(QString path)
{
    LstQueries lst;
    QFile file( path );
    file.open(QIODevice::ReadOnly);
    QDataStream storeStream(&file);
    storeStream >> lst;
    file.close();

    return lst;
}

void saveLstQueries(LstQueries &lst, QString path)
{
    QFile qryfile( path );
    if (!qryfile.open(QIODevice::WriteOnly))
        return;

    QDataStream storeStream(&qryfile);
    storeStream << lst;
    qryfile.close();
}

QString simplicateName(QString name)
{
    int pos = name.indexOf(".");

    if ( pos > 0 )
        name = name.left( pos );

    QRegExp exp2("RP.+CPU\\d\\:");
    if ( name.contains(exp2) )
        name.replace(exp2,"");

    return name;
}

QStringList numberRangeToLST(QString str)
{
    QStringList lst = str.split(",",QString::SkipEmptyParts);

    QStringList salida;
    foreach (QString txt, lst)
    {
        if ( ! txt.contains("-") )
        {
            salida.append(txt);
            continue;
        }
        else
        {
            QStringList lstR = txt.split("-");
            int i = lstR.at(0).toInt();
            int s = lstR.at(1).toInt();

            for ( ; i <= s; i++  )
                salida.append( QString::number(i) );
        }
    }
    return salida;
}

Queries *buscarEquipoPorIPnombre(QList<Queries> &lst, QString ip, QString nombre)
{
    bool nom=false;
    for (Queries &qry : lst)
    {
        //si no habia informacion de cdp/lldp y fue agregado por ospf
        if ( nombre == "**porOSPF**" )
        {
            if ( validateIpQuery( qry,ip ) )
                return &qry;
            continue;
        }

        //tiene que coincidir el nombre. La ip la validamos con el nombre con la funcion. Se hace esto
        //ya que puede ser que la IP que reporto EquipmentNeighbors sea diferente a la ip con la que se hace el backup
        //caso como equipos L3

        if ( qry.hostName() == nombre )
        {
            nom=true;

            if ( validateIpQuery( qry,ip ) )
                return &qry;
        }
    }

//    if ( !nom )
//        qDebug() << "buscarEquipos: no se encontro nombre del equipo en lst" << nombre <<
//                    "no se se empezo a validar IP" << ip;

    return nullptr;
}

Queries* buscarEquipoPorIP(QList<Queries> &lst, QString ip)
{
    for (Queries &qry : lst)
    {
        if ( validateIpQuery( qry,ip ) )
            return &qry;
    }
    return nullptr;
}

Queries *buscarEquipoPorNombre(QList<Queries> &lst, QString nombre)
{
    for (Queries &qry : lst)
    {
        if ( qry.hostName() == nombre )
            return &qry;
    }
    return nullptr;
}

bool validateIpQuery(Queries &qry, QString ip)
{
    //validamos primero la ip, si la enviada concide exactamente se devuleve
    if ( qry.ip() == ip )
        return true;

    else if ( qry.interfacesIpAddressesQuery )
    {
        //el query tiene informacion de las ips del equipo. Validamos la IP
        for (SIpInfo &i : qry.interfacesIpAddressesInfo())
        {
            if ( i.ip == ip )
                return true;
        }

        //TODO dependiendo del contexto de los Nexus puede o no salir la IP con show ip int brief
        //pero no quiere decir que no sea esa ip de ese equipo. Hay que trabajar los Nexus de diferente forma.
    }

    return false;
}

bool platformValidate(QString platform)
{
    if ( platform.contains("ASR9") ||
         platform.contains("CRS") ||
         platform.contains("NCS") ||
         platform.contains("mwr") ||
         platform.contains("760") ||
         platform.contains("3400") ||
         platform.contains("3750") ||
         platform.contains("A90") ||
         platform.contains("ASR-") ||
         platform.contains("6524") ||
         platform.contains("4503") ||
         platform.contains("3560") ||
         platform.contains("3550") ||
         platform.contains("360") ||
         platform.contains("380") ||
         platform.contains("N5K") ||
         platform.contains("N7K") ||
         platform.contains("ATN") ||
         platform.contains("NE40") )
        return true;
    return false;
}

bool equipmentNeighborsVecinoValidarEquipo(SEquipmentNeighborsInfo &e)
{
//    //Si exite un query del equipo entonces pertenece a la red
//    if ( consultarQueries )
//        if ( buscarEquipo( lstQueries, e->ip, e->nombreequipo ) )
//            return true;

    QRegExp exp,exp2;
    exp.setMinimal(true);
    exp2.setMinimal(true);

    exp.setPattern("\\w+\\.(telgua|enitel|claro|sercom|telecom)+");
    exp2.setPattern("\\w{13}(XM|GM|RM)+\\d+");

    if ( !e.nombreequipo.contains(exp) &&
         !e.nombreequipo.contains(exp2) )
    {
        if ( !platformValidate(e.plataforma) )
            return false;
    }

    return true;
}

QString interfaceDescription(QList<SInterfaceInfo> &lstInterfaceDescriptions, QString interfaz)
{
    for ( SInterfaceInfo  &id : lstInterfaceDescriptions )
    {
        if ( id.interfaz == interfaz )
            return id.description;
    }
    return "";
}

QList<SEquipmentNeighborsInfo*> interfaceHasEquipmentNeighborsInfo(QList<SEquipmentNeighborsInfo> &lstEquipmentNeighborsInfo, QList<SPortChannel> &lstPCInfo, QString i)
{
    QStringList lstVecinos;
    QList<SEquipmentNeighborsInfo*> lstSalida;

    QRegExp exp;
    //Si es un Po lo convertimos a una de sus interfaces, esto para poder averiguar si tiene vecinos
    exp.setPattern("(Po|BE)([0-9]+)");
    if ( i.contains(exp) )
    {
        QString group = exp.cap(2);
        for (SPortChannel &pc : lstPCInfo)
        {
            if ( group == pc.group )
            {
                i = pc.interfaz;
                break;
            }
        }
    }

    for (SEquipmentNeighborsInfo &equipo : lstEquipmentNeighborsInfo)
    {
        if ( equipo.interfazestesalida != i || !equipo.operativo  )
            continue;

        QString vecinonombre = simplicateName(equipo.nombreequipo);

        //verificamos si ya se habia agregado un equipo con el mismo nombre. Caso de que se aprenda dos veces el mismo equipo
        //por la misma interfaz
        if ( lstVecinos.contains( vecinonombre ) )
            continue;
        else
            lstVecinos.append( vecinonombre );

        if ( equipmentNeighborsVecinoValidarEquipo(equipo) )
            lstSalida.append(&equipo);
    }
    return lstSalida;
}

bool interfaceIsL2(Queries &q, QString interfaz)
{
    //Si la interfaz pertenece a un Po se busca el mismo para poder averiguar si es L2 o L3
    //Se usa "interfaz" para averiguar la informacion de vlans u ospf ya sea el Po o la interfaz original
    //Al final se gráfica la interfaz
    QString interfazPO = interfaceToPortChannelInterface( q.portChannelInfo(),
                                                        interfaz,
                                                        q.platform() );

    for (SInterfaceVlans &ipv : q.interfacesPermitedVlansInfo() )
    {
        if ( ipv.interfaz == interfazPO )
        {
            int vlans = ipv.vlans.size();

            if ( vlans > 0 )
                return true;
            else
                return false;
        }
    }
    return false;
}

bool interfacesAreL2OneVlanMatches(Queries &q1, QString interfaz1, Queries &q2, QString interfaz2)
{
    //Si la interfaz pertenece a un Po se busca el mismo para poder averiguar si es L2 o L3
    //Se usa "interfaz" para averiguar la informacion de vlans u ospf ya sea el Po o la interfaz original
    //Al final se gráfica la interfaz
    QString interfazPO1 = interfaceToPortChannelInterface( q1.portChannelInfo(),
                                                        interfaz1,
                                                        q1.platform() );

    QString interfazPO2 = interfaceToPortChannelInterface( q2.portChannelInfo(),
                                                        interfaz2,
                                                        q2.platform() );

    //por lo menos una vlan tiene que se ser comun en ambos
    for (SInterfaceVlans &ipv1 : q1.interfacesPermitedVlansInfo())
    {
        if ( ipv1.interfaz == interfazPO1 )
        {
            for (SInterfaceVlans &ipv2 : q2.interfacesPermitedVlansInfo())
            {
                if ( ipv2.interfaz == interfazPO2 )
                {
                    for (QString vlan : ipv1.vlans)
                    {
                        if ( ipv2.vlans.contains( vlan ) )
                            return true;
                    }
                    break;
                }
            }
            break;
        }
    }
    return false;
}

bool interfaceIsL3(Queries &q, QString interfaz)
{
    //Si la interfaz pertenece a un Po se busca el mismo para poder averiguar si es L2 o L3
    //Se usa "interfaz" para averiguar la informacion de vlans u ospf ya sea el Po o la interfaz original
    //Al final se gráfica la interfaz
    QString interfazPO1 = interfaceToPortChannelInterface( q.portChannelInfo(),
                                                        interfaz,
                                                        q.platform() );

    QRegExp exp( interfazPO1+"\\.\\d+" ); //para verificar si existe una sesion ospf en un subinterfa, si si, regresamos true
    for (SOSPFInfo &oi1 : q.ospfInfo())
    {
        if ( oi1.interfaz == interfazPO1 ||
             oi1.interfaz.contains( exp ) )
            return true;
    }
    return false;
}

bool interfaceIsInVRF(Queries &q, QString interfaz)
{
    QString interfazPO1 = interfaceToPortChannelInterface( q.portChannelInfo(),
                                                        interfaz,
                                                        q.platform() );

    for (SVrfInfo &vi : q.vrfsInfo())
    {
        if ( vi.interfaz == interfazPO1 &&
             !vi.vrf.isEmpty() )
            return true;
    }
    return false;
}

QString interfaceOspfArea(Queries &q, QString interfaz)
{
    QString interfazPO1 = interfaceToPortChannelInterface( q.portChannelInfo(),
                                                        interfaz,
                                                        q.platform() );

    for (SOSPFInfo &io : q.ospfInfo())
    {
        if ( io.interfaz == interfazPO1 )
            return io.area;
    }
    return "";
}

QString interfaceOspfProceso(Queries &q, QString interfaz)
{
    QString interfazPO1 = interfaceToPortChannelInterface( q.portChannelInfo(),
                                                           interfaz,
                                                           q.platform() );

    for (SOSPFInfo &io : q.ospfInfo())
    {
        if ( io.interfaz == interfazPO1 )
            return io.process;
    }
    return "";
}

QString interfaceToPortChannelInterface(QList<SPortChannel> &lst, QString interface, QString plataforma)
{
    //regresa la Po interfaz a la que pertenece la interfaz.
    //Si no pertenece a alguno PO se regresa la misma interfaz enviada.
    for (SPortChannel pc : lst)
    {
        if ( pc.interfaz == interface )
        {
            if ( equipmentOSFromPlatform(plataforma) == "IOS XR" )
                return "BE"+pc.group;
            else
                return "Po"+pc.group;
        }
    }
    return interface;
}

QStringList interfacePortChannelToInterfaces(QList<SPortChannel> &lst, QString pcInterface)
{
    QString group;
    QRegExp exp;
    exp.setPattern( "(Po|Bundle\\-Ether|BE)(\\d+)$" );
    if ( pcInterface.contains(exp) )
        group = exp.cap(2);

    if ( group.isEmpty() )
    {
        exp.setPattern("(Po|Bundle\\-Ether|BE)\\d+(/\\d+)+");
        if ( pcInterface.contains(exp) )
            group = pcInterface.split("/").last().simplified();
    }

    if ( group.isEmpty() )
        return QStringList() << pcInterface; //Interfaz proporcionada no esta en portchannel, se regresa la misma

    QStringList salida;
    for (SPortChannel pc : lst)
    {
        if ( group == pc.group )
            salida.append( pc.interfaz );
    }
    return salida;
}

bool sonIPsParejaMascara30_31(QString ip1, QString ip2 )
{
    QString ipuno = IP2Binario(ip1);
    QString ipdos = IP2Binario(ip2);

    if ( ipuno.left(30) == ipdos.left(30) ||
         ipuno.left(31) == ipdos.left(31) )
        return true;

    return false;
}

QString IP2Binario(QString ip)
{
    QStringList lst = ip.split(".");
    QString salida;
    foreach (QString oct, lst)
    {
        QString s="00000000";
        s.append(QString::number(oct.toInt(),2));
        salida.append( s.right(8) );
    }
    return salida;
}

bool validarIPperteneceAsegmento(QString IP, QString segmentoIP_mascara2digitos)
{
    QRegExp exp("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})/(\\d{1,2})");
    if ( !exp.exactMatch( segmentoIP_mascara2digitos ) )
        return false;

    QString segmento = exp.cap( 1 );
    short int mascara = exp.cap( 2 ).toShort();

    if ( IP2Binario(IP).left(mascara) == IP2Binario(segmento).left(mascara) )
        return true;

    return false;
}

Queries *buscarEquipoInterfazPorParejaDeIPMascara30(QList<Queries> &lst,
                                                    QString ipBuscar,
                                                    QString &interfaz,
                                                    QString ospfarea)
{
    if ( ipBuscar.isEmpty() )
        return nullptr;

    for (Queries &q : lst)
    {
        if ( !q.interfacesIpAddressesQuery )
            continue;

        for (SIpInfo &ii : q.interfacesIpAddressesInfo())
        {
            //verificamos que no se svi
            if ( ii.interfaz.contains("Vlan") ||
                 ii.interfaz.contains("BVI") ||
                 ii.interfaz.contains("BDI"))
                continue;

            //Que la ip no sea la misma
            if ( ipBuscar == ii.ip )
                continue;

            //que la ip no se este vacia
            if ( ii.ip.isEmpty() )
                continue;

            //verificamos que la interfaz tenga ospf
            bool encontrado=false;
            for (SOSPFInfo &oi : q.ospfInfo())
            {
                if ( oi.interfaz == ii.interfaz && oi.area == ospfarea )
                {
                    encontrado=true;
                    break;
                }
            }
            if ( !encontrado )
                continue;

            //verificando si en este equipo hay una IP que pertenezca al mismo segmento
            if ( !sonIPsParejaMascara30_31( ipBuscar, ii.ip ) )
                continue;

            //IP pertenece al mismo segmento /30

            //verificando que la interfaz de este equipo no esta en una vrf
            encontrado = false;
            for (SVrfInfo &vrf : q.vrfsInfo())
            {
                if ( vrf.interfaz == ii.interfaz &&
                     !vrf.vrf.isEmpty() )
                {
                    encontrado=true;
                    break;
                }
            }
            if ( encontrado )
                continue;

            if ( !q.ospfQuery )
                continue;

            //interfaz no esta en una vrf
            //interfaz tiene ospf
            //Las IPs pertenecen al mismo segmento /30
            interfaz=ii.interfaz;

            return &q;
        }
    }

    return nullptr;
}

bool existeEquipmentNeighborsOperativoHaciaEquipo(QList<SEquipmentNeighborsInfo> &lst, QString hostname )
{
    for (SEquipmentNeighborsInfo &vecino : lst)
    {
        if ( !vecino.operativo )
            continue;

        if ( vecino.nombreequipo == hostname )
            return true;
    }
    return false;
}

QString eliminarCaractaresNoImprimibles(QString txt)
{
    //eliminamos caracteres menores al ascii 32

    if ( txt.isEmpty() )
        return txt;

    int c=0;
    forever
    {
        char caracter = txt.at( c ).toLatin1();
        if ( caracter < 32 && caracter != 13 && caracter != 10 )
            txt.remove(c,1);
        else
            c++;

        if ( c == txt.size() )
            break;
    }
    return txt;
}

//SIpInfo* buscarARP(QList<Queries *> lstQueries, QString IPoMAc)
//{
//    foreach (Queries *q, lstQueries)
//    {
//        if ( !q->arpsQuery )
//            return nullptr; //si al primer queries no tiene info de arp se regresa null ya que los demas tampoco tendran

//        foreach (SIpInfo *arp, q->arpsInfo())
//        {
//            if ( arp->ip == IPoMAc )
//                return arp;
//            if ( arp->mac == IPoMAc )
//                return arp;
//        }
//    }
//    return nullptr;
//}

bool mplsInterfaceStatus(QString interface,
                         QString &txtStatusOut,
                         QList<SMplsLdpInfo> &lstDiscovery,
                         QList<SMplsLdpInfo> &lstInterfaces)
{
    for (SMplsLdpInfo &discoveryInfo : lstDiscovery)
    {
        if ( discoveryInfo.interfaz == interface )
        {
            //si se encuentra en discovery esta arriba y obviamente configurado
            txtStatusOut = "up";
            return true;
        }
    }

    //No estaba arriba, vemos en las interfaces si esta configurado
    for (SMplsLdpInfo &interfaceInfo : lstInterfaces)
    {
        if ( interfaceInfo.interfaz == interface )
        {
            //Si se encuentra
            txtStatusOut = "down";
            return false;
        }
    }

    //no estaba en las interfaces, significa que no estaba configurado
    txtStatusOut = "No config";
    return true;
}

bool ospfInterfaceStatus(QString interface,
                         QString& txtStatusOut,
                         QList<SOSPFInfo> &lstOspfInfo )
{
    for (SOSPFInfo &info : lstOspfInfo)
    {
        if ( info.interfaz == interface )
        {
            if ( info.state.contains("FULL") )
            {
                txtStatusOut = "up";
                return true;
            }
            else
            {
                txtStatusOut = "down";
                return false;
            }
        }
    }

    //no estaba en las interfaces, significa que no estaba configurado
    txtStatusOut = "No config";
    return true;
}

QString buscarArchivoConsultasActual(QString path, QString filelabel, QString grupo)
{
    //buscando el archivo de consultas mas actual a actualizar
    QString file;
    QDateTime mieDateTime;
    QDir dir(path);
    QDirIterator dirIter( path, QDir::Files );
    QRegExp exp(grupo+"_"+filelabel+"_*Queries_"+dir.dirName()+"_(\\d{4}-\\d{2}-\\d{2}-\\d{2}-\\d{2}-\\d{2})\\.dat");
    while (dirIter.hasNext())
    {
        QString filepath = dirIter.next();

        if ( filepath.contains(exp) )
        {
            QDateTime dateTime = QDateTime::fromString( exp.cap(1),"yyyy-MM-dd-hh-mm-ss" );
            if ( dateTime > mieDateTime )
            {
                file = filepath;
                mieDateTime=dateTime;
            }
        }
    }
    return file;
}

QString nombreArchivoLstQueries(LstQueries &lstQ)
{
    return lstQ.grupo +
            (lstQ.label.isEmpty()?"":"_"+lstQ.label)+
            "_Queries"+
            (lstQ.tipoconsulta.isEmpty()?"":"_"+lstQ.tipoconsulta)+
            "_"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".dat";
}

QString estandarizarInterfaz(QString interfaz)
{
    interfaz.replace("nGigabitEthernet","");
    interfaz.replace("rtyGigabitEthernet","");
    interfaz.replace("Bundle-Ether","BE");
    interfaz.replace("Port-channel","Po",Qt::CaseInsensitive);
    interfaz.replace("gabitEthernet","");
    interfaz.replace("stEthernet","");
    interfaz.replace("ndredGigE","");
    interfaz.replace("rtyGigE","");
    interfaz.replace("opback","");
    interfaz.replace("nGigE","");
    interfaz.replace("GigE","");
    interfaz.replace("POS","PO");
    interfaz.replace("Ethernet","Eth");
    interfaz.replace("Vlanif","Vlan");
    interfaz.replace("VLAN","Vlan");
    interfaz.replace("Po0/0/","Po");
    if ( !interfaz.contains("100GE") ) interfaz.replace("GE","Gi",Qt::CaseSensitive);

    return interfaz;
}

QString estandarizarProtocoloEnrutamiento(QString proto)
{
    if ( proto == "S" || proto == "Static" ) return "STATIC";
    //TODO agregar los demas

    return proto;
}

bool continuarPorsiguienteInterfazMismoDominioOSPF(Queries &q,
                                                   QString interfazOipDondeViene,
                                                   QString interfazSiguienteEquipo )
{
    if ( interfazOipDondeViene.isEmpty() ) //primer equipo
    {
        qCDebug(queriesthreadNeighbors) << q.ip() << "Primer equipo. Se agrega";
        return true;
    }

    qCDebug(queriesthreadNeighbors) << q.ip() << "continuarPorsiguienteInterfazMismoDominioOSPF" << interfazOipDondeViene << interfazSiguienteEquipo;

    QString ospfProceso;
    QString interfazDeDondeSeViene;
    QRegExp expIP("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
    if ( !interfazOipDondeViene.contains(expIP) )
        //se paso una interfaz de informacion de cdp
        interfazDeDondeSeViene = interfazOipDondeViene;
    else
    {
        //como se paso una ip de ospf hay que buscar la interfaz a la que pertenece
        for ( SIpInfo &ii : q.interfacesIpAddressesInfo() )
        {
            if ( sonIPsParejaMascara30_31( ii.ip, interfazOipDondeViene ) )
            {
                interfazDeDondeSeViene = ii.interfaz;
                break;
            }
        }

        if ( interfazDeDondeSeViene.isEmpty() )
        {
            //no se encontro la interfaz por medio de la ip
            qCDebug(queriesthreadNeighbors) << q.ip() << "continuarPorsiguienteInterfazMismoDominioOSPF: No se encontro la interfaz de donde se viene";
            return false;
        }
    }

    //buscamos el proceso de la interfaz de donde se viene
    interfazDeDondeSeViene = interfaceToPortChannelInterface(q.portChannelInfo(),
                                                             interfazDeDondeSeViene,
                                                             q.platform());       

    for ( SOSPFInfo &oi : q.ospfInfo() )
    {
        if ( oi.interfaz == interfazDeDondeSeViene )
        {
            ospfProceso = oi.process;
            break;
        }
    }

    if ( ospfProceso.isEmpty() )
    {
        qCDebug(queriesthreadNeighbors) << q.ip() << "continuarPorsiguienteInterfazMismoDominioOSPF: no se encuentra proceso de la interfaz de donde se viene";
        return false;
    }

    //verificamos si la interfaz siguiente pertenece al mismo proceso que la interfaz donde se viene
    interfazSiguienteEquipo = interfaceToPortChannelInterface(q.portChannelInfo(),
                                                              interfazSiguienteEquipo,
                                                              q.platform());

    qCDebug(queriesthreadNeighbors) << q.ip() << "interfazdedondeseviene" << interfazDeDondeSeViene
                                    << "interfazSiguienteEquipo" << interfazSiguienteEquipo;

    for ( SOSPFInfo &oi : q.ospfInfo() )
    {
        if ( oi.interfaz == interfazSiguienteEquipo )
        {            
            if ( ospfProceso == oi.process )
                return true;
            else
            {
                qCDebug(queriesthreadNeighbors) << q.ip() << "continuarPorsiguienteInterfazMismoDominioOSPF: proceso no es igual del de entrada y salida"
                         << interfazDeDondeSeViene << interfazSiguienteEquipo;
                return false;
            }
        }
    }

    qCDebug(queriesthreadNeighbors) << q.ip() << "continuarPorsiguienteInterfazMismoDominioOSPF ERROR";
    return false;
}
