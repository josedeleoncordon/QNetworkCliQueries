#include "interfaceinfo.h"
#include "funciones.h"

SInterfaceInfo::SInterfaceInfo(const SInterfaceInfo &other)
{
    interfaz = other.interfaz;
    status = other.status;
    mac = other.mac;
    ratein = other.ratein;
    rateout = other.rateout;
    dropsin = other.dropsin;
    dropsout = other.dropsout;
    errorsin = other.errorsin;
    errorsout = other.errorsout;
    CRC = other.CRC;
    overrun = other.overrun;
    collisions = other.collisions;
    bandwidth = other.bandwidth;
    mtu = other.mtu;
    duplex = other.duplex;
    speed = other.speed;
    opticalRx = other.opticalRx;
    mediaType = other.mediaType;
}

QDataStream& operator<<(QDataStream& out, const SInterfaceInfo* data)
{
    out << data->interfaz;
    out << data->status;
    out << data->description;
    out << data->mac;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SInterfaceInfo*& data)
{
    data = new SInterfaceInfo;
    in >> data->interfaz;
    in >> data->status;
    in >> data->description;
    in >> data->mac;
    //infobase
    in >> data->datetime;
    in >> data->operativo;
    return in;
}

QDataStream& operator<<(QDataStream& out, const SInterfaceVlans* data)
{
    out << data->interfaz;
    out << data->vlans;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SInterfaceVlans*& data)
{
    data = new SInterfaceVlans;
    in >> data->interfaz;
    in >> data->vlans;
    //infobase
    in >> data->datetime;
    in >> data->operativo;
    return in;
}

