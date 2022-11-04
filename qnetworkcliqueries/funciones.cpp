#include "funciones.h"
#include "queries.h"
#include "queriesthread.h"
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
              platform.contains("380") ||
              platform.contains("ASR1002"))
        return "IOS";
    else if ( platform.contains("N5K") ||
              platform.contains("N7K") ||
              platform.contains("Nexus") )
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
    if ( ip.contains("/") )
        ip = ip.split("/",QString::SkipEmptyParts).first();

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

QString binario2IP(QString ip)
{
    if ( ip.size() != 32 )
        return "";

    bool ok;
    QString octeto1 = QString::number( ip.mid( 0,8 ).toUInt(&ok,2));
    QString octeto2 = QString::number( ip.mid( 8,8 ).toUInt(&ok,2));
    QString octeto3 = QString::number( ip.mid( 16,8 ).toUInt(&ok,2));
    QString octeto4 = QString::number( ip.mid( 24,8 ).toUInt(&ok,2));

    return octeto1+"."+octeto2+"."+octeto3+"."+octeto4;
}

bool esIP1MayorQueIP2(QString ip1, QString ip2)
{
    bool ok;
    unsigned int IP1 = IP2Binario( ip1 ).toUInt(&ok,2);
    unsigned int IP2 = IP2Binario( ip2 ).toUInt(&ok,2);

    if ( IP1 > IP2 )
        return true;

    return false;
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

bool isRedPrivada(QString red)
{
    if ( red.contains("/") )
        red = red.split("/").first();

    if ( validarIPperteneceAsegmento(red,"10.0.0.0/8") )
        return true;
    if ( validarIPperteneceAsegmento(red,"172.16.0.0/12") )
        return true;
    if ( validarIPperteneceAsegmento(red,"192.168.0.0/16") )
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
    interfaz.replace("BVI","BV");
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

QString convertirMascaraMayorAmenor(QString red, int mascara)
{
    QRegExp exp("/\\d+");
    red.replace(exp,"");
    red = IP2Binario( red );
    red = red.left(mascara);
    red.resize(32,'0');
    return binario2IP( red )+"/"+QString::number(mascara);
}

QString convertirMascaraOctetosAdos(QString mascara)
{
    QString mascarabinario = IP2Binario(mascara);
    return QString::number( mascarabinario.count("1") );
}

QList<QStringList> tableMultiRow2Table(QString &txt,
                                       QMap<int,QRegExp> &mapPosicionesExpCampos)
{
    QList<QStringList> lstSalida;
    if ( mapPosicionesExpCampos.isEmpty() )
        return lstSalida;

    qDebug() << "\n\n\n tableMultiRow2Table ************************************* ";

    QStringList *lstFilaActual=nullptr;
    QMapIterator<int,QRegExp> imap(mapPosicionesExpCampos);
    uint c=0;
    for ( QString line : txt.split("\n",QString::SkipEmptyParts) )
    {
        //cada inicio de linea se empieza a analizar los campos desde la primera posicion
        //a medida de que se vaya avanzando con las fila se van a gregando los campos
        //se inicia una nueva fila cuando se encuentre nuevamente un campo en la primera posicion

//        qDebug() << "line ***" << line;
//        qDebug() << "line size" << line.size();

        imap.toFront();
        imap.next();
        bool imapOnFirst=true;

        uint d=0;
        forever
        {
            int pos = imap.key();
            QRegExp exp = imap.value();
            d++;

//            qDebug() << "c" << c << "d" << d << "pos" << pos << "exp" << exp.pattern() << "imapOnFirst" << imapOnFirst;

            if ( pos >= line.size() )
            {
//                qDebug() << "no queda info pos:" << pos << "line.size" << line.size();
                break;
            }

            int posmatch = exp.indexIn( line, pos );

//            qDebug() << "pos" << pos << "posmatch" << posmatch;

            if ( posmatch == pos )
            {
                if ( imapOnFirst )
                {
//                    qDebug() << "Nueva fila **********************************";
                    QStringList lst;
                    lstSalida.append(lst);
                    lstFilaActual = &lstSalida[lstSalida.size()-1];
                    c=1;
                }
                else
                    c++;

//                qDebug() << "agregando campo" << exp.cap(0);
                lstFilaActual->append( exp.cap(0).simplified() );
            }
            else
            {
//                qDebug() << "no se encuentra";
            }

            //Si no se encontro el campo se agrega uno vacio
            if ( (c+1) == d && c > 1 )
            {
                c++;
                lstFilaActual->append("");
//                qDebug() << "2222";
            }

            if ( lstFilaActual )
            {
                if ( imap.hasNext() )
                {
//                    qDebug() << "1111";
                    imap.next();
                    imapOnFirst=false;
                }
                else
                {
//                    qDebug() << "3333";
                    lstFilaActual=nullptr;
                    c=0;
                    break;
                }
            }
            else
            {
//                qDebug() << "4444";
                break;
            }
        }
    }

    return lstSalida;
}

QString filasIndentadasSubirAanterior(QString txt)
{
    QStringList lstSalida;

    QRegExp exp("^ +");
    for ( QString line : txt.split("\n") )
    {
        if ( lstSalida.isEmpty() )
        {
            //se agrega la primera linea para empezar
            lstSalida.append(line);
            continue;
        }

        if ( line.contains(exp) )

            //empieza con espacio por lo que esta indentado
            lstSalida[lstSalida.size()-1].append(line);
        else
            lstSalida.append(line);
    }

    return lstSalida.join("\n");
}

//QList<SBGPNeighborASPath> bgpNeighborASPathFromEquiposUfinet(QStringList lstIPs,
//                                                       bool agregarRedesMayoresMascara24)
//{
//    qDebug() << "bgpNeighborASPathFromEquiposUfinet list" << lstIPs;

//    if ( lstIPs.isEmpty() )
//    {
//        QList<SBGPNeighborASPath> list;
//        return list;
//    }

//    QueriesConfiguration qc;

//    QueriesThread *qt = new QueriesThread;
//    qt->setLstIP( lstIPs );
//    qt->setConnectionProtocol( QRemoteShell::SSHTelnet );
//    qt->setInterval( 1000 );
//    qt->setSimultaneos( 3 );
//    qt->setMaxParalelos( 40 );

//    qc.clear();
//    qc.addQueryParameter({
//                             //IOS XR

//                             {"FuncionInfo_txt",
//                              "sh rpl route-policy","","IOS XR",""},
//                             {"FuncionInfo_txt",
//                              "sh run formal | i \"router bgp 52468 vrf INTERNET .+ route-policy .+ in\"","","IOS XR",""},
//                             {"FuncionInfo_txt",
//                              "sh bgp vrf INTERNET ipv6 unicast summary","","IOS XR",""},
//                             {"FuncionInfo_txt",
//                              "sh bgp vrf INTERNET summary","","IOS XR",""},

//                             //VRP

//                             {"FuncionInfo_txt",
//                              "display current-configuration configuration xpl-filter","","VRP",""},
//                             {"FuncionInfo_txt",
//                              "display current-configuration configuration bgp 52468 | include \"route-filter .+ import\""
//                              ,"","VRP",""},
//                             {"FuncionInfo_txt",
//                              "display bgp vpnv6 vpn-instance INTERNET peer","","VRP",""},
//                             {"FuncionInfo_txt",
//                              "display bgp vpnv4 vpn-instance INTERNET peer","","VRP",""},

//                         });
//    qt->setQueriesConfiguration( qc );

//    qt->setOptions({QueryOpcion::Connect, QueryOpcion::Platform, QueryOpcion::Funcion, QueryOpcion::Exit });
//    qt->iniciarSync();

//    QMap<QString,QString> mapIPASN;  //para guardar el ASN de un vecino
//    QMap<QString,QString> mapPEsVecinosAConsultar;
//    QMap<QString,QString> _mapRoutePolicies;

//    QStringList _lstPeNeighborRPIn;

//    for ( Queries &qry : qt->lstQueries() )
//    {
//        QStringList lstRoutePoliciesIxpIpt;
//        QStringList lstNeighbors;

//        QStringList lstTXT = qry.funcionQuery->lstTxtReceived();
//        if ( lstTXT.size() < 4 )
//            continue;

//        //route policies
//        _mapRoutePolicies.insert( qry.ip(), lstTXT.at(0));
//        QString lastRP;
//        for ( QString line : lstTXT.at(0).split("\n") )
//        {
//            //IOS XR - VRP
//            line = line.simplified();
//            QRegExp exp("(xpl route-filter|route-policy) (\\S+)$");
//            if ( line.contains(exp) )
//            {
//                lastRP = exp.cap(2);
//                continue;
//            }

//            exp.setPattern("(set|apply) community.+(52468:3[1-3]0|no-advertise)");
//            if ( line.contains(exp) )
//            {
//                qDebug() << "***" << qry.ip() << "agregando policy de ixp ipt" << lastRP;
//                lstRoutePoliciesIxpIpt.append( lastRP );
//            }
//        }

//        //neighbor RP
//        for ( QString line : lstTXT.at(1).split("\n") )
//        {
//            line = line.simplified();

//            QString neighbor;
//            QString rp;

//            QRegExp exp(".+ neighbor (.+) address-family .+ route-policy (\\S+) in");
//            if ( line.contains(exp) )
//            {
//                //IOS XR
//                neighbor = exp.cap(1);
//                rp = exp.cap(2);
//            }

//            exp.setPattern("peer (.+) route-filter (\\S+) import");
//            if ( line.contains(exp) )
//            {
//                //VRP
//                neighbor = exp.cap(1);
//                rp = exp.cap(2);
//            }

//            if ( !neighbor.isEmpty() && !rp.isEmpty() )
//            {
//                _lstPeNeighborRPIn.append(qry.ip()+"*"+neighbor+"*"+rp);
//                if ( !lstRoutePoliciesIxpIpt.contains(rp) && !lstNeighbors.contains(neighbor) )
//                {
//                    qDebug() << "***" << qry.ip() << "neighbor" << neighbor <<
//                                "con RP aceptado, se agrega a validacion de prefijos aprendidos y de ASN";
//                    lstNeighbors.append( neighbor );
//                }
//            }
//        }

//        //bgp neigh ipv6 unicast summary, vemos si se permite el vecino segun la politica y si se aprenden menos de 300 rutas
//        QString ips;
//        QString summaryIPv6TXT;
//        if ( qry.os() == "IOS XR" )
//            summaryIPv6TXT = filasIndentadasSubirAanterior(lstTXT.at(2));
//        else if ( qry.os() == "VRP" )
//            summaryIPv6TXT = lstTXT.at(2);
//        for ( QString line : summaryIPv6TXT.split("\n") )
//        {
//            line = line.simplified();

//            qDebug() << "Summary IPV6. IPs de bgp a agregar" << qry.ip() << line;

//            if ( line.contains("52468") )
//            {
//                qDebug() << "No se agrega vecino bgp a la consulta de redes" << line;
//                continue;
//            }

//            QRegExp exp("^(([a-f]|[A-F]|\\d+|\\:)+) +\\d+ +((\\d+|\\.)+) .+ +(\\d+)\\S*$");
//            if ( line.contains(exp) )
//            {
//                if ( lstNeighbors.contains(exp.cap(1)) && exp.cap(5).toInt() > 0 && exp.cap(5).toInt() < 300 )
//                {
//                    qDebug() << "IP de bgp a agregar a consulta" << qry.ip() << exp.cap(1) << exp.cap(3) << exp.cap(5);
//                    ips.append( (!ips.isEmpty()?",":"")+exp.cap(1) ); //se agrega ese vecino a la consulta

//                    mapIPASN.insert(qry.ip()+"_"+exp.cap(1),exp.cap(3));
//                }
//            }
//        }

//        //bgp neighbor summary, vemos si se permite el vecino segun la politica y si se aprenden menos de 15000 rutas
//        for ( QString line : lstTXT.at(3).split("\n") )
//        {
//            line = line.simplified();

//            qDebug() << "Summary. IPs de bgp a agregar" << qry.ip() << line;

//            if ( line.contains("52468") )
//            {
//                qDebug() << "No se agrega vecino bgp a la consulta de redes" << line;
//                continue;
//            }

//            QRegExp exp("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) +\\d+ +((\\d+|\\.)+) .+ +(\\d+)\\S*$");
//            if ( line.contains(exp) )
//                if ( lstNeighbors.contains(exp.cap(1)) && exp.cap(4).toInt() > 0 && exp.cap(4).toInt() < 15000 )
//                {
//                    qDebug() << "IP de bgp a agregar a consulta" << qry.ip() << exp.cap(1) << exp.cap(2) << exp.cap(4);
//                    ips.append( (!ips.isEmpty()?",":"")+exp.cap(1) ); //se agrega ese vecino a la consulta

//                    mapIPASN.insert(qry.ip()+"_"+exp.cap(1),exp.cap(2));
//                }
//        }

//        mapPEsVecinosAConsultar.insert( qry.ip(),ips ); //  key: 172.16.30.253 value: 192.168.1.1,192.168.1.2,192.168.1.3
//    }

//    qc.clear();
//    qt->deleteLater();

//    //***********************  Entrando a todos los PEs origenes y se consultan las redes de los vecinos bgp en la vrf

//    qDebug() << "Entrando a todos los PEs origenes y se consultan las redes de los vecinos bgp en la vrf";

//    qt = new QueriesThread;
//    qt->setLstIP( lstIPs );
//    qt->setConnectionProtocol( QRemoteShell::SSHTelnet );
//    qt->setInterval( 1000 );
//    qt->setSimultaneos( 3 );
//    qt->setMaxParalelos( 40 );

//    QMapIterator<QString,QString> imapPEsVecinosAConsultar(mapPEsVecinosAConsultar);
//    //agregando la consulta de rutas por los vecinos de bgp
//    while (imapPEsVecinosAConsultar.hasNext()) {
//        imapPEsVecinosAConsultar.next();
//        for ( QString neighbor : imapPEsVecinosAConsultar.value().split(",",QString::SkipEmptyParts) )
//        {
//            if (neighbor.contains(":"))
//                //IPV6
//                qc.addQueryParameter({
//                                         {"FuncionInfo_txt",
//                                          "sh bgp vrf INTERNET ipv6 unicast neighbors "+
//                                          neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
//                                         {"FuncionInfo_txt",
//                                          "display bgp vpnv6 vpn-instance INTERNET routing-table peer "+
//                                          neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
//                                     });
//            else
//                //IPV4
//                qc.addQueryParameter({
//                                         {"FuncionInfo_txt",
//                                          "sh bgp vrf INTERNET neighbors "+
//                                          neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
//                                         {"FuncionInfo_txt",
//                                          "display bgp vpnv4 vpn-instance INTERNET routing-table peer "+
//                                          neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
//                                     });
//        }

//        //agregando la consulta de network a distribuir por bgp IPv6
//        qc.addQueryParameter({
//                                 {"FuncionInfo_txt",
//                                  "sh run router bgp 52468 vrf INTERNET address-family ipv6 unicast",imapPEsVecinosAConsultar.key(),
//                                  "IOS XR",""},
//                                 //No consultamos para IPv6 HUAWEI ya que la consulta es la misma para IPv4. En el analisis lo separamos
//                                 /*{"FuncionInfo_txt",
//                                  "display current-configuration configuration bgp | i network|vpn-instance|#",imapPEsVecinosAConsultar.key(),
//                                  "VRP",""},*/
//                             });

//        //agregando la consulta de network a distribuir por bgp IPv4
//        qc.addQueryParameter({
//                                 {"FuncionInfo_txt",
//                                  "sh run router bgp 52468 vrf INTERNET address-family ipv4 unicast",imapPEsVecinosAConsultar.key(),
//                                  "IOS XR",""},
//                                 {"FuncionInfo_txt",
//                                  "display current-configuration configuration bgp | i network|vpn-instance|#",imapPEsVecinosAConsultar.key(),
//                                  "VRP",""},
//                             });
//    }
//    qt->setQueriesConfiguration( qc );

//    qt->setOptions({QueryOpcion::Connect, QueryOpcion::Platform, QueryOpcion::Funcion, QueryOpcion::Exit });
//    qt->iniciarSync();

//    //agregando

//    QList<SBGPNeighborASPath> lstSalidaIPv4;
//    QList<SBGPNeighborASPath> lstSalidaIPv6;

//    for ( Queries &qry : qt->lstQueries() )
//    {
//        int posInicioASPath=63;
//        QString lastIPv6;

//        for ( QString txt : qry.funcionQuery->lstTxtReceived() )
//        {
//            QStringList lines = txt.split("\n",QString::SkipEmptyParts);
//            if ( lines.isEmpty() )
//                continue;
//            QString line1 = lines.at(0);

//            qDebug() << "line1" << line1;

//            bool neighboripv6=false;
//            bool neighboripv4=false;
//            bool network=false;

//            if ( line1.contains("INTERNET ipv6 unicast neighbors") || line1.contains("vpnv6 vpn-instance INTERNET routing-table peer") )
//                neighboripv6=true;
//            if ( line1.contains("INTERNET neighbors") || line1.contains("vpnv4 vpn-instance INTERNET routing-table peer") )
//                neighboripv4=true;
//            if ( line1.contains("sh run router bgp") || line1.contains("display current-configuration configuration bgp ") )
//                network=true;

//            qDebug() << "Tipo" << neighboripv6 << neighboripv4 << network;

//            if ( neighboripv6 )
//            {
//                //IPV6

//                QList<QStringList> lstCampos;
//                if ( qry.os() == "IOS XR" )
//                {
//                    //IOS XR
//                    QMap<int,QRegExp> map;
//                    map.insert( 3,QRegExp("([0-9a-f]{1,4}:\\S+/\\d{1,3})"));
//                    map.insert(22,QRegExp("([0-9a-f]{1,4}:\\S+)"));
//                    map.insert(63,QRegExp(".+$"));

//                    lstCampos = tableMultiRow2Table(txt,map);
//                }
//                else if ( qry.os() == "VRP" )
//                {
//                    //VRP
//                    QString lastNetwork;
//                    QString lastPrefix;
//                    QString lastNexthop;
//                    for ( QString line : txt )
//                    {
//                        line = line.simplified();
//                        QRegExp exp("Network : (([a-f]|[a-f]|\\d+|\\:)+) +PrefixLen (\\d+)$");
//                        if ( line.contains(exp) )
//                        {
//                            lastNetwork = exp.cap(1);
//                            lastPrefix = exp.cap(3);
//                            continue;
//                        }
//                        exp.setPattern("");
//                        if ( "NextHop : (([a-f]|[a-f]|\\d+|\\:)+) +" )
//                        {
//                            lastNexthop = exp.cap(1);
//                            continue;
//                        }
//                        exp.setPattern("Path/Ogn : (.+)");
//                        {
//                            lstCampos.append( QStringList() << lastNetwork+"/"+lastPrefix << lastNexthop << exp.cap(1) );
//                            lastNetwork.clear();
//                            lastPrefix.clear();
//                            lastNexthop.clear();
//                        }
//                    }
//                }

//                for ( QStringList lst : lstCampos )
//                {
//                    if ( lst.size() != 3 )
//                        continue;

//                     QString red = lst.at(0);
//                     QString peer = lst.at(1);
//                     QString aspath = lst.at(2);
//                     aspath.replace(" i","");
//                     aspath.replace(" ?","");

//                     if ( red.split("/").at(1).toInt() > 48 )
//                         continue;

//                     QString ASN = mapIPASN.value( qry.ip()+"_"+peer );

//                     //buscamos el Route Policy del neighbor
//                     QString rp;
//                     for ( QString PeNeighborRPIn : _lstPeNeighborRPIn )
//                     {
//                         QStringList data = PeNeighborRPIn.split("*");
//                         if ( data.size() != 3 )
//                             continue;

//                         if ( data.at(0) == qry.ip() && data.at(1) == peer )
//                         {
//                             rp =  data.at(2);
//                             break;
//                         }
//                     }

//                     SBGPNeighborASPath i;
//                     i.peip = qry.ip();
//                     i.peerip = peer;
//                     i.peerasn = ASN;
//                     i.red = red;
//                     i.asnorigen = aspath.split(" ",QString::SkipEmptyParts).last().simplified();
//                     i.aspath = "52468 "+aspath.simplified();
//                     i.routepolicy = rp;

//                     lstSalidaIPv6.append(i);
//                }
//            }
//            else if ( neighboripv4 )
//            {
//                //IPV4

//                for ( QString line : lines )
//                {
//                    if (line.contains("Network") && line.contains("Metric") && line.contains("Weight") && line.contains("Path"))
//                    {
//                        //IOS XR
//                        posInicioASPath=line.indexOf("Path");
//                        qDebug() << "Nuevo posInicioASPath" << posInicioASPath;
//                        continue;
//                    }
//                    if (line.contains("Network") && line.contains("MED") && line.contains("LocPrf") && line.contains("Path"))
//                    {
//                        //VRP
//                        posInicioASPath=line.indexOf("Path")+1;
//                        qDebug() << "Nuevo posInicioASPath" << posInicioASPath;
//                        continue;
//                    }

//                    QRegExp exp(" *(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/\\d+) +(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) ");
//                    if ( line.contains(exp) )
//                    {
//                        if ( exp.cap(1) == "0.0.0.0/0" )
//                            continue;

//                        qDebug() << "Por vecino:" << qry.ip() << "agregando red y su verdadero origen" << exp.cap(1) << qry.ip()
//                                 << "desde" << exp.cap(2) << posInicioASPath;

//                        if ( exp.cap(1).split("/").at(1).toInt() > 24 && !agregarRedesMayoresMascara24 )
//                            continue;

//                        QString ASN = mapIPASN.value( qry.ip()+"_"+exp.cap(2) );
//                        if ( ASN.isEmpty() ) ASN="52468";

//                        //buscamos el Route Policy del neighbor
//                        QString rp;
//                        for ( QString PeNeighborRPIn : _lstPeNeighborRPIn )
//                        {
//                            QStringList data = PeNeighborRPIn.split("*");
//                            if ( data.size() != 3 )
//                                continue;

//                            if ( data.at(0) == qry.ip() && data.at(1) == exp.cap(2) )
//                            {
//                                rp =  data.at(2);
//                                break;
//                            }
//                        }

//                        QString aspath = "52468 "+line.mid(posInicioASPath);
//                        aspath.replace(" i","");
//                        aspath.replace(" ?","");

//                        SBGPNeighborASPath i;
//                        i.peip = qry.ip();
//                        i.peerip = exp.cap(2);
//                        i.peerasn = ASN;
//                        i.red = exp.cap(1);
//                        i.asnorigen = aspath.split(" ",QString::SkipEmptyParts).last().simplified();
//                        i.aspath = "52468 "+aspath.simplified();
//                        i.routepolicy = rp;

//                        lstSalidaIPv4.append(i);
//                    }
//                }
//            }
//            else if ( network )
//            {
//                //NETWORK

//                qDebug() << "Network" << qry.os() << qry.ip() << qry.hostName();

//                QString lastVpnInstance; //Para Huawei
//                for ( QString line : lines )
//                {
//                    line = line.simplified();
//                    qDebug() << "line" << line;

//                    QRegExp exp;

//                    if ( qry.os() == "IOS XR" )
//                    {
//                        //IOS XR
//                        exp.setPattern("network (.+) route-(policy|filter) (.+)");
//                        if ( line.contains(exp) )
//                        {
//                            QString red = exp.cap(1);
//                            if ( red.contains(".") )
//                            {
//                                if ( red.split("/").at(1).toInt() > 24 )
//                                    continue;

//                                SBGPNeighborASPath i;
//                                i.peip = qry.ip();
//                                i.peerasn = "52468";
//                                i.red = red;
//                                i.asnorigen = "52468";
//                                i.routepolicy = exp.cap(3);

//                                lstSalidaIPv4.append(i);
//                            }
//                            else if ( red.contains(":") )
//                            {
//                                if ( red.split("/").at(1).toInt() > 48 )
//                                    continue;

//                                SBGPNeighborASPath i;
//                                i.peip = qry.ip();
//                                i.peerasn = "52468";
//                                i.red = red;
//                                i.asnorigen = "52468";
//                                i.routepolicy = exp.cap(3);

//                                lstSalidaIPv6.append(i);
//                            }
//                        }
//                    }
//                    else if ( qry.os() == "VRP" )
//                    {
//                        //VRP
//                        exp.setPattern("ipv\\d-family vpn-instance (.+)$");
//                        if ( line.contains(exp) )
//                        {
//                            lastVpnInstance = exp.cap(1);
//                            qDebug() << "last vpn instance" << lastVpnInstance;
//                            continue;
//                        }

//                        if ( line == "#" )
//                        {
//                            lastVpnInstance.clear();
//                            qDebug() << "limpiando vrf";
//                            continue;
//                        }

//                        if ( lastVpnInstance!= "INTERNET" )
//                            continue;

//                        exp.setPattern("network (.+) (.+) route-(policy|filter) (.+)");
//                        if ( line.contains(exp) )
//                        {
//                            qDebug() << "Por network:" << qry.ip() << "agregando red y su verdadero origen" << exp.cap(1) << qry.ip();

//                            QString red = exp.cap(1);
//                            QString mascara = exp.cap(2);
//                            if ( red.contains(".") )
//                            {
//                                mascara = convertirMascaraOctetosAdos(mascara);
//                                qDebug() << "mascara" << mascara;
//                                if ( mascara.toInt() > 24 )
//                                    continue;

//                                SBGPNeighborASPath i;
//                                i.peip = qry.ip();
//                                i.peerasn = "52468";
//                                i.red = red+"/"+mascara;
//                                i.asnorigen = "52468";
//                                i.routepolicy = exp.cap(4);

//                                lstSalidaIPv4.append(i);
//                            }
//                            else if ( red.contains(":") )
//                            {
//                                qDebug() << "mascara" << mascara;
//                                if ( mascara.toInt() > 48 )
//                                    continue;

//                                SBGPNeighborASPath i;
//                                i.peip = qry.ip();
//                                i.peerasn = "52468";
//                                i.red = red+"/"+mascara;
//                                i.asnorigen = "52468";
//                                i.routepolicy = exp.cap(4);

//                                lstSalidaIPv6.append(i);
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }

//    qc.clear();
//    qt->deleteLater();

//    QList<SBGPNeighborASPath> lstSalida;

//    lstSalida.append( lstSalidaIPv4 );
//    lstSalida.append( lstSalidaIPv6 );

//    return lstSalida;
//}

//QList<SBGPNeighborASPath> bgpNeighborASPathFromEquiposNedetel(QStringList lstIPs, bool agregarRedesMayoresMascara24)
//{
//    //Nedetel antes de homologacion
//    //Clientes estan en la global y VRF INET-DEF
//    //Full table de Transito esta en las VRFs INET-Q e INET-G
//    //Peering en la VRF PEERING-Q y PEERING-G
//    //Caches en la VRF INET-DEF

//    qDebug() << "bgpNeighborASPathFromEquipos list" << lstIPs;

//    if ( lstIPs.isEmpty() )
//    {
//        QList<SBGPNeighborASPath> list;
//        return list;
//    }

//    QueriesConfiguration qc;

//    QueriesThread *qt = new QueriesThread;
//    qt->setLstIP( lstIPs );
//    qt->setConnectionProtocol( QRemoteShell::SSHTelnet );
//    qt->setInterval( 1000 );
//    qt->setSimultaneos( 3 );
//    qt->setMaxParalelos( 40 );

//    qc.clear();
//    qc.addQueryParameter({
//                             //IOS XR
////                             {"FuncionInfo_txt",
////                              "sh bgp vrf INET-DEF ipv6 unicast summary","","IOS XR",""},
////                             {"FuncionInfo_txt",
////                              "sh bgp ipv6 unicast summary","","IOS XR",""},
//                             {"FuncionInfo_txt",
//                              "sh rpl route-policy","","IOS XR",""},
//                             {"FuncionInfo_txt",
//                              "sh run formal | i \"router bgp 264668 vrf INET-DEF neighbor .+ route-policy .+ in\"","","IOS XR",""},
//                             {"FuncionInfo_txt",
//                              "sh run formal | i \"router bgp 264668 neighbor .+ route-policy .+ in\"","","IOS XR",""},
//                             {"FuncionInfo_txt",
//                              "sh bgp vrf INET-DEF summary","","IOS XR",""},
//                             {"FuncionInfo_txt",
//                              "sh bgp summary","","IOS XR",""},

//                             //VRP

////                             {"FuncionInfo_txt",
////                              "display bgp vpnv6 vpn-instance INTERNET peer","","VRP",""},
////                             {"FuncionInfo_txt",
////                              "display bgp IPV6 peer","","VRP",""},
////                             {"FuncionInfo_txt",
////                              "display bgp vpnv4 vpn-instance INTERNET peer","","VRP",""},
////                             {"FuncionInfo_txt",
////                              "display bgp IPV4 peer","","VRP",""},
//                         });
//    qt->setQueriesConfiguration( qc );

//    qt->setOptions({QueryOpcion::Connect, QueryOpcion::Platform, QueryOpcion::Funcion, QueryOpcion::Exit });
//    qt->iniciarSync();

//    QMap<QString,QString> mapIPASN;  //para guardar el ASN de un vecino
//    QMap<QString,QString> mapPEsVecinosAConsultar;
//    QMap<QString,QString> _mapRoutePolicies;

//    QStringList _lstPeNeighborRPIn;

//    for ( Queries &qry : qt->lstQueries() )
//    {
//        QStringList lstRoutePoliciesIxpIpt;
//        QStringList lstNeighbors;

//        QStringList lstTXT = qry.funcionQuery->lstTxtReceived();
//        if ( lstTXT.size() < 5 )
//            continue;

////        //bgp neigh ipv6 VRF unicast summary, vemos si se aprenden menos de 300 rutas
////        if ( qry.os() == "IOS XR" )
////            summaryIPv6TXT = filasIndentadasSubirAanterior(lstTXT.at(0));
////        else if ( qry.os() == "VRP" )
////            summaryIPv6TXT = lstTXT.at(0);
////        for ( QString line : summaryIPv6TXT.split("\n") )
////        {
////            line = line.simplified();

////            qDebug() << "Summary IPV6. IPs de bgp a agregar" << qry.ip() << line;

////            QRegExp exp("^(([a-f]|[A-F]|\\d+|\\:)+) +\\d+ +((\\d+|\\.)+) .+ +(\\d+)\\S*$");
////            if ( line.contains(exp) )
////            {
////                if ( exp.cap(5).toInt() > 0 && exp.cap(5).toInt() < 300 )
////                {
////                    qDebug() << "IP de bgp a agregar a consulta" << qry.ip() << exp.cap(1) << exp.cap(3) << exp.cap(5);
////                    ips.append( (!ips.isEmpty()?",":"")+exp.cap(1)+"_vrf" ); //se agrega ese vecino a la consulta

////                    mapIPASN.insert(qry.ip()+"_"+exp.cap(1),exp.cap(3));
////                }
////            }
////        }

////        //bgp neigh ipv6 unicast summary, vemos si se aprenden menos de 300 rutas
////        if ( qry.os() == "IOS XR" )
////            summaryIPv6TXT = filasIndentadasSubirAanterior(lstTXT.at(1));
////        else if ( qry.os() == "VRP" )
////            summaryIPv6TXT = lstTXT.at(1);
////        for ( QString line : summaryIPv6TXT.split("\n") )
////        {
////            line = line.simplified();

////            qDebug() << "Summary IPV6. IPs de bgp a agregar" << qry.ip() << line;

////            QRegExp exp("^(([a-f]|[A-F]|\\d+|\\:)+) +\\d+ +((\\d+|\\.)+) .+ +(\\d+)\\S*$");
////            if ( line.contains(exp) )
////            {
////                if ( exp.cap(5).toInt() > 0 && exp.cap(5).toInt() < 300 )
////                {
////                    qDebug() << "IP de bgp a agregar a consulta" << qry.ip() << exp.cap(1) << exp.cap(3) << exp.cap(5);
////                    ips.append( (!ips.isEmpty()?",":"")+exp.cap(1) ); //se agrega ese vecino a la consulta

////                    mapIPASN.insert(qry.ip()+"_"+exp.cap(1),exp.cap(3));
////                }
////            }
////        }

//        //route policies
//        _mapRoutePolicies.insert( qry.ip(), lstTXT.at(0));
//        QString lastRP;
//        for ( QString line : lstTXT.at(0).split("\n") )
//        {
//            //IOS XR - VRP
//            line = line.simplified();
//            QRegExp exp("(xpl route-filter|route-policy) (\\S+)$");
//            if ( line.contains(exp) )
//            {
//                lastRP = exp.cap(2);
//                continue;
//            }

//            exp.setPattern("(set|apply) community.+(26466:300|26466:350|26466:200)");
//            if ( line.contains(exp) )
//            {
//                qDebug() << "***" << qry.ip() << "agregando policy de ixp ipt" << lastRP;
//                lstRoutePoliciesIxpIpt.append( lastRP );
//            }
//        }

//        //neighbor RP
//        for ( int cc=1; cc<3; cc++ )
//            for ( QString line : lstTXT.at(cc).split("\n") )
//            {
//                line = line.simplified();

//                QString neighbor;
//                QString rp;

//                QRegExp exp(".+ neighbor (.+) address-family .+ route-policy (\\S+) in");
//                if ( line.contains(exp) )
//                {
//                    //IOS XR
//                    neighbor = exp.cap(1);
//                    rp = exp.cap(2);
//                }

//                exp.setPattern("peer (.+) route-filter (\\S+) import");
//                if ( line.contains(exp) )
//                {
//                    //VRP
//                    neighbor = exp.cap(1);
//                    rp = exp.cap(2);
//                }

//                if ( !neighbor.isEmpty() && !rp.isEmpty() )
//                {
//                    _lstPeNeighborRPIn.append(qry.ip()+"*"+neighbor+"*"+rp);
//                    if ( !lstRoutePoliciesIxpIpt.contains(rp) && !lstNeighbors.contains(neighbor) )
//                    {
//                        qDebug() << "***" << qry.ip() << "neighbor" << neighbor <<
//                                    "con RP aceptado, se agrega a validacion de prefijos aprendidos y de ASN";
//                        lstNeighbors.append( neighbor );
//                    }
//                }
//            }

//        //bgp neighbor VRF summary, vemos si se permite el vecino segun la politica y si se aprenden menos de 500 rutas
//        QString ips;
//        QString summaryIPv6TXT;
//        for ( QString line : lstTXT.at(3).split("\n") )
//        {
//            line = line.simplified();

//            qDebug() << "Summary. IPs de bgp a agregar" << qry.ip() << line;

//            QRegExp exp("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) +\\d+ +((\\d+|\\.)+) .+ +(\\d+)\\S*$");
//            if ( line.contains(exp) )
//                if ( lstNeighbors.contains(exp.cap(1)) && exp.cap(4).toInt() > 0 && exp.cap(4).toInt() < 500 )
//                {
//                    qDebug() << "IP de bgp a agregar a consulta" << qry.ip() << exp.cap(1) << exp.cap(2) << exp.cap(4);
//                    ips.append( (!ips.isEmpty()?",":"")+exp.cap(1)+"_vrf" ); //se agrega ese vecino a la consulta

//                    mapIPASN.insert(qry.ip()+"_"+exp.cap(1),exp.cap(2));
//                }
//        }

//        //bgp neighbor summary, vemos si se permite el vecino segun la politica y si se aprenden menos de 500 rutas
//        for ( QString line : lstTXT.at(4).split("\n") )
//        {
//            line = line.simplified();

//            qDebug() << "Summary. IPs de bgp a agregar" << qry.ip() << line;

//            QRegExp exp("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) +\\d+ +((\\d+|\\.)+) .+ +(\\d+)\\S*$");
//            if ( line.contains(exp) )
//                if ( lstNeighbors.contains(exp.cap(1)) &&
//                     !validarIPperteneceAsegmento(exp.cap(1),"172.18.0.0/16") &&
//                     exp.cap(4).toInt() > 0 &&
//                     exp.cap(4).toInt() < 500 )
//                {
//                    qDebug() << "IP de bgp a agregar a consulta" << qry.ip() << exp.cap(1) << exp.cap(2) << exp.cap(4);
//                    ips.append( (!ips.isEmpty()?",":"")+exp.cap(1) ); //se agrega ese vecino a la consulta

//                    mapIPASN.insert(qry.ip()+"_"+exp.cap(1),exp.cap(2));
//                }
//        }

//        //  key: 172.16.30.253 value: 192.168.1.1,192.168.1.2_vrf,192.168.1.3
//        mapPEsVecinosAConsultar.insert( qry.ip(),ips );
//    }

//    qc.clear();
//    qt->deleteLater();

//    //***********************  Entrando a todos los PEs origenes y se consultan las redes de los vecinos bgp

//    qDebug() << "Entrando a todos los PEs origenes y se consultan las redes de los vecinos bgp";

//    QList<SBGPNeighborASPath> lstSalidaIPv4;
//    QList<SBGPNeighborASPath> lstSalidaIPv6;

//    qt = new QueriesThread;
//    qt->setLstIP( lstIPs );
//    qt->setConnectionProtocol( QRemoteShell::SSHTelnet );
//    qt->setInterval( 1000 );
//    qt->setSimultaneos( 3 );
//    qt->setMaxParalelos( 40 );

//    QMapIterator<QString,QString> imapPEsVecinosAConsultar(mapPEsVecinosAConsultar);
//    //agregando la consulta de rutas por los vecinos de bgp
//    while (imapPEsVecinosAConsultar.hasNext()) {
//        imapPEsVecinosAConsultar.next();
//        for ( QString neighbor : imapPEsVecinosAConsultar.value().split(",",QString::SkipEmptyParts) )
//        {
//            bool vrf=false;
//            if ( neighbor.contains("_vrf") )
//            {
//                vrf=true;
//                neighbor.replace("_vrf","");
//            }
//            if (neighbor.contains(":"))
//            {
//                //IPV6
//                if ( vrf )
//                    qc.addQueryParameter({
//                                             {"FuncionInfo_txt",
//                                              "sh bgp vrf INET-DEF ipv6 unicast neighbors "+
//                                              neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
//                                             {"FuncionInfo_txt",
//                                              "display bgp vpnv6 vpn-instance INET-DEF routing-table peer "+
//                                              neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
//                                         });
//                else
//                    qc.addQueryParameter({
//                                             {"FuncionInfo_txt",
//                                              "sh bgp ipv6 unicast neighbors "+
//                                              neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
////                                             {"FuncionInfo_txt",
////                                              "display bgp routing-table peer "+
////                                              neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
//                                         });
//            }
//            else
//            {
//                //IPV4
//                if ( vrf )
//                    qc.addQueryParameter({
//                                             {"FuncionInfo_txt",
//                                              "sh bgp vrf INET-DEF ipv4 unicast neighbors "+
//                                              neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
//                                             {"FuncionInfo_txt",
//                                              "display bgp vpnv4 vpn-instance INTERNET routing-table peer "+
//                                              neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
//                                         });
//                else
//                    qc.addQueryParameter({
//                                             {"FuncionInfo_txt",
//                                              "sh bgp ipv4 unicast neighbors "+
//                                              neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
////                                             {"FuncionInfo_txt",
////                                              "display bgp vpnv4 vpn-instance INTERNET routing-table peer "+
////                                              neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
//                                         });
//            }
//        }

////        //agregando la consulta de network a distribuir por bgp IPv6 VRF
////        qc.addQueryParameter({
////                                 {"FuncionInfo_txt",
////                                  "sh run router bgp 264668 vrf INET-DEF address-family ipv6 unicast",imapPEsVecinosAConsultar.key(),
////                                  "IOS XR",""},
////                                 //No consultamos para IPv6 HUAWEI ya que la consulta es la misma para IPv4. En el analisis lo separamos
////                             });

//        //agregando la consulta de network a distribuir por bgp IPv4 VRF
//        qc.addQueryParameter({
//                                 {"FuncionInfo_txt",
//                                  "sh run router bgp 264668 vrf INET-DEF address-family ipv4 unicast",imapPEsVecinosAConsultar.key(),
//                                  "IOS XR",""},
////                                 {"FuncionInfo_txt",
////                                  "display current-configuration configuration bgp | i network|vpn-instance|#",imapPEsVecinosAConsultar.key(),
////                                  "VRP",""},
//                             });

////        //agregando la consulta de network a distribuir por bgp IPv6
////        qc.addQueryParameter({
////                                 {"FuncionInfo_txt",
////                                  "sh run router bgp 264668 address-family ipv6 unicast",imapPEsVecinosAConsultar.key(),
////                                  "IOS XR",""},
////                                 //No consultamos para IPv6 HUAWEI ya que la consulta es la misma para IPv4. En el analisis lo separamos
////                             });

//        //agregando la consulta de network a distribuir por bgp IPv4
//        qc.addQueryParameter({
//                                 {"FuncionInfo_txt",
//                                  "sh run router bgp 264668 address-family ipv4 unicast",imapPEsVecinosAConsultar.key(),
//                                  "IOS XR",""},
////                                 {"FuncionInfo_txt",
////                                  "display current-configuration configuration bgp | i network|vpn-instance|#",imapPEsVecinosAConsultar.key(),
////                                  "VRP",""},
//                             });
//    }
//    qt->setQueriesConfiguration( qc );

//    qt->setOptions({QueryOpcion::Connect, QueryOpcion::Platform, QueryOpcion::Funcion, QueryOpcion::Exit });
//    qt->iniciarSync();

//    //agregando

//    for ( Queries &qry : qt->lstQueries() )
//    {
//        int posInicioASPath=63;
//        QString lastIPv6;

//        for ( QString txt : qry.funcionQuery->lstTxtReceived() )
//        {
//            QStringList lines = txt.split("\n",QString::SkipEmptyParts);
//            if ( lines.isEmpty() )
//                continue;
//            QString line1 = lines.at(0);

//            qDebug() << "line1" << line1;

//            bool neighboripv6=false;
//            bool neighboripv4=false;
//            bool network=false;

//            if ( line1.contains("ipv6 unicast neighbors") /*|| line1.contains("vpnv6 vpn-instance INET-DEF routing-table peer")*/ )
//                neighboripv6=true;
//            else if ( line1.contains("ipv4 unicast neighbors") /*|| line1.contains("vpnv4 routing-table peer")*/ )
//                neighboripv4=true;
//            else if ( line1.contains("sh run router bgp") /*|| line1.contains("display current-configuration configuration bgp ")*/ )
//                network=true;

//            qDebug() << "Tipo" << neighboripv6 << neighboripv4 << network;

//            if ( neighboripv6 )
//            {
//                //IPV6

//                QList<QStringList> lstCampos;
//                if ( qry.os() == "IOS XR" )
//                {
//                    //IOS XR
//                    QMap<int,QRegExp> map;
//                    map.insert( 3,QRegExp("([0-9a-f]{1,4}:\\S+/\\d{1,3})"));
//                    map.insert(22,QRegExp("([0-9a-f]{1,4}:\\S+)"));
//                    map.insert(63,QRegExp(".+$"));

//                    lstCampos = tableMultiRow2Table(txt,map);
//                }
//                else if ( qry.os() == "VRP" )
//                {
//                    //VRP
//                    QString lastNetwork;
//                    QString lastPrefix;
//                    QString lastNexthop;
//                    for ( QString line : txt )
//                    {
//                        line = line.simplified();
//                        QRegExp exp("Network : (([a-f]|[a-f]|\\d+|\\:)+) +PrefixLen (\\d+)$");
//                        if ( line.contains(exp) )
//                        {
//                            lastNetwork = exp.cap(1);
//                            lastPrefix = exp.cap(3);
//                            continue;
//                        }
//                        exp.setPattern("");
//                        if ( "NextHop : (([a-f]|[a-f]|\\d+|\\:)+) +" )
//                        {
//                            lastNexthop = exp.cap(1);
//                            continue;
//                        }
//                        exp.setPattern("Path/Ogn : (.+)");
//                        {
//                            lstCampos.append( QStringList() << lastNetwork+"/"+lastPrefix << lastNexthop << exp.cap(1) );
//                            lastNetwork.clear();
//                            lastPrefix.clear();
//                            lastNexthop.clear();
//                        }
//                    }
//                }

//                for ( QStringList lst : lstCampos )
//                {
//                    if ( lst.size() != 3 )
//                        continue;

//                    QString red = lst.at(0);
//                    QString peer = lst.at(1);
//                    QString aspath = lst.at(2);
//                    aspath.replace(" i","");
//                    aspath.replace(" ?","");

//                    if ( red.split("/").at(1).toInt() > 48 )
//                        continue;

//                    QString ASN = mapIPASN.value( qry.ip()+"_"+peer );

//                    //buscamos el Route Policy del neighbor
//                    QString rp;
//                    for ( QString PeNeighborRPIn : _lstPeNeighborRPIn )
//                    {
//                        QStringList data = PeNeighborRPIn.split("*");
//                        if ( data.size() != 3 )
//                            continue;

//                        if ( data.at(0) == qry.ip() && data.at(1) == peer )
//                        {
//                            rp =  data.at(2);
//                            break;
//                        }
//                    }

//                    SBGPNeighborASPath i;
//                    i.peip = qry.ip();
//                    i.peerip = peer;
//                    i.peerasn = ASN;
//                    i.red = red;
//                    i.asnorigen = aspath.split(" ",QString::SkipEmptyParts).last().simplified();
//                    i.aspath = "264668 "+aspath.simplified();
//                    i.routepolicy = rp;

//                    lstSalidaIPv6.append(i);
//                }
//            }
//            else if ( neighboripv4 )
//            {
//                //IPV4

//                for ( QString line : lines )
//                {
//                    if (line.contains("Network") && line.contains("Metric") && line.contains("Weight") && line.contains("Path"))
//                    {
//                        //IOS XR
//                        posInicioASPath=line.indexOf("Path");
//                        qDebug() << "Nuevo posInicioASPath" << posInicioASPath;
//                        continue;
//                    }
//                    if (line.contains("Network") && line.contains("MED") && line.contains("LocPrf") && line.contains("Path"))
//                    {
//                        //VRP
//                        posInicioASPath=line.indexOf("Path")+1;
//                        qDebug() << "Nuevo posInicioASPath" << posInicioASPath;
//                        continue;
//                    }

//                    QRegExp exp(" *(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/\\d+) +(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) ");
//                    if ( line.contains(exp) )
//                    {
//                        if ( exp.cap(1) == "0.0.0.0/0" )
//                            continue;

//                        qDebug() << "Por vecino:" << qry.ip() << "agregando red y su verdadero origen" << exp.cap(1) << qry.ip()
//                                 << "desde" << exp.cap(2) << posInicioASPath;

//                        if ( exp.cap(1).split("/").at(1).toInt() > 24 && !agregarRedesMayoresMascara24 )
//                            continue;

//                        QString ASN = mapIPASN.value( qry.ip()+"_"+exp.cap(2) );
//                        if ( ASN.isEmpty() ) ASN="264668";

//                        //buscamos el Route Policy del neighbor
//                        QString rp;
//                        for ( QString PeNeighborRPIn : _lstPeNeighborRPIn )
//                        {
//                            QStringList data = PeNeighborRPIn.split("*");
//                            if ( data.size() != 3 )
//                                continue;

//                            if ( data.at(0) == qry.ip() && data.at(1) == exp.cap(2) )
//                            {
//                                rp =  data.at(2);
//                                break;
//                            }
//                        }

//                        QString aspath = "264668 "+line.mid(posInicioASPath);
//                        aspath.replace(" i","");
//                        aspath.replace(" ?","");

//                        SBGPNeighborASPath i;
//                        i.peip = qry.ip();
//                        i.peerip = exp.cap(2);
//                        i.peerasn = ASN;
//                        i.red = exp.cap(1);
//                        i.asnorigen = aspath.split(" ",QString::SkipEmptyParts).last().simplified();
//                        i.aspath = aspath.simplified();
//                        i.routepolicy = rp;

//                        lstSalidaIPv4.append(i);
//                    }
//                }
//            }
//            else if ( network )
//            {
//                //NETWORK

//                qDebug() << "Network" << qry.os() << qry.ip() << qry.hostName();

//                QString lastVpnInstance; //Para Huawei
//                for ( QString line : lines )
//                {
//                    line = line.simplified();
//                    qDebug() << "line" << line;

//                    QRegExp exp;
//                    QRegExp exp2;

//                    if ( qry.os() == "IOS XR" )
//                    {
//                        //IOS XR
//                        exp.setPattern("network (.+) route-(policy|filter) (.+)");
//                        exp2.setPattern("network (.+)$");

//                        QString red;
//                        QString policy;

//                        if ( line.contains(exp) )
//                        {
//                            red = exp.cap(1);
//                            policy = exp.cap(3);
//                        }
//                        else if ( line.contains(exp2) )
//                            red = exp2.cap(1);

//                        if ( !red.isEmpty() )
//                        {
//                            if ( red.contains(".") )
//                            {
//                                if ( red.split("/").at(1).toInt() > 24 )
//                                    continue;

//                                if ( isRedPrivada(red) )
//                                    continue;

//                                SBGPNeighborASPath i;
//                                i.peip = qry.ip();
//                                i.peerasn = "264668";
//                                i.red = red;
//                                i.asnorigen = "264668";
//                                i.routepolicy = policy;

//                                lstSalidaIPv4.append(i);
//                            }
//                            else if ( red.contains(":") )
//                            {
//                                if ( red.split("/").at(1).toInt() > 48 )
//                                    continue;

//                                SBGPNeighborASPath i;
//                                i.peip = qry.ip();
//                                i.peerasn = "264668";
//                                i.red = red;
//                                i.asnorigen = "264668";
//                                i.routepolicy = policy;

//                                lstSalidaIPv6.append(i);
//                            }
//                        }
//                    }
//                    else if ( qry.os() == "VRP" )
//                    {
//                        //VRP

//                        //Agregar red que no tiene policy

//                        exp.setPattern("ipv\\d-family vpn-instance (.+)$");
//                        if ( line.contains(exp) )
//                        {
//                            lastVpnInstance = exp.cap(1);
//                            qDebug() << "last vpn instance" << lastVpnInstance;
//                            continue;
//                        }

//                        if ( line == "#" )
//                        {
//                            lastVpnInstance.clear();
//                            qDebug() << "limpiando vrf";
//                            continue;
//                        }

//                        if ( lastVpnInstance!= "INTERNET" )
//                            continue;

//                        exp.setPattern("network (.+) (.+) route-(policy|filter) (.+)");
//                        if ( line.contains(exp) )
//                        {
//                            qDebug() << "Por network:" << qry.ip() << "agregando red y su verdadero origen" << exp.cap(1) << qry.ip();

//                            QString red = exp.cap(1);
//                            QString mascara = exp.cap(2);
//                            if ( red.contains(".") )
//                            {
//                                mascara = convertirMascaraOctetosAdos(mascara);
//                                qDebug() << "mascara" << mascara;
//                                if ( mascara.toInt() > 24 )
//                                    continue;

//                                SBGPNeighborASPath i;
//                                i.peip = qry.ip();
//                                i.peerasn = "264668";
//                                i.red = red+"/"+mascara;
//                                i.asnorigen = "264668";
//                                i.routepolicy = exp.cap(4);

//                                lstSalidaIPv4.append(i);
//                            }
//                            else if ( red.contains(":") )
//                            {
//                                qDebug() << "mascara" << mascara;
//                                if ( mascara.toInt() > 48 )
//                                    continue;

//                                SBGPNeighborASPath i;
//                                i.peip = qry.ip();
//                                i.peerasn = "264668";
//                                i.red = red+"/"+mascara;
//                                i.asnorigen = "264668";
//                                i.routepolicy = exp.cap(4);

//                                lstSalidaIPv6.append(i);
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }

