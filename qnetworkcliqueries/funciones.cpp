#include "funciones.h"
#include "queries.h"
#include "bdhosts.h"
#include "equipmentneighbors.h"
#include "interfaceinfo.h"
#include "portchannelsinfo.h"
#include "arpinfo.h"

QStringList lstPaises()
{
    QStringList lstPaises;
    lstPaises.append("GT");
    lstPaises.append("SV");
    lstPaises.append("HN");
    lstPaises.append("NIC");
    lstPaises.append("CR");
    return lstPaises;
}

QString equipmentOSFromPlatform(QString platform)
{
    if ( platform.contains("ASR9K") ||
         platform.contains("CRS") ||
         platform.contains("NCS-6000") )
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

void saveFile(QString txt, QString path,QIODevice::OpenModeFlag openmode)
{
    QFile file(path);
    if (!file.open( openmode | QIODevice::Text ))
        return;

    QTextStream out(&file);
    out << txt;
    file.close();
}

LstQueries *openLstQueries(QString path)
{
    LstQueries *lst;
    QFile file( path );
    file.open(QIODevice::ReadOnly);
    QDataStream storeStream(&file);
    storeStream >> lst;
    file.close();
    return lst;
}

void saveLstQueries(LstQueries* lst, QString path)
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
//    QRegExp exp("\\.(telgua|enitel|claro|sercom|telecom)+\\.*\\w*");
//    exp.setCaseSensitivity( Qt::CaseInsensitive );
//    int pos = exp.indexIn(name);

    int pos = name.indexOf(".");

    if ( pos > 0 )
        name = name.left( pos );

    QRegExp exp2("RP(_|/)0.+(_|:)");
    if ( name.contains(exp2) )
        name.replace(exp2,"");

    return name;
}