void updateInfoList(QList<SInterfaceInfo *> &lstDest, QList<SInterfaceInfo *> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SInterfaceInfo *dest = lstDest.at(c);
        if ( dest->datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest->operativo )
             lstDest.removeAt( c );
        else
        {
            dest->operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    foreach ( SInterfaceInfo *origin, lstOrigin )
    {
        bool encontrado=false;
        foreach ( SInterfaceInfo *dest, lstDest )
        {
            if ( origin->interfaz == dest->interfaz )
            {
                //Si se encontro, actualizamos los datos
                dest->datetime = origin->datetime;
                dest->operativo = true;
                dest->status = origin->status;
                dest->description = origin->description;
                dest->mac = origin->mac;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

void updateInfoList(QList<SInterfaceVlans *> &lstDest, QList<SInterfaceVlans *> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SInterfaceVlans *dest = lstDest.at(c);
        if ( dest->datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest->operativo )
             lstDest.removeAt( c );
        else
        {
            dest->operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    foreach ( SInterfaceVlans *origin, lstOrigin )
    {
        bool encontrado=false;
        foreach ( SInterfaceVlans *dest, lstDest )
        {
            if ( origin->interfaz == dest->interfaz )
            {
                //Si se encontro, actualizamos los datos
                dest->datetime = origin->datetime;
                dest->operativo = true;
                dest->vlans = origin->vlans;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

InterfaceInfo::InterfaceInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{
    m_InterfacesInfo_onlyPhysicalInterfaces=true;
    m_continueShowVlan=false;
    m_continueShowVlanBridge=false;
}

InterfaceInfo::InterfaceInfo(const InterfaceInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_InterfacesInfo_onlyPhysicalInterfaces = other.m_InterfacesInfo_onlyPhysicalInterfaces;
    foreach (SIpInfo *ii, other.m_lstInterfacesIPAddresses)
    {
        SIpInfo *ii2 = new SIpInfo(*ii);
        m_lstInterfacesIPAddresses.append(ii2);
    }
    foreach (SInterfaceInfo *ii, other.m_lstInterfacesInfo)
    {
        SInterfaceInfo *ii2 = new SInterfaceInfo(*ii);
        m_lstInterfacesInfo.append(ii2);
    }
    foreach (SInterfaceVlans *ii, other.m_lstInterfacesPermitedVlans)
    {
        SInterfaceVlans *ii2 = new SInterfaceVlans(*ii);
        m_lstInterfacesPermitedVlans.append(ii2);
    }
    m_continueShowVlan = other.m_continueShowVlan;
    m_continueShowVlanBridge = other.m_continueShowVlanBridge;
    m_interfaces = other.m_interfaces;
    m_interfaceCurrent = other.m_interfaceCurrent;
    m_interfacesPos = other.m_interfacesPos;
}

InterfaceInfo::~InterfaceInfo()
{
    qDeleteAll(m_lstInterfacesIPAddresses);
    qDeleteAll(m_lstInterfacesInfo);
    qDeleteAll(m_lstInterfacesPermitedVlans);
}

void InterfaceInfo::getInterfacesInfo()
{
    m_interfacesPos=-1;
    m_interfaces = QueriesConfiguration::instance()->mapQueriesToList("InterfaceInfo_Interfaces");
    getInterfacesInfoNextInteface();
}

void InterfaceInfo::getInterfacesInfoNextInteface()
{
    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_Info()));

    if ( m_interfaces.isEmpty() )
    {
        //se consultan todas las interfaces
        if ( m_brand == "Cisco" )
            termSendText("show interface");
        else if ( m_brand == "Huawei" )
            termSendText("display interface");
        else
            finished();
    }
    else
    {
        //se consultan las interfaces configuradas
        if ( m_interfacesPos < m_interfaces.size()-1 )
        {
            m_interfaceCurrent = m_interfaces.at( ++m_interfacesPos );
            if ( m_brand == "Cisco" )
                termSendText("show interface "+m_interfaceCurrent);

            //TODO ANALIZAR HUAWEI
//            else if ( m_brand == "Huawei" )
//                termSendText("display interface "+m_interfaceCurrent);

            else
                finished();
        }
        else
            //ya no hay mas interfaces que buscar, se finaliza
            finished();
    }
}

void InterfaceInfo::getInterfacesIpAddresses()
{
    if ( m_brand != "Cisco" )
    {
        finished();
        return;
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_IpAddresses()));    
    termSendText("show ip interface brief");
}

void InterfaceInfo::getInterfacesPermitedVlans()
{
    term->disconnectReceiveTextSignalConnections();

    if ( m_brand == "Cisco" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_PermitedVlansTrunk()));
        if ( m_os == "IOS XR" )
            termSendText("show ip interface brief | i unassigned");
        else
        {
            m_continueShowVlanBridge=true;
            termSendText("show interfaces trunk");
        }
    }
    else if ( m_brand == "Huawei" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_PermitedVlansTrunkHuawei()));
        termSendText("display vlan brief");
    }
    else
    {
        emit finished();
        return;
    }
}

void InterfaceInfo::getInterfacesDescriptions()
{
    term->disconnectReceiveTextSignalConnections();   

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_Descriptions()));        

    if ( m_brand == "Cisco" )
        termSendText("show interface description");
    else if ( m_brand == "Huawei" )
        termSendText("display interface description");
    else
        finished();
}

void InterfaceInfo::on_term_receiveText_Info()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    SInterfaceInfo *id=nullptr;
    QString interline,interstatus;
    QRegExp expInterface;
    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( m_brand == "Cisco" )
        {
            expInterface.setPattern("^(.+) is (.+), line protocol is (\\w+)");
            if ( line.contains(expInterface) )
            {
                QString interface = estandarizarInterfaz(expInterface.cap(1));

                id = new SInterfaceInfo;
                id->queryParent = m_queriesParent;
                id->interfaz = interface;
                id->datetime = QDateTime::currentDateTime();
                id->operativo = true;

                interstatus = id->status = expInterface.cap(2);
                interline = id->status = expInterface.cap(3);
                if ( interstatus == "administratively down" )
                    id->status = interstatus;
                else
                {
                    if ( interstatus == "up" && interline == "up" )
                        id->status = "up";
                    else
                        id->status = "down";
                }
                m_lstInterfacesInfo.append(id);

                continue;
            }
            expInterface.setPattern("^(.+) is (up|down)($| )+");
            if ( line.contains(expInterface) )
            {
                QString interface = estandarizarInterfaz(expInterface.cap(1));

                id = new SInterfaceInfo;
                id->queryParent = m_queriesParent;
                id->interfaz = interface;
                id->status = expInterface.cap(2);
                id->datetime = QDateTime::currentDateTime();
                id->operativo = true;
                m_lstInterfacesInfo.append(id);

                continue;
            }

            if ( !id )
                continue;

            //MTU, BW
            exp.setPattern("MTU (\\d+) bytes, BW (\\d+) Kbit");
            if ( line.contains(exp) )
            {
                id->mtu = exp.cap(1);
                id->bandwidth = exp.cap(2);
                continue;
            }

            //duplex, speed
            exp.setPattern("(.+duplex), (\\S+),");
            if ( line.contains(exp) )
            {
                id->duplex = exp.cap(1);
                id->speed = exp.cap(2);
                exp.setPattern("media type is (.+)$");
                if ( line.contains(exp) )
                    id->mediaType = exp.cap(1);
                continue;
            }

            //mac
            exp.setPattern("Hardware.+address is ((\\w|\\d|\\.)+)( |$)");
            if ( line.contains(exp) )
            {
                id->mac = exp.cap(1);
                continue;
            }

            //Descripcion
            exp.setPattern("Description: (.+)$");
            if ( line.contains(exp) )
            {
                id->description = exp.cap(1).simplified().replace("\"","").replace("'","");
                continue;
            }

            //rate
            exp.setPattern("input rate (\\d+) bits");
            if ( line.contains(exp) )
            {
                id->ratein=exp.cap(1);
                continue;
            }
            exp.setPattern("output rate (\\d+) bits");
            if ( line.contains(exp) )
            {
                id->rateout=exp.cap(1);
                continue;
            }

            //drops
            expInterface.setPattern("Total output drops: (\\d+)");
            if ( line.contains(expInterface) )
            {
                id->dropsout = expInterface.cap(1);
                continue;
            }
            expInterface.setPattern("(\\d+) total output drops");
            if ( line.contains(expInterface) )
            {
                id->dropsout = expInterface.cap(1);
                continue;
            }
            expInterface.setPattern("(\\d+) total input drops");
            if ( line.contains(expInterface) )
            {
                id->dropsin = expInterface.cap(1);
                continue;
            }

            //input errors, CRC, overruns
            expInterface.setPattern("(\\d+) input errors,.+(\\d+) CRC,.+(\\d+) overrun");
            if ( line.contains(expInterface) )
            {
                id->errorsin = expInterface.cap(1);
                id->CRC = expInterface.cap(2);
                id->overrun = expInterface.cap(3);
                continue;
            }

            //output errors, collisions
            expInterface.setPattern("(\\d+) output errors");
            if ( line.contains(expInterface) )
            {
                id->errorsout = expInterface.cap(1);
                expInterface.setPattern("(\\d)+ collisions");
                if ( line.contains(expInterface) )
                    id->collisions = expInterface.cap(1);

                //lo ultimo que se busca
                id=nullptr;

                continue;
            }
        }
        else if ( m_brand == "Huawei" )
        {
            expInterface.setPattern("^(.+) current state : (UP|DOWN|Administratively DOWN)");
            if ( line.contains(expInterface) )
            {
                QString interface = estandarizarInterfaz(expInterface.cap(1));
                if ( interface == "Line protocol" )
                {
                    interline = expInterface.cap(1).simplified().toLower();
                    if ( interstatus == "administratively down" )
                        id->status = interstatus;
                    else
                    {
                        if ( interstatus == "up" && interline == "up" )
                            id->status = "up";
                        else
                            id->status = "down";
                    }
                }
                else
                {
                    id = new SInterfaceInfo;
                    id->queryParent = m_queriesParent;
                    id->interfaz = interface;
                    id->datetime = QDateTime::currentDateTime();
                    id->operativo = true;
                    interstatus = expInterface.cap(2).toLower();
                    m_lstInterfacesInfo.append(id);
                }
                continue;
            }

            if ( !id )
                continue;

            //Descripcion
            exp.setPattern("^Description:(.+)$");
            if ( line.contains(exp) )
            {
                id->description = exp.cap(1).simplified().replace("\"","").replace("'","");
                continue;
            }
        }
    }

    if (  m_interfaces.isEmpty() )
        //si es consulta de todas las interfaces no se consulta las potencia opticas ya que en algunos equipos es
        //muy tardado
        finished();
    else
    {
        //consultamos potencia optica ya que se especifo la interfaz
        txt.clear();
        term->disconnectReceiveTextSignalConnections();
        if ( m_brand == "Cisco" )
        {
            connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_InfoTransceiver()));
            if ( m_os == "IOS XR" )
            {
                QString iitt;
                if ( m_platform.contains("NCS-6000") )
                {
                    //en los NCS-6000 se cambia la tengiga Te por tenGigE para que funcione el comando
                    if ( m_interfaceCurrent.left(2) == "Te" )
                        iitt = "tenGigE";
                    else
                        iitt = m_interfaceCurrent;
                }
                else
                    iitt = m_interfaceCurrent;

                termSendText("sh controllers "+ iitt +" phy | i Rx");
            }
            else
                termSendText("sh int "+ m_interfaceCurrent +" transceiver | b -----");
        }
        if ( m_brand == "Huawei")
            //Huawei de una vez muestra la potencia en la informacion de interfaz, continuamos con la siguiente.
            getInterfacesInfoNextInteface();
    }
}