//    qc.clear();
//    qt->deleteLater();

//    QList<SBGPNeighborASPath> lstSalida;

//    lstSalida.append( lstSalidaIPv4 );
//    lstSalida.append( lstSalidaIPv6 );

//    return lstSalida;
//}

QList<SBGPNeighborASPath> bgpNeighborASPathFromEquipos(QStringList lstIPs,
                                                       QStringList lstVRFs,
                                                       QString ASN,
                                                       QString comunidadesProveedor,
                                                       QStringList lstNeighborEnRedFiltroNegar,
                                                       bool agregarRedesMayoresMascara24)
{
    qDebug() << "\n\n\n----- bgpNeighborASPathFromEquipos INICIANDO";
    qDebug() << "bgpNeighborASPathFromEquipos list" << lstIPs;
    qDebug() << "\n\n\n----- bgpNeighborASPathFromEquipos Consulta de RPL BGP Summary";

    if ( lstIPs.isEmpty() )
    {
        QList<SBGPNeighborASPath> list;
        return list;
    }

    QStringList lstConfiguraciones;

    QueriesConfiguration qc;

    QueriesThread *qt = new QueriesThread;
    qt->setLstIP( lstIPs );
    qt->setConnectionProtocol( QRemoteShell::SSHTelnet );
    qt->setInterval( 1000 );
    qt->setSimultaneos( 3 );
    qt->setMaxParalelos( 40 );

    qc.clear();

    qc.addQueryParameter({
                             //IOS XR
                             {"FuncionInfo_txt",
                              "sh rpl route-policy","","IOS XR",""},

                             //VRP
                             {"FuncionInfo_txt",
                              "display current-configuration configuration xpl-filter","","VRP",""},
                             //Unica consulta global de neighbor RP, en la misma va para la global y VRFs
                             {"FuncionInfo_txt",
                              "display current-configuration configuration bgp "+ASN+" | include \"route-filter .+ import\""
                              ,"","VRP",""},
                         });

    for ( QString vrf : lstVRFs )
    {
        if ( vrf == "default" )
            vrf = "";

        if ( vrf.isEmpty() )
            //Global
            qc.addQueryParameter({
                                     //IOS XR
                                     {"FuncionInfo_txt",
                                      "sh run formal | i \"router bgp "+ASN+" neighbor .+ route-policy .+ in\"","","IOS XR",""},
                                     {"FuncionInfo_txt",
                                      "sh bgp ipv4 unicast summary","","IOS XR",""},
                                     {"FuncionInfo_txt",
                                      "sh bgp ipv6 unicast summary","","IOS XR",""},

                                     //VRP
                                     //No se consulta configuración de router BGP por global ni VRF. Se realiza una sola Global
                                     {"FuncionInfo_txt",
                                      "display bgp ipv4 peer","","VRP",""},
                                     {"FuncionInfo_txt",
                                      "display bgp ipv6 peer","","VRP",""},
                                 });
        else
            //VRF
            qc.addQueryParameter({
                                     //IOS XR
                                     {"FuncionInfo_txt",
                                      "sh run formal | i \"router bgp "+ASN+" vrf "+vrf+" neighbor .+ route-policy .+ in\"","",
                                      "IOS XR",""},
                                     {"FuncionInfo_txt",
                                      "sh bgp vrf "+vrf+" ipv4 unicast summary","","IOS XR",""},
                                     {"FuncionInfo_txt",
                                      "sh bgp vrf "+vrf+" ipv6 unicast summary","","IOS XR",""},

                                     //VRP
                                     //No se consulta configuración de router BGP por global ni VRF. Se realiza una sola Global
                                     {"FuncionInfo_txt",
                                      "display bgp vpnv4 vpn-instance INTERNET peer","","VRP",""},
                                     {"FuncionInfo_txt",
                                      "display bgp vpnv6 vpn-instance INTERNET peer","","VRP",""},
                                 });
    }

    qt->setQueriesConfiguration( qc );

    qt->setOptions({QueryOpcion::Connect, QueryOpcion::Platform, QueryOpcion::Funcion, QueryOpcion::Exit });
    qt->iniciarSync();

    QMap<QString,QString> mapIPASN;  //para guardar el ASN de un vecino
    QMap<QString,QString> mapPEsVecinosAConsultar;
    QMap<QString,QString> _mapRoutePolicies;

    QStringList _lstPeNeighborRPIn;

    for ( Queries *qry : qt->lstQueriesPointers() )
    {
        QStringList lstRoutePoliciesIxpIpt;
        QStringList lstNeighbors;

        QString ips;
        for ( QString txt : qry->funcionQuery->lstTxtReceived() )
        {
            QStringList lines = txt.split("\n",QString::SkipEmptyParts);
            if ( lines.isEmpty() )
                continue;
            QString line1 = lines.at(0);

            qDebug() << "line1" << line1;

            bool rplrp=false;
            bool neighborrp=false;
            bool bgpipv4summary=false;
            bool bgpipv6summary=false;

            if ( line1.contains("sh rpl route-policy") || line1.contains("display current-configuration configuration xpl-filter") )
                rplrp=true;
            else if ( line1.contains("sh run formal | i \"router bgp") || line1.contains("route-filter .+ import") )
                neighborrp=true;
            else if ( line1.contains("ipv4 unicast summary") ||
                      line1.contains("bgp ipv4 peer") ||
                      line1.contains("bgp vpnv4 vpn-instance") )
                bgpipv4summary=true;
            else if ( line1.contains("ipv6 unicast summary") ||
                      line1.contains("bgp ipv6 peer") ||
                      line1.contains("bgp vpnv6 vpn-instance"))
                bgpipv6summary=true;

            qDebug() << "Tipo" << rplrp << neighborrp << bgpipv4summary << bgpipv6summary;

            if ( rplrp )
            {
                //Configuración de Route Policies

                _mapRoutePolicies.insert( qry->ip(), txt);
                QString lastRP;
                for ( QString line : lines )
                {
                    //IOS XR - VRP
                    line = line.simplified();
                    QRegExp exp("(xpl route-filter|route-policy) (\\S+)$");
                    if ( line.contains(exp) )
                    {
                        lastRP = exp.cap(2);
                        continue;
                    }

                    exp.setPattern("(set|apply) community.+("+comunidadesProveedor+")");
                    if ( line.contains(exp) )
                    {
                        qDebug() << "***" << qry->ip() << "agregando policy de ixp ipt" << lastRP;
                        lstRoutePoliciesIxpIpt.append( lastRP );
                    }
                }
            }
            else if ( neighborrp )
            {
                //Configuracion de neighbor y su politica de entrada

                for ( QString line : lines )
                {
                    line = line.simplified();

                    QString neighbor;
                    QString rp;

                    QRegExp exp(".+ neighbor (.+) address-family .+ route-policy (\\S+) in");
                    if ( line.contains(exp) )
                    {
                        //IOS XR
                        neighbor = exp.cap(1);
                        rp = exp.cap(2);
                    }

                    exp.setPattern("peer (.+) route-filter (\\S+) import");
                    if ( line.contains(exp) )
                    {
                        //VRP
                        neighbor = exp.cap(1);
                        rp = exp.cap(2);
                    }

                    if ( !neighbor.isEmpty() && !rp.isEmpty() )
                    {
                        _lstPeNeighborRPIn.append(qry->ip()+"*"+neighbor+"*"+rp);
                        if ( !lstRoutePoliciesIxpIpt.contains(rp) && !lstNeighbors.contains(neighbor) )
                        {
                            qDebug() << "***" << qry->ip() << "neighbor" << neighbor <<
                                        "con RP aceptado, se agrega a validacion de prefijos aprendidos y de ASN";
                            lstNeighbors.append( neighbor );
                        }
                    }
                }
            }
            else if ( bgpipv4summary )
            {
                //summary de una sesion BGP IPv4

                QString vrf;
                for ( QString line : lines )
                {
                    line = line.simplified();

                    qDebug() << "Summary. IPs de bgp a agregar" << qry->ip() << line;

                    QRegExp exp("(vrf|vpn-instance) (.+) (ip|peer)");
                    if ( line.contains(exp) )
                        vrf = exp.cap(2);

                    exp.setPattern("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) +\\d+ +((\\d+|\\.)+) .+ +(\\d+)\\S*$");
                    if ( line.contains(exp) )
                        if ( lstNeighbors.contains(exp.cap(1)) && exp.cap(4).toInt() > 0 && exp.cap(4).toInt() < 500 )
                        {
                            bool negar = false;
                            for ( QString filtro : lstNeighborEnRedFiltroNegar )
                                if ( validarIPperteneceAsegmento(exp.cap(1),filtro) )
                                {
                                    negar = true;
                                    break;
                                }
                            if (!negar)
                            {
                                qDebug() << "IP de bgp a agregar a consulta" << qry->ip() << exp.cap(1) << exp.cap(2) << exp.cap(4);
                                ips.append( (!ips.isEmpty()?",":"")+exp.cap(1)+(!vrf.isEmpty()?"_"+vrf:"") ); //se agrega ese vecino a la consulta

                                mapIPASN.insert(qry->ip()+"_"+exp.cap(1),exp.cap(2));
                            }
                        }
                }
            }
            else if ( bgpipv6summary )
            {
                //summary de una sesion BGP IPv6

                QString summaryIPv6TXT;
                if ( qry->os() == "IOS XR" )
                    summaryIPv6TXT = filasIndentadasSubirAanterior(txt);
                else if ( qry->os() == "VRP" )
                    summaryIPv6TXT = txt;
                QString vrf;
                for ( QString line : summaryIPv6TXT.split("\n") )
                {
                    line = line.simplified();

                    QRegExp exp("(vrf|vpn-instance) (\\S+) (ip|peer)");
                    if ( line.contains(exp) )
                        vrf = exp.cap(2);

                    qDebug() << "Summary IPV6. IPs de bgp a agregar" << qry->ip() << line;

                    if ( line.contains("52468") )
                    {
                        qDebug() << "No se agrega vecino bgp a la consulta de redes" << line;
                        continue;
                    }

                    exp.setPattern("^(([a-f]|[A-F]|\\d+|\\:)+) +\\d+ +((\\d+|\\.)+) .+ +(\\d+)\\S*$");
                    if ( line.contains(exp) )
                    {
                        if ( lstNeighbors.contains(exp.cap(1)) && exp.cap(5).toInt() > 0 && exp.cap(5).toInt() < 300 )
                        {
                            qDebug() << "IP de bgp a agregar a consulta" << qry->ip() << exp.cap(1) << exp.cap(3) << exp.cap(5);
                            ips.append( (!ips.isEmpty()?",":"")+exp.cap(1)+(!vrf.isEmpty()?"_"+vrf:"") ); //se agrega ese vecino a la consulta

                            mapIPASN.insert(qry->ip()+"_"+exp.cap(1),exp.cap(3));
                        }
                    }
                }
            }
        }
        //  key: 172.16.30.253 value: 192.168.1.1,192.168.1.2_vrf,192.168.1.3
        mapPEsVecinosAConsultar.insert( qry->ip(),ips );
    }

    qc.clear();
    qt->deleteLater();

    //***********************  Entrando a todos los PEs origenes y se consultan las redes de los vecinos bgp

    qDebug() << "\n\n\n----- bgpNeighborASPathFromEquipos Consulta de vecinos BGP, se consultan las redes aprendidas";
    qDebug() << "Entrando a todos los PEs origenes y se consultan las redes de los vecinos bgp";

    QList<SBGPNeighborASPath> lstSalidaIPv4;
    QList<SBGPNeighborASPath> lstSalidaIPv6;

    qt = new QueriesThread;
    qt->setLstIP( lstIPs );
    qt->setConnectionProtocol( QRemoteShell::SSHTelnet );
    qt->setInterval( 1000 );
    qt->setSimultaneos( 3 );
    qt->setMaxParalelos( 40 );

    QMapIterator<QString,QString> imapPEsVecinosAConsultar(mapPEsVecinosAConsultar);
    //agregando la consulta de rutas por los vecinos de bgp
    while (imapPEsVecinosAConsultar.hasNext()) {
        imapPEsVecinosAConsultar.next();
        for ( QString neighbor : imapPEsVecinosAConsultar.value().split(",",QString::SkipEmptyParts) )
        {
            QString vrf;
            if ( neighbor.contains("_") )
            {
                QStringList data = neighbor.split("_",QString::SkipEmptyParts);
                vrf=data.at(1);
                neighbor=data.at(0);
            }
            if (neighbor.contains("."))
            {
                //IPV4
                if ( !vrf.isEmpty() )
                    qc.addQueryParameter({
                                             {"FuncionInfo_txt",
                                              "sh bgp vrf "+vrf+" ipv4 unicast neighbors "+
                                              neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
                                             {"FuncionInfo_txt",
                                              "display bgp vpnv4 vpn-instance "+vrf+" routing-table peer "+
                                              neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
                                         });
                else
                    qc.addQueryParameter({
                                             {"FuncionInfo_txt",
                                              "sh bgp ipv4 unicast neighbors "+
                                              neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
                                             {"FuncionInfo_txt",
                                              "display bgp routing-table peer "+
                                              neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
                                         });
            }
            else if (neighbor.contains(":"))
            {
                //IPV6
                if ( !vrf.isEmpty() )
                    qc.addQueryParameter({
                                             {"FuncionInfo_txt",
                                              "sh bgp vrf "+vrf+" ipv6 unicast neighbors "+
                                              neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
                                             {"FuncionInfo_txt",
                                              "display bgp vpnv6 vpn-instance "+vrf+" routing-table peer "+
                                              neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
                                         });
                else
                    qc.addQueryParameter({
                                             {"FuncionInfo_txt",
                                              "sh bgp ipv6 unicast neighbors "+
                                              neighbor+" routes",imapPEsVecinosAConsultar.key(),"IOS XR",""},
                                             {"FuncionInfo_txt",
                                              "display bgp ipv6 routing-table peer "+
                                              neighbor+" accepted-routes",imapPEsVecinosAConsultar.key(),"VRP",""},
                                         });
            }
        }

        for ( QString vrf : lstVRFs )
        {
            if ( vrf == "default" )
                vrf = "";

            if ( vrf.isEmpty() )
                //Global
                //agregando la consulta de network a distribuir por bgp Global
                qc.addQueryParameter({
                                         //IOS XR
                                         {"FuncionInfo_txt",
                                          "sh run router bgp "+ASN+" address-family ipv4 unicast",
                                          imapPEsVecinosAConsultar.key(),
                                          "IOS XR",""},
                                         {"FuncionInfo_txt",
                                          "sh run router bgp "+ASN+" address-family ipv6 unicast",
                                          imapPEsVecinosAConsultar.key(),
                                          "IOS XR",""},

                                         //VRP
                                         {"FuncionInfo_txt",
                                          "display current-configuration configuration bgp | i network|vpn-instance|#",
                                          imapPEsVecinosAConsultar.key(),
                                          "VRP",""},
                                         //No consultamos para IPv6 HUAWEI ya que la consulta es la misma para IPv4.
                                         //En el analisis lo separamos
                                     });
            else
                //VRF
                //agregando la consulta de network a distribuir por bgp VRF
                qc.addQueryParameter({
                                         //IOS XR
                                         {"FuncionInfo_txt",
                                          "sh run router bgp "+ASN+" vrf "+vrf+" address-family ipv4 unicast",
                                          imapPEsVecinosAConsultar.key(),
                                          "IOS XR",""},
                                         {"FuncionInfo_txt",
                                          "sh run router bgp "+ASN+" vrf "+vrf+" address-family ipv6 unicast",
                                          imapPEsVecinosAConsultar.key(),
                                          "IOS XR",""},

                                         //VRP
                                         {"FuncionInfo_txt",
                                          "display current-configuration configuration bgp | i network|vpn-instance|#",
                                          imapPEsVecinosAConsultar.key(),
                                          "VRP",""},
                                         //No consultamos para IPv6 HUAWEI ya que la consulta es la misma para IPv4.
                                         //En el analisis lo separamos
                                     });
        }
    }
    qt->setQueriesConfiguration( qc );

    qt->setOptions({QueryOpcion::Connect, QueryOpcion::Platform, QueryOpcion::Funcion, QueryOpcion::Exit });
    qt->iniciarSync();

    //agregando

    for ( Queries *qry : qt->lstQueriesPointers() )
    {
        int posInicioASPath=63;
        QString lastIPv6;

        for ( QString txt : qry->funcionQuery->lstTxtReceived() )
        {
            QStringList lines = txt.split("\n",QString::SkipEmptyParts);
            if ( lines.isEmpty() )
                continue;
            QString line1 = lines.at(0);

            qDebug() << "line1" << line1;

            QString vrf;

            QRegExp expvrf("(vrf|vpn-instance) (\\S+) (ip|routing|address)");
            if ( line1.contains(expvrf) )
                vrf = expvrf.cap(2);

            bool neighboripv6=false;
            bool neighboripv4=false;
            bool network=false;

            if ( line1.contains("ipv4 unicast neighbors") ||
                 line1.contains("bgp routing-table peer") ||
                 line1.contains(QRegExp("vpnv4 vpn-instance .+ routing-table peer"))
                 )
                neighboripv4=true;
            else if ( line1.contains("ipv6 unicast neighbors") ||
                      line1.contains("bgp ipv6 routing-table peer") ||
                      line1.contains(QRegExp("vpnv6 vpn-instance .+ routing-table peer"))
                      )
                neighboripv6=true;
            else if ( line1.contains("sh run router bgp") ||
                      line1.contains("display current-configuration configuration bgp ") )
                network=true;

            qDebug() << "Tipo" << neighboripv6 << neighboripv4 << network;


            if ( neighboripv4 )
            {
                //IPV4

                for ( QString line : lines )
                {
                    if (line.contains("Network") && line.contains("Metric") && line.contains("Weight") && line.contains("Path"))
                    {
                        //IOS XR
                        posInicioASPath=line.indexOf("Path");
                        qDebug() << "Nuevo posInicioASPath" << posInicioASPath;
                        continue;
                    }
                    if (line.contains("Network") && line.contains("MED") && line.contains("LocPrf") && line.contains("Path"))
                    {
                        //VRP
                        posInicioASPath=line.indexOf("Path")+1;
                        qDebug() << "Nuevo posInicioASPath" << posInicioASPath;
                        continue;
                    }

                    QRegExp exp(" *(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}/\\d+) +(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}) ");
                    if ( line.contains(exp) )
                    {
                        if ( exp.cap(1) == "0.0.0.0/0" )
                            continue;

                        qDebug() << "Por vecino:" << qry->ip() << "agregando red y su verdadero origen" << exp.cap(1) << qry->ip()
                                 << "desde" << exp.cap(2) << posInicioASPath;

                        if ( exp.cap(1).split("/").at(1).toInt() > 24 && !agregarRedesMayoresMascara24 )
                            continue;

                        QString IPASN = mapIPASN.value( qry->ip()+"_"+exp.cap(2) );
                        if ( IPASN.isEmpty() ) IPASN=ASN;

                        //buscamos el Route Policy del neighbor
                        QString rp;
                        for ( QString PeNeighborRPIn : _lstPeNeighborRPIn )
                        {
                            QStringList data = PeNeighborRPIn.split("*");
                            if ( data.size() != 3 )
                                continue;

                            if ( data.at(0) == qry->ip() && data.at(1) == exp.cap(2) )
                            {
                                rp =  data.at(2);
                                break;
                            }
                        }

                        QString aspath = IPASN+" "+line.mid(posInicioASPath);
                        aspath.replace(" i","");
                        aspath.replace(" ?","");

                        SBGPNeighborASPath i;
                        i.peip = qry->ip();
                        i.peerip = exp.cap(2);
                        i.peerasn = IPASN;
                        i.red = exp.cap(1);
                        i.asnorigen = aspath.split(" ",QString::SkipEmptyParts).last().simplified();
                        i.aspath = aspath.simplified();
                        i.routepolicy = rp;
                        i.vrf = vrf;

                        lstSalidaIPv4.append(i);
                    }
                }
            }
            else if ( neighboripv6 )
            {
                //IPV6

                QList<QStringList> lstCampos;
                if ( qry->os() == "IOS XR" )
                {
                    //IOS XR
                    QMap<int,QRegExp> map;
                    map.insert( 3,QRegExp("([0-9a-f]{1,4}:\\S+/\\d{1,3})"));
                    map.insert(22,QRegExp("([0-9a-f]{1,4}:\\S+)"));
                    map.insert(63,QRegExp(".+$"));

                    lstCampos = tableMultiRow2Table(txt,map);
                }
                else if ( qry->os() == "VRP" )
                {
                    //VRP
                    QString lastNetwork;
                    QString lastPrefix;
                    QString lastNexthop;
                    for ( QString line : txt )
                    {
                        line = line.simplified();
                        QRegExp exp("Network : (([a-f]|[a-f]|\\d+|\\:)+) +PrefixLen (\\d+)$");
                        if ( line.contains(exp) )
                        {
                            lastNetwork = exp.cap(1);
                            lastPrefix = exp.cap(3);
                            continue;
                        }
                        exp.setPattern("");
                        if ( "NextHop : (([a-f]|[a-f]|\\d+|\\:)+) +" )
                        {
                            lastNexthop = exp.cap(1);
                            continue;
                        }
                        exp.setPattern("Path/Ogn : (.+)");
                        {
                            lstCampos.append( QStringList() << lastNetwork+"/"+lastPrefix << lastNexthop << exp.cap(1) );
                            lastNetwork.clear();
                            lastPrefix.clear();
                            lastNexthop.clear();
                        }
                    }
                }

                for ( QStringList lst : lstCampos )
                {
                    if ( lst.size() != 3 )
                        continue;

                    QString red = lst.at(0);
                    QString peer = lst.at(1);
                    QString aspath = lst.at(2);
                    aspath.replace(" i","");
                    aspath.replace(" ?","");

                    if ( red.split("/").at(1).toInt() > 48 )
                        continue;

                    QString IPASN = mapIPASN.value( qry->ip()+"_"+peer );

                    //buscamos el Route Policy del neighbor
                    QString rp;
                    for ( QString PeNeighborRPIn : _lstPeNeighborRPIn )
                    {
                        QStringList data = PeNeighborRPIn.split("*");
                        if ( data.size() != 3 )
                            continue;

                        if ( data.at(0) == qry->ip() && data.at(1) == peer )
                        {
                            rp =  data.at(2);
                            break;
                        }
                    }

                    SBGPNeighborASPath i;
                    i.peip = qry->ip();
                    i.peerip = peer;
                    i.peerasn = IPASN;
                    i.red = red;
                    i.asnorigen = aspath.split(" ",QString::SkipEmptyParts).last().simplified();
                    i.aspath = ASN+" "+aspath.simplified();
                    i.routepolicy = rp;
                    i.vrf = vrf;

                    lstSalidaIPv6.append(i);
                }
            }
            else if ( network )
            {
                //NETWORK

                qDebug() << "Network" << qry->os() << qry->ip() << qry->hostName();

                QString lastVpnInstance; //Para Huawei
                for ( QString line : lines )
                {
                    line = line.simplified();
                    qDebug() << "line" << line;

                    QRegExp exp;
                    QRegExp exp2;

                    if ( qry->os() == "IOS XR" )
                    {
                        //IOS XR
                        exp.setPattern("network (.+) route-(policy|filter) (.+)");
                        exp2.setPattern("network (.+)$");

                        QString red;
                        QString policy;

                        if ( line.contains(exp) )
                        {
                            red = exp.cap(1);
                            policy = exp.cap(3);
                        }
                        else if ( line.contains(exp2) )
                            red = exp2.cap(1);

                        if ( !red.isEmpty() )
                        {
                            if ( red.contains(".") )
                            {
                                if ( red.split("/").at(1).toInt() > 24 )
                                    continue;

                                if ( isRedPrivada(red) )
                                    continue;

                                SBGPNeighborASPath i;
                                i.peip = qry->ip();
                                i.peerasn = ASN;
                                i.red = red;
                                i.asnorigen = ASN;
                                i.routepolicy = policy;
                                i.vrf = vrf;

                                lstSalidaIPv4.append(i);
                            }
                            else if ( red.contains(":") )
                            {
                                if ( red.split("/").at(1).toInt() > 48 )
                                    continue;

                                SBGPNeighborASPath i;
                                i.peip = qry->ip();
                                i.peerasn = ASN;
                                i.red = red;
                                i.asnorigen = ASN;
                                i.routepolicy = policy;
                                i.vrf = vrf;

                                lstSalidaIPv6.append(i);
                            }
                        }
                    }
                    else if ( qry->os() == "VRP" )
                    {
                        //VRP

                        //Agregar red que no tiene policy

                        exp.setPattern("ipv\\d-family vpn-instance (.+)$");
                        if ( line.contains(exp) )
                        {
                            lastVpnInstance = exp.cap(1);
                            qDebug() << "last vpn instance" << lastVpnInstance;
                            continue;
                        }

                        if ( line == "#" )
                        {
                            lastVpnInstance.clear();
                            qDebug() << "limpiando vrf";
                            continue;
                        }

                        if ( lastVpnInstance!= "INTERNET" )
                            continue;

                        exp.setPattern("network (.+) (.+) route-(policy|filter) (.+)");
                        if ( line.contains(exp) )
                        {
                            qDebug() << "Por network:" << qry->ip() << "agregando red y su verdadero origen" << exp.cap(1) << qry->ip();

                            QString red = exp.cap(1);
                            QString mascara = exp.cap(2);
                            if ( red.contains(".") )
                            {
                                mascara = convertirMascaraOctetosAdos(mascara);
                                qDebug() << "mascara" << mascara;
                                if ( mascara.toInt() > 24 )
                                    continue;

                                SBGPNeighborASPath i;
                                i.peip = qry->ip();
                                i.peerasn = ASN;
                                i.red = red+"/"+mascara;
                                i.asnorigen = ASN;
                                i.routepolicy = exp.cap(4);
                                i.vrf = vrf;

                                lstSalidaIPv4.append(i);
                            }
                            else if ( red.contains(":") )
                            {
                                qDebug() << "mascara" << mascara;
                                if ( mascara.toInt() > 48 )
                                    continue;

                                SBGPNeighborASPath i;
                                i.peip = qry->ip();
                                i.peerasn = ASN;
                                i.red = red+"/"+mascara;
                                i.asnorigen = ASN;
                                i.routepolicy = exp.cap(4);
                                i.vrf = vrf;

                                lstSalidaIPv6.append(i);
                            }
                        }
                    }
                }
            }
        }
    }

    qc.clear();
    qt->deleteLater();

    QList<SBGPNeighborASPath> lstSalida;

    lstSalida.append( lstSalidaIPv4 );
    lstSalida.append( lstSalidaIPv6 );

    qDebug() << "\n\n\n----- bgpNeighborASPathFromEquipos FINALIZADO";

    return lstSalida;
}

QString buscarServicioID(QString txt)
{
    QRegExp exp("([A-Z]|&){7}\\d{8}(I|C)");
    if ( txt.contains(exp) )
        return exp.cap(0);

    exp.setPattern("[A-Z]{6}\\,\\d{8}(I|C)");
    if ( txt.contains(exp) )
        return exp.cap(0);

    exp.setPattern("[A-Z]{4}(\\d{8}|\\d{7})");
    if ( txt.contains(exp) )
        return exp.cap(0);

    exp.setPattern("[A-Z]{2}(\\d{8}|\\d{7})");
    if ( txt.contains(exp) )
        return exp.cap(0);

    exp.setPattern("IDU:(\\S+)-(CLI|DE)");
    if ( txt.contains(exp) )
        return exp.cap(0);

    return "";
}