QString interfazSimplifier(QString interfaz)
{
    interfaz.replace("nGigabitEthernet","");
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

Queries *buscarEquipoPorIPnombrePais(QList<Queries*> lst, QString ip,QString nombre,QString pais)
{
    bool nom=false;
    foreach (Queries *qry, lst)
    {
        //tiene que coincidir el nombre. La ip la validamos con el nombre con la funcion. Se hace esto
        //ya que puede ser que la IP que reporto EquipmentNeighbors sea diferente a la ip con la que se hace el backup
        //caso como equipos L3

        if ( qry->hostName() == nombre )
        {
            nom=true;

            if ( validateIpQuery( qry,ip,pais ) )
                return qry;
            else if ( validateIpHost( ip, nombre,pais ) ) //el query no tiene informacion de las ips, validamos en el backup
                return qry;
        }
    }

    if ( !nom )
        qDebug() << "buscarEquipos: no se encontro nombre del equipo en lst" << nombre <<
                    "no se se empezo a validar IP" << ip;

    return nullptr;
}

Queries *buscarEquipoPorIP(QList<Queries*> lst, QString ip, QString pais)
{
    foreach (Queries *qry, lst)
    {
        if ( validateIpQuery( qry,ip,pais ) )
            return qry;
    }
    return nullptr;
}

Queries *buscarEquipoPorNombre(QList<Queries*> lst, QString nombre)
{
    foreach (Queries *qry, lst)
    {
        if ( qry->hostName() == nombre )
            return qry;
    }
    return nullptr;
}

bool validateIpQuery(Queries *qry,QString ip,QString pais)
{
    //validamos primero la ip, si la enviada concide exactamente se devuleve
    if ( qry->ip() == ip )
        return true;
    else if ( qry->interfacesIpAddressesQuery )
    {
        //el query tiene informacion de las ips del equipo. Validamos la IP
        foreach (SIpInfo *i, qry->interfacesIpAddressesInfo())
        {
            if ( i->ip == ip )
                return true;
        }

        //TODO dependiendo del contexto de los Nexus puede o no salir la IP con show ip int brief
        //pero no quiere decir que no sea esa ip de ese equipo. Hay que trabajar los Nexus de diferente forma.
    }

    return false;
}

bool validateIpHost(QString ip, QString name, QString country)
{
    //Se verifica si existe el equipo en el listado y si coincide la IP
    //Si el equipo no se encuentra se regresa true ya que no es posible validarlo, esto para que se pueda graficar

    bool nombreEncontrado=false;
    foreach ( Host *host, BDHosts::instance()->hostsAllInterfacesIPs( country ))
    {
        if ( host->nombre == name )
        {
            nombreEncontrado=true;
            if ( host->ip == ip ||
                 ip.isEmpty() )
                return true;
        }
    }

    return !nombreEncontrado;
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

QString ipToCountry(QString ip)
{
    QStringList lstPaises;
    lstPaises.append("GT");
    lstPaises.append("SV");
    lstPaises.append("HN");
    lstPaises.append("NIC");
    lstPaises.append("CR");

    foreach (QString pais, lstPaises)
    {
        foreach (Host *host, BDHosts::instance()->hostsAllInterfacesIPs(pais))
        {
            if ( host->ip == ip &&
                 !host->interfaceIsShutdown )
                return host->pais;
        }
    }
    return "";
}

bool equipmentNeighborsVecinoValidarEquipo(SEquipmentNeighborsInfo *e)
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

    if ( !e->nombreequipo.contains(exp) &&
         !e->nombreequipo.contains(exp2) )
    {
        if ( !platformValidate(e->plataforma) )
            return false;
    }

    return true;
}

QString interfaceDescription(QList<SInterfaceInfo*> lstInterfaceDescriptions,QString interfaz)
{       
    qDebug() << "interfaceDescription" << lstInterfaceDescriptions.size() << interfaz;

    foreach ( SInterfaceInfo* id, lstInterfaceDescriptions )
    {
        qDebug() << "2222" << id->interfaz << interfaz;

        if ( id->interfaz == interfaz )
            return id->description;
    }
    return "";
}

QList<SEquipmentNeighborsInfo*> interfaceHasEquipmentNeighborsInfo(QList<SEquipmentNeighborsInfo *> lstEquipmentNeighborsInfo, QList<SPortChannel *> lstPCInfo, QString i, QString pais)
{
    QStringList lstVecinos;
    QList<SEquipmentNeighborsInfo*> lstSalida;

    QRegExp exp;
    //Si es un Po lo convertimos a una de sus interfaces, esto para poder averiguar si tiene vecinos
    exp.setPattern("(Po|BE)([0-9]+)");
    if ( i.contains(exp) )
    {
        QString group = exp.cap(2);
        foreach (SPortChannel *pc, lstPCInfo)
        {
            if ( group == pc->group )
            {
                i = pc->interfaz;
                break;
            }
        }
    }

    foreach (SEquipmentNeighborsInfo *equipo, lstEquipmentNeighborsInfo)
    {
        if ( equipo->interfazestesalida != i || !equipo->operativo  )
            continue;

        QString vecinonombre = simplicateName(equipo->nombreequipo);

        //verificamos si ya se habia agregado un equipo con el mismo nombre. Caso de que se aprenda dos veces el mismo equipo
        //por la misma interfaz
        if ( lstVecinos.contains( vecinonombre ) )
            continue;
        else
            lstVecinos.append( vecinonombre );

        //si se establecio el pais se verifica que el vecino pertenezca al mismo para saltarse a equipos de otro.
        if ( !pais.isEmpty() )
        {
            QString vecinopais = BDHosts::instance()->hostCountry( vecinonombre );
            if (  vecinopais != pais && !vecinopais.isEmpty() )
                continue;
        }

        if ( equipmentNeighborsVecinoValidarEquipo(equipo) )
            lstSalida.append(equipo);
    }
    return lstSalida;
}

QList<SMacInfo*> interfaceLearnedManagementVlans(QList<SMacInfo*> lst, QString interfaz,QString codigopais)
{
    QList<SMacInfo*> lstsalida;

    foreach (SMacInfo *m, lst)
    {
        if ( codigopais == "GT" )
        {
//            m->vlan == "3579" huawei
            if ( m->interfaz == interfaz &&
                 m->vlan == "189"
                 )
                lstsalida.append( m );
        }
        else if ( codigopais == "SV" )
        {
            if ( m->interfaz == interfaz &&
                 ( m->vlan == "161" ||
                   m->vlan == "116" ||
                   (m->vlan >= "3500" &&
                   m->vlan <= "3699" ))
                 )
                lstsalida.append( m );
        }
        else if ( codigopais == "HN" )
        {
            if ( m->interfaz == interfaz &&
                 ( m->vlan == "120" ||
                   m->vlan == "126" ||
                   m->vlan == "125" ||
                   m->vlan == "121" ||
                   m->vlan == "122" ||
                   m->vlan == "123" ||
                   m->vlan == "124" ||
                   m->vlan == "110" )
                 )
                lstsalida.append( m );
        }
        else if ( codigopais == "NIC" )
        {
            if ( m->interfaz == interfaz && m->vlan == "173" )
                lstsalida.append( m );
        }
        else if ( codigopais == "CR" )
        {
            if ( m->interfaz == interfaz && m->vlan == "189" )
                lstsalida.append( m );
        }
    }
    return lstsalida;
}

bool interfaceIsL2(Queries* q, QString interfaz)
{
    //Si la interfaz pertenece a un Po se busca el mismo para poder averiguar si es L2 o L3
    //Se usa "interfaz" para averiguar la informacion de vlans u ospf ya sea el Po o la interfaz original
    //Al final se gráfica la interfaz
    QString interfazPO = interfaceToPortChannelInterface( q->portChannelInfo(),
                                                        interfaz,
                                                        q->platform() );

    qDebug() << "verificando si la interfaz es L2" << q->hostName() << q->ip() << interfazPO;

    foreach (SInterfaceVlans *ipv, q->interfacesPermitedVlansInfo())
    {
        if ( ipv->interfaz == interfazPO )
        {
            int vlans = ipv->vlans.size();

            qDebug() << "vlans permitidas" << vlans;

            if ( vlans > 0 )
                return true;
            else
                return false;
        }
    }
    return false;
}

bool interfacesAreL2OneVlanMatches(Queries *q1, QString interfaz1, Queries *q2, QString interfaz2)
{
    //Si la interfaz pertenece a un Po se busca el mismo para poder averiguar si es L2 o L3
    //Se usa "interfaz" para averiguar la informacion de vlans u ospf ya sea el Po o la interfaz original
    //Al final se gráfica la interfaz
    QString interfazPO1 = interfaceToPortChannelInterface( q1->portChannelInfo(),
                                                        interfaz1,
                                                        q1->platform() );

    QString interfazPO2 = interfaceToPortChannelInterface( q2->portChannelInfo(),
                                                        interfaz2,
                                                        q2->platform() );

    qDebug() << "verificando si las dos interfaces son L2 y una vlan tienen al menos en comun" <<
                q1->hostName() << interfazPO1 << q2->hostName() << interfazPO2;

    //por lo menos una vlan tiene que se ser comun en ambos
    foreach (SInterfaceVlans *ipv1, q1->interfacesPermitedVlansInfo())
    {
        if ( ipv1->interfaz == interfazPO1 )
        {
            foreach (SInterfaceVlans *ipv2, q2->interfacesPermitedVlansInfo())
            {
                if ( ipv2->interfaz == interfazPO2 )
                {
                    foreach (QString vlan, ipv1->vlans)
                    {
                        if ( ipv2->vlans.contains( vlan ) )
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

bool interfaceIsL3(Queries *q, QString interfaz)
{
    //Si la interfaz pertenece a un Po se busca el mismo para poder averiguar si es L2 o L3
    //Se usa "interfaz" para averiguar la informacion de vlans u ospf ya sea el Po o la interfaz original
    //Al final se gráfica la interfaz
    QString interfazPO1 = interfaceToPortChannelInterface( q->portChannelInfo(),
                                                        interfaz,
                                                        q->platform() );

    QRegExp exp( interfazPO1+"\\.\\d+" ); //para verificar si existe una sesion ospf en un subinterfa, si si, regresamos true
    foreach (SOSPFInfo *oi1, q->ospfInfo())
    {
        if ( oi1->interfaz == interfazPO1 ||
             oi1->interfaz.contains( exp ) )
            return true;
    }
    return false;
}

bool interfaceIsInVRF(Queries *q, QString interfaz)
{
    QString interfazPO1 = interfaceToPortChannelInterface( q->portChannelInfo(),
                                                        interfaz,
                                                        q->platform() );

    foreach (SVrfInfo *vi, q->vrfsInfo())
    {
        if ( vi->interfaz == interfazPO1 &&
             !vi->vrf.isEmpty() )
            return true;
    }
    return false;
}

QString interfaceOspfArea(Queries *q, QString interfaz)
{
    QString interfazPO1 = interfaceToPortChannelInterface( q->portChannelInfo(),
                                                        interfaz,
                                                        q->platform() );

    foreach (SOSPFInfo *io, q->ospfInfo())
    {
        if ( io->interfaz == interfazPO1 )
            return io->area;
    }
    return "";
}

QString interfaceToPortChannelInterface(QList<SPortChannel *> lst, QString interface, QString plataforma)
{
    //regresa la Po interfaz a la que pertenece la interfaz.
    //Si no pertenece a alguno PO se regresa la misma interfaz enviada.
    foreach (SPortChannel *pc, lst)
    {
        if ( pc->interfaz == interface )
        {
            if ( equipmentOSFromPlatform(plataforma) == "IOS XR" )
                return "BE"+pc->group;
            else
                return "Po"+pc->group;
        }
    }
    return interface;
}

QStringList interfacePortChannelToInterfaces(QList<SPortChannel *> lst,QString pcInterface)
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
    foreach (SPortChannel *pc, lst)
    {
        if ( group == pc->group )
            salida.append( pc->interfaz );
    }
    return salida;
}

Queries *buscarEquipoInterfazPorParejaDeIPMascara30(QList<Queries *> lst,
                                                    QString ipBuscar,
                                                    QString &interfaz,
                                                    QString ospfarea)
{
    if ( ipBuscar.isEmpty() )
        return nullptr;

    foreach (Queries *q, lst)
    {
        if ( !q->interfacesIpAddressesQuery )
            continue;

        foreach (SIpInfo *ii, q->interfacesIpAddressesInfo())
        {
            //verificamos que no se svi
            if ( ii->interfaz.contains("Vlan") ||
                 ii->interfaz.contains("BVI") ||
                 ii->interfaz.contains("BDI"))
                continue;

            //Que la ip no sea la misma
            if ( ipBuscar == ii->ip )
                continue;

            //que la ip no se este vacia
            if ( ii->ip.isEmpty() )
                continue;

            //verificamos que la interfaz tenga ospf
            bool encontrado=false;
            foreach (SOSPFInfo *oi, q->ospfInfo())
            {
                if ( oi->interfaz == ii->interfaz && oi->area == ospfarea )
                {
                    encontrado=true;
                    break;
                }
            }
            if ( !encontrado )
                continue;

            //verificando si en este equipo hay una IP que pertenezca al mismo segmento

            QStringList lstUno = ipBuscar.split(".");
            QStringList lstDos = ii->ip.split(".");

            QString ipuno;
            QString ipdos;

            foreach (QString oct, lstUno)
            {
                int dec = oct.toInt();
                QString salida="00000000";
                salida.append(QString::number(dec,2));
                ipuno.append( salida.right(8) );
            }

            foreach (QString oct, lstDos)
            {
                int dec = oct.toInt();
                QString salida="00000000";
                salida.append(QString::number(dec,2));
                ipdos.append( salida.right(8) );
            }

            if ( ipuno.left(30) != ipdos.left(30) )
                continue;

            //IP pertenece al mismo segmento /30

            //verificando que la interfaz de este equipo no esta en una vrf
            encontrado = false;
            foreach (SVrfInfo *vrf, q->vrfsInfo())
            {
                if ( vrf->interfaz == ii->interfaz &&
                     !vrf->vrf.isEmpty() )
                {
                    encontrado=true;
                    break;
                }
            }
            if ( encontrado )
                continue;

            if ( !q->ospfQuery )
                continue;           

            //interfaz no esta en una vrf
            //interfaz tiene ospf
            //Las IPs pertenecen al mismo segmento /30
            interfaz=ii->interfaz;

            qDebug() << "buscar vecino mascara 30: se encontro" << ipBuscar << q->hostName() << q->ip()
                     << ii->vrf << ii->interfaz;

            return q;
        }
    }

    return nullptr;
}

bool existeEquipmentNeighborsOperativoHaciaEquipo(QList<SEquipmentNeighborsInfo *> lst, QString hostname )
{
    foreach (SEquipmentNeighborsInfo *vecino, lst)
    {
        if ( !vecino->operativo )
            continue;

        if ( vecino->nombreequipo == hostname )
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

SIpInfo* buscarARP(QList<Queries *> lstQueries, QString IPoMAc)
{
    foreach (Queries *q, lstQueries)
    {
        if ( !q->arpsQuery )
            return nullptr; //si al primer queries no tiene info de arp se regresa null ya que los demas tampoco tendran

        foreach (SIpInfo *arp, q->arpsInfo())
        {
            if ( arp->ip == IPoMAc )
                return arp;
            if ( arp->mac == IPoMAc )
                return arp;
        }
    }
    return nullptr;
}

bool mplsInterfaceStatus(QString interface,
                         QString& txtStatusOut,
                         QList<SMplsLdpInfo*> lstDiscovery,
                         QList<SMplsLdpInfo*> lstInterfaces)
{
    foreach (SMplsLdpInfo *discoveryInfo, lstDiscovery)
    {
        if ( discoveryInfo->interfaz == interface )
        {
            //si se encuentra en discovery esta arriba y obviamente configurado
            txtStatusOut = "up";
            return true;
        }
    }

    //No estaba arriba, vemos en las interfaces si esta configurado
    foreach (SMplsLdpInfo *interfaceInfo, lstInterfaces)
    {
        if ( interfaceInfo->interfaz == interface )
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
                         QList<SOSPFInfo*> lstOspfInfo )
{
    foreach (SOSPFInfo* info, lstOspfInfo)
    {
        if ( info->interfaz == interface )
        {
            if ( info->state.contains("FULL") )
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

QString buscarArchivoConsultasActual(QString pais,QString path,QString filelabel)
{
    //buscando el archivo de consultas mas actual a actualizar
    QString file;
    QDateTime mieDateTime;
    QDir dir(path);
    QDirIterator dirIter( path, QDir::Files );
    QRegExp exp(pais+"_"+filelabel+"_*Queries_"+dir.dirName()+"_(\\d{4}-\\d{2}-\\d{2}-\\d{2}-\\d{2}-\\d{2})\\.dat");
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

QString nombreArchivoLstQueries(LstQueries* lstQ)
{
    return lstQ->pais +
            (lstQ->label.isEmpty()?"":"_"+lstQ->label)+
            "_Queries"+
            (lstQ->grupoconsulta.isEmpty()?"":"_"+lstQ->grupoconsulta)+
            "_"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".dat";
}