void InterfaceInfo::on_term_receiveText_InfoTransceiver()
{
    //una sola interfaz se analiza ya que desde el principio solo se consulta potencias si solo es una

    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QString rx;
    foreach (QString line, lines)
    {
        line = line.simplified();        

        if ( m_os == "IOS XR" )
        {
            exp.setPattern( "Rx power: (.+)$" );
            if ( line.contains(exp) )
            {
                rx = exp.cap(1).simplified();
                break;
            }
        }
        else
        {
            exp.setPattern(" +\\-\\d+\\.\\d+ +(\\-\\d+\\.\\d+)");
            if ( line.contains(exp) )
            {
                rx = exp.cap(1);
                break;
            }
        }       
    }

    foreach (SInterfaceInfo* ii, m_lstInterfacesInfo)
    {
        if ( ii->interfaz == m_interfaceCurrent )
        {
            ii->opticalRx = rx;
            break;
        }
    }

    getInterfacesInfoNextInteface();
}

void InterfaceInfo::on_term_receiveText_IpAddresses()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    exp.setPattern("^(.+) +(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}|unassigned).+");
    exp.setMinimal(false);
    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( !line.contains(exp) )
            continue;

        SIpInfo *m = new SIpInfo;
        m->queryParent = m_queriesParent;
        m->interfaz = estandarizarInterfaz( exp.cap( 1 ) );
        m->ip = exp.cap(2);
        m->datetime = QDateTime::currentDateTime();
        m->operativo = true;
        m_lstInterfacesIPAddresses.append(m);
    }
    finished();
}

