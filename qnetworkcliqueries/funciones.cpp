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
#include <QTimer>
#include <QRegularExpression>

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
    QRegularExpression expNCS("NCS\\-*[5-6]\\d{2,}");
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
              platform.contains("ASR1002") ||
              platform.contains("WS-C4948") ||
              platform.contains("C1111")
             )
        return "IOS";
    else if ( platform.contains("N5K") ||
              platform.contains("N7K") ||
              platform.contains("Nexus")
             )
        return "IOS NX";
    else if ( platform.contains("ATN") ||
              platform.contains("NE40") ||
              platform.contains("NE9000") ||
              platform.contains("NetEngine 8000") ||
              platform.contains("S6730") ||
              platform.contains("S5735") ||
              platform.contains("S5731")
             )
        return "VRP";
    else if ( platform.contains("ISCOM2948")
             )
        return "ROS";
    else if ( platform.contains("MikroTik")
             )
        return "RouterOS";
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
    QStringList lstmie = openFile( path ).split("\n",Qt::SkipEmptyParts);
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

    QRegularExpression exp2("RP.+CPU\\d\\:");
    if ( name.contains(exp2) )
        name.replace(exp2,"");

    return name;
}

QStringList numberRangeToLST(QString str)
{
    QStringList lst = str.split(",",Qt::SkipEmptyParts);

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
    // bool nom=false;
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
            // nom=true;

            if ( validateIpQuery( qry,ip ) )
                return &qry;
        }
    }

//    if ( !nom )
//        qDebug() << "buscarEquipos: no se encontro nombre del equipo en lst" << nombre <<
//                    "no se se empezo a validar IP" << ip;

    return nullptr;
}

Queries* getQuery(QList<Queries*> lst, QString ip)
{
    for ( Queries *q : lst )
        if ( q->ip() == ip )
            return q;
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

    QRegularExpression exp,exp2;

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

    QRegularExpression exp;
    QRegularExpressionMatch match;
    //Si es un Po lo convertimos a una de sus interfaces, esto para poder averiguar si tiene vecinos
    exp.setPattern("(Po|BE)([0-9]+)");
    if ( i.contains(exp,&match) )
    {
        QString group = match.captured(2);
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

    QRegularExpression exp( interfazPO1+"\\.\\d+" ); //para verificar si existe una sesion ospf en un subinterfa, si si, regresamos true
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
    QRegularExpression exp;
    QRegularExpressionMatch match;
    exp.setPattern( "(Po|Bundle\\-Ether|BE)(\\d+)$" );
    if ( pcInterface.contains(exp,&match) )
        group = match.captured(2);

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
        ip = ip.split("/",Qt::SkipEmptyParts).first();

    QString salida;
    if ( ip.contains(".") )
    {
        //IPv4
        QStringList lst = ip.split(".");
        foreach (QString bb, lst)
        {
            bool ok;
            salida.append( QString("%1").arg(bb.toULongLong(&ok, 10), 8, 2, QChar('0')) );
        }
    }
    else if ( ip.contains(":") )
    {
        //IPv6
        if ( !ip.contains("::") )
        {
            QStringList lst = ip.split(":");
            foreach (QString bb, lst)
            {
                bool ok;
                salida.append( QString("%1").arg(bb.toULongLong(&ok, 16), 16, 2, QChar('0')) );
            }
        }
        else
        {
            QStringList data = ip.split("::",Qt::SkipEmptyParts);
            if ( data.size() == 1 )
            {
                //:: suponemos que esta al final
                QStringList lst = data.first().split(":");
                QString ii;
                bool ok;
                foreach (QString bb, lst)
                    ii.append( QString("%1").arg(bb.toULongLong(&ok, 16), 16, 2, QChar('0')) );

                salida.append( QString("%1").arg(ii.toULongLong(&ok, 2), -128, 2, QChar('0')) );
            }
            else if ( data.size() == 2 )
            {
                //:: esta en medio
                QString primero,segundo;
                for ( int c=0; c<data.size(); c++ )
                {
                    QString aa = data.at(c);
                    QStringList lst = aa.split(":");
                    foreach (QString bb, lst)
                    {
                        bool ok;
                        QString cc = QString("%1").arg(bb.toULongLong(&ok, 16), 16, 2, QChar('0'));
                        if ( c==0 )
                            primero.append( cc );
                        if ( c==1 )
                            segundo.append( cc );
                    }
                }
                int diferencia=128-primero.size()-segundo.size();
                salida.append( primero + QString(diferencia,QChar('0'))+ segundo );
            }
        }
    }

//    qDebug() << "convirtiendo IP a binario" << ip << salida;

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

QString decimal2IP(QString dec)
{
    bool ok;
    QString bin = QString("%1").arg(dec.toULongLong(&ok, 10), 32, 2, QChar('0'));
    return binario2IP( bin );
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
    QString segmento;
    short int mascara;

    QRegularExpression exp(QRegularExpression::anchoredPattern("(\\S+)/(\\d{1,3})"));
    QRegularExpressionMatch match;
    if ( segmentoIP_mascara2digitos.contains(exp,&match) )
    {
        segmento = match.captured( 1 );
        mascara = match.captured( 2 ).toShort();
    }
    else
        return false;

    if ( IP2Binario(IP).left(mascara) == IP2Binario(segmento).left(mascara) )
    {
//        qDebug() << "validarIPperteneceAsegmento" << IP <<  segmentoIP_mascara2digitos << "true";
        return true;
    }

//    qDebug() << "validarIPperteneceAsegmento" << IP <<  segmentoIP_mascara2digitos << "false";
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
        if ( caracter < 32 && caracter != 13 && caracter != 10 && caracter != 9 )
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
    QRegularExpression exp(grupo+"_"+filelabel+"_*Queries_"+dir.dirName()+"_(\\d{4}-\\d{2}-\\d{2}-\\d{2}-\\d{2}-\\d{2})\\.dat");
    QRegularExpressionMatch match;
    while (dirIter.hasNext())
    {
        QString filepath = dirIter.next();

        if ( filepath.contains(exp,&match) )
        {
            QDateTime dateTime = QDateTime::fromString( match.captured(1),"yyyy-MM-dd-hh-mm-ss" );
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
    interfaz.replace("Virtual-Ethernet","VE");
    interfaz.replace("Ethernet","Eth");
    interfaz.replace("Vlanif","Vlan");
    interfaz.replace("VLAN","Vlan");
    interfaz.replace("BVI","BV");
    interfaz.replace("BDI","BD");
    interfaz.replace("Po0/0/","Po");
    interfaz.replace("GE","Gi",Qt::CaseSensitive);
    interfaz.replace("gigaethernet","Gi");
    interfaz.replace(QRegularExpression("\\(\\w+\\)"),"");

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
    QRegularExpression expIP("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
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
    QRegularExpression exp("/\\d+");
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
                                       QMap<int,QRegularExpression> &mapPosicionesExpCampos)
{
    QList<QStringList> lstSalida;
    if ( mapPosicionesExpCampos.isEmpty() )
        return lstSalida;

    qDebug() << "\n\n\n tableMultiRow2Table ************************************* ";

    QStringList *lstFilaActual=nullptr;
    QMapIterator<int,QRegularExpression> imap(mapPosicionesExpCampos);
    uint c=0;
    for ( QString line : txt.split("\n",Qt::SkipEmptyParts) )
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
            QRegularExpression exp = imap.value();
            d++;

//            qDebug() << "c" << c << "d" << d << "pos" << pos << "exp" << exp.pattern() << "imapOnFirst" << imapOnFirst;

            if ( pos >= line.size() )
            {
//                qDebug() << "no queda info pos:" << pos << "line.size" << line.size();
                break;
            }

            QRegularExpressionMatch match = exp.match(line,pos);
            int posmatch = match.capturedStart(0);

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

//                qDebug() << "agregando campo" << match.captured(0);
                lstFilaActual->append( match.captured(0).simplified() );
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

    QRegularExpression exp("^ +");
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

QString buscarServicioID(QString txt)
{
    QRegularExpression exp("([A-Z]|&){7}\\d{8}(I|C)");
    QRegularExpressionMatch match;
    if ( txt.contains(exp,&match) )
        return match.captured(0);

    exp.setPattern("[A-Z]{6}\\,\\d{8}(I|C)");
    if ( txt.contains(exp,&match) )
        return match.captured(0);

    exp.setPattern("[A-Z]{4}(\\d{8}|\\d{7})");
    if ( txt.contains(exp,&match) )
        return match.captured(0);

    exp.setPattern("[A-Z]{2}(\\d{8}|\\d{7})");
    if ( txt.contains(exp,&match) )
        return match.captured(0);

    exp.setPattern("IDU:(\\S+)-(CLI|DE)");
    if ( txt.contains(exp,&match) )
        return match.captured(0);

    return "";
}

QStringList txt2Columns(QString txt, QList<short> lstIndex)
{
    QStringList lst;
    for ( short c=0; c<lstIndex.size() ; c++ )
    {
        short a = lstIndex.at(c);
        short b;

        if ( lstIndex.size()-1 >= c+1 )
            b=lstIndex.at(c+1);
        else
            b=txt.size()-1;

//        qDebug() << "txt size" << txt.size() << "c" << c << "a" << a << "b" << b;

        QString column = txt.mid( a,b-a).simplified();
        if ( !column.isEmpty() )
            lst.append( column );
    }
    return lst;
}

short numbertxt2short(QString numbertxt)
{
    QRegularExpression exp("\\d+");
    QRegularExpressionMatch match;
    if ( numbertxt.contains(exp,&match) )
        return match.captured(0).toShort();
    return 0;
}

int numbertxt2int(QString numbertxt)
{
    QRegularExpression exp("\\d+");
    QRegularExpressionMatch match;
    if ( numbertxt.contains(exp,&match) )
        return match.captured(0).toInt();
    return 0;
}

QStringList getRegExpAllMatches(QRegularExpression rx, QString txt)
{
    QStringList list;

    QRegularExpressionMatchIterator i = rx.globalMatch(txt);
    while ( i.hasNext() )
    {
        QRegularExpressionMatch match = i.next();
        list << match.captured(1);
    }

    return list;
}

QString numberExp10Anumber(QString txt)
{
    QString salida;
    QRegularExpression exp("(e|E)+(\\d+)$");
    QRegularExpressionMatch match;

    if ( txt.contains(exp,&match) )
    {
        int e=match.captured(2).toInt();
        txt.replace(exp,QString(e,'0'));
        bool encontrado = false;
        int d=0;
        for ( int c=0; c<txt.length(); c++ )
        {
            QChar qchar = txt.at(c);
            if ( encontrado )
            {
                d++;
                if ( d == e )
                {
                    salida.append(qchar);
                    return salida;
                }
            }

            if ( qchar != '.' )
                salida.append( qchar );
            else
                encontrado=true;
        }
        return salida;
    }
    else
        return txt.left( txt.indexOf(".") );
}

bool prefixSetAllowsRange32_128(QString &pfxtxt)
{
    QRegularExpression exple("le (\\d+)");
    QRegularExpression expge("ge (\\d+)");
    QRegularExpressionMatch match;
    for ( QString &line : pfxtxt.split("\n",Qt::SkipEmptyParts) )
    {
        if ( line.contains(QRegularExpression("0.0.0.0/0 (le|ge)")) )
            return true;

        //TODO
        continue;

        if ( !line.contains(" le ") && !line.contains(" ge ") )
            //no tiene para rango. segmento especifico continuamos
            continue;

        int ge=0;
        int le=0;
        int gmask=0; //mayor mascara /32 /128
        if ( line.contains(":") )
            le=128;
        else
            le=32;
        gmask=le;
        if ( line.contains(expge,&match) )
            ge=match.captured(1).toInt();
        if ( line.contains(exple,&match) )
            le=match.captured(1).toInt();

        // line.replace(exple,"");
        // line.replace(expge,"");
        // if ( !line.contains(" ") && !line.contains("/") )
        //     //es un cisco q no tiene mascara, host
        //     continue;

        // line.replace(" ","/"); //el espacio que queda es la separacion de la ip y masca en VRP
        // int mask=line.split("/",Qt::SkipEmptyParts).last().toInt();

        // if ( mask == le || mask == ge )
        //     //mascara igual a le
        //     continue;

        // if ( mask >= ge && mask <= le )
        //     return true;

        if ( ge <= gmask && gmask <= le )
            return true;
    }
    return false;
}