void InterfaceInfo::on_term_receiveText_PermitedVlansTrunk()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    bool a = false;
    SInterfaceVlans *iv = nullptr;
    QString temp;

    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( m_os == "IOS XR" )
        {
//            GigabitEthernet200/0/0/8       unassigned      Up                    Up
//            GigabitEthernet200/0/0/8.1609  unassigned      Up                    Up
//            GigabitEthernet200/0/0/8.1705  unassigned      Up                    Up

            exp.setPattern("unassigned +Up +Up");
            if ( ! line.contains(exp) )
                continue;

            QStringList c = line.split(" ",QString::SkipEmptyParts);
            QString interface = c.at(0);

            if ( !interface.contains(".") )
                continue;

            QStringList a = interface.split(".");
            QString inter = estandarizarInterfaz( a.at(0) );
            QString vlan = a.at(1);

            if ( !iv || inter != temp )
            {
                iv = new SInterfaceVlans;
                iv->queryParent = m_queriesParent;
                iv->interfaz = inter;
                iv->datetime = QDateTime::currentDateTime();
                iv->operativo = true;
                m_lstInterfacesPermitedVlans.append( iv );
                temp = inter;
            }
            iv->vlans.append( vlan );
        }
        else
        {
//            Port                Vlans allowed on trunk
//            Gi3/3               1832
//            Gi3/6               1-4094
//            Gi3/7               1-4094
//            Gi3/8               1-4094
//            Gi3/9               10,12,14,16,25,37,48,61-64,83-84,91,93,96,99,152,172,174-175,177,179,184,186,191,238,26
//            6,368,379-380,387,493,512,514,518,526,913,953,968-969,983-984

            if ( !a )
            {
                if ( line.contains("Vlans allowed on trunk",Qt::CaseInsensitive) )
                    a = true;
                continue;
            }
            else
            {
                if ( line.contains(" ") )
                {
                    if ( iv )
                    {
                        iv->vlans = numberRangeToLST( temp );
                        m_lstInterfacesPermitedVlans.append( iv );
                    }

                    if ( line.contains("Vlans allowed and active") )
                    {
                        a = false;
                        break;
                    }

                    QStringList a = line.split(" ",QString::SkipEmptyParts);
                    iv = new SInterfaceVlans;
                    iv->queryParent = m_queriesParent;
                    iv->interfaz =  estandarizarInterfaz( a.at(0) );
                    iv->datetime = QDateTime::currentDateTime();
                    iv->operativo = true;
                    temp = a.at(1);
                    continue;
                }
                else
                {
                    temp.append( line );
                    continue;
                }
            }
        }
    }

    if ( m_continueShowVlanBridge )
    {
        txt.clear();
        term->disconnectReceiveTextSignalConnections();
        m_continueShowVlan=true;
		connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_PermitedVlansBridge()));
        termSendText("show ethernet service interface detail | i Interface:|Bridge-domains:");
    }
    else
        finished();
}

void InterfaceInfo::on_term_receiveText_PermitedVlansTrunkHuawei()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    QMap<QString,QString> map;
    QString vlan;
    QRegExp exp("(\\d+).+enable +(TG|UT): ");
    QRegExp expinter("(Eth-Trunk\\d+|[a-zA-Z]+\\d+/\\d+(/\\d+)*)\\(");
    for (QString line : lines)
    {
        line = line.simplified();
        if ( line.contains(exp) )
        {
            vlan = exp.cap(1);
            int pos = 0;
            while ((pos = expinter.indexIn(line, pos)) != -1)
            {
                QString interfaz = estandarizarInterfaz(expinter.cap(1));
                map.insertMulti(interfaz,vlan);
                pos += expinter.matchedLength();
            }
        }
        else if ( line.contains(expinter) )
        {
            int pos = 0;
            while ((pos = expinter.indexIn(line, pos)) != -1)
            {
                QString interfaz = estandarizarInterfaz(expinter.cap(1));
                map.insertMulti(interfaz,vlan);
                pos += expinter.matchedLength();
            }
        }
    }
    for ( QString interfaz : map.uniqueKeys() )
    {
        SInterfaceVlans *iv = new SInterfaceVlans;
        iv->queryParent = m_queriesParent;
        iv->interfaz = interfaz;
        iv->vlans = map.values( interfaz );
        iv->datetime = QDateTime::currentDateTime();
        iv->operativo=true;
        m_lstInterfacesPermitedVlans.append( iv );
    }

    finished();
}

void InterfaceInfo::on_term_receiveText_PermitedVlansBridge()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    SInterfaceVlans *iv = nullptr;
    for (QString line : lines)
    {
        line = line.simplified();

        exp.setPattern("Interface: (.+)\\, Type:.+"); //Interface: GigabitEthernet0/1, Type: UNI
        if ( line.contains(exp) )
        {
            iv = new SInterfaceVlans;
            iv->queryParent = m_queriesParent;
            iv->interfaz = estandarizarInterfaz( exp.cap(1) );
            iv->datetime = QDateTime::currentDateTime();
            iv->operativo = true;
            continue;
        }

        exp.setPattern("Bridge-domains: (.+)$"); //Bridge-domains: 1-5,7,10-11,14-16,18-22,24,28-29,32,34
        if ( line.contains(exp) && iv )
        {
            iv->vlans = numberRangeToLST( exp.cap(1) );
            m_lstInterfacesPermitedVlans.append( iv );
            iv = nullptr;
            continue;
        }
    }

    if ( m_continueShowVlan )
    {
        txt.clear();
        term->disconnectReceiveTextSignalConnections();
		connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_PermitedVlansAccess()));
        termSendText("show vlan");
    }
    else
        finished();
}

void InterfaceInfo::on_term_receiveText_PermitedVlansAccess()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    for (QString line : lines)
    {
        line = line.simplified();

//        333  IX_IJSUD_YUSGUARE_SAY_01_2M      active
//        344  ESCUELA_JOSE_CECILIO_            active
//        525  TIENDAS_CLARO                    active
//        530  SEGURIDAD_ELECTRONICA_CHOLUTECA  active    Fa0/2, Fa0/3, Fa0/4, Fa0/24

        if ( !line.contains(" active") )
            continue;

        QStringList data = line.split(" ",QString::SkipEmptyParts);
        if ( data.size() > 3 )
        {
            for ( int c=3; c<data.size(); c++ )
            {
                SInterfaceVlans *iv = new SInterfaceVlans;
                iv->queryParent = m_queriesParent;
                QString interfaz = data.at(c);
                interfaz.replace(",","");

                iv->interfaz = estandarizarInterfaz( interfaz );
                iv->vlans = QStringList() << data.at(0);
                iv->datetime = QDateTime::currentDateTime();
                iv->operativo = true;

                m_lstInterfacesPermitedVlans.append( iv );
            }
        }
    }
    finished();
}

void InterfaceInfo::on_term_receiveText_Descriptions()
{
    txt.append(term->dataReceived());   

    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");    

    foreach (QString line, lines)
    {
        if ( m_brand == "Cisco" )
        {
            if (line.contains("Interface") || line.contains("Status") ||
                    line.contains("interface descr") || line.contains("Description") ||
                    !line.contains(QRegExp(" (up|down|admin) +(up|down) ")))
                continue;                                    

            SInterfaceInfo *id = new SInterfaceInfo;
            id->queryParent = m_queriesParent;

            QStringList data = line.split("  ",QString::SkipEmptyParts);

            id->interfaz = estandarizarInterfaz( data.at(0).simplified() );
            id->datetime = QDateTime::currentDateTime();
            id->operativo = true;
            QString status = data.at(1).simplified();
            QString protocol = data.at(2).simplified();
            if ( status == "up" && protocol == "up" )
                id->status = "up";
            else if ( status.contains("admin") )
                id->status = "admin down";
            else
                id->status = "down";
            if ( data.size() >= 4 )
            {
                QStringList desc;
                for ( int c=3; c<data.size(); c++ )
                    desc.append(data.at(c).simplified());

                id->description = desc.join(" ").replace("\"","").replace("'","");
            }
            m_lstInterfacesInfo.append(id);
        }
        else if ( m_brand == "Huawei" )
        {
            if ((line.contains("Interface",Qt::CaseInsensitive) && line.contains("Description",Qt::CaseInsensitive)) )
                continue;

            QStringList data = line.split("   ",QString::SkipEmptyParts);

            if ( data.size() < 2 )
                continue;

            SInterfaceInfo *id = new SInterfaceInfo;
            id->queryParent = m_queriesParent;
            id->interfaz = estandarizarInterfaz( data.at(0).simplified() );
            id->datetime = QDateTime::currentDateTime();
            id->operativo = true;
            id->description = data.at(1).simplified().replace("\"","").replace("'","");
            m_lstInterfacesInfo.append(id);
        }        
    }    

    finished();
}

QStringList InterfaceInfo::interfacePermitedVlans(QString interface)
{
    foreach (SInterfaceVlans *i, m_lstInterfacesPermitedVlans)
    {
        if ( interface == i->interfaz )
            return i->vlans;
    }
    return QStringList();
}

SInterfaceInfo *InterfaceInfo::interfaceInfo(QString interfaz)
{
    foreach (SInterfaceInfo *i, m_lstInterfacesInfo)
    {
        if ( interfaz == i->interfaz )
            return i;
    }
    return nullptr;
}

QDataStream& operator<<(QDataStream& out, const InterfaceInfo* ii)
{
    out << ii->m_lstInterfacesIPAddresses;
    out << ii->m_lstInterfacesInfo;
    out << ii->m_lstInterfacesPermitedVlans;
    return out;
}

QDataStream& operator>>(QDataStream& in, InterfaceInfo*& ii)
{
    ii = new InterfaceInfo(nullptr,nullptr);
    in >> ii->m_lstInterfacesIPAddresses;
    in >> ii->m_lstInterfacesInfo;
    in >> ii->m_lstInterfacesPermitedVlans;
    return in;
}

QDebug operator<<(QDebug dbg, const InterfaceInfo &info)
{
    dbg.nospace() << "InterfaceInfo:\n";
    foreach (SIpInfo *i, info.m_lstInterfacesIPAddresses)
        dbg.space() << i->interfaz << i->ip << i->datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    foreach (SInterfaceInfo *i, info.m_lstInterfacesInfo)
        dbg.space() << i->interfaz << i->status << i->mac << i->description <<
                       i->ratein << i->rateout << i->dropsin << i->dropsout << i->errorsin << i->errorsout <<
                       i->CRC << i->overrun << i->collisions << i->bandwidth << i->mtu << i->duplex << i->speed <<
                       i->opticalRx << i->mediaType << i->datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    foreach (SInterfaceVlans *i, info.m_lstInterfacesPermitedVlans)
        dbg.space() << i->interfaz << i->vlans << i->datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
