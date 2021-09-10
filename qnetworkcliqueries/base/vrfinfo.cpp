#include "vrfinfo.h"
#include "funciones.h"

SVrfInfo::SVrfInfo(const SVrfInfo &other)
{
    interfaz = other.interfaz;
    vrf = other.vrf;
}

QDataStream& operator<<(QDataStream& out, const SVrfInfo& data)
{
    out << data.interfaz;
    out << data.vrf;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SVrfInfo& data)
{
    in >> data.interfaz;
    in >> data.vrf;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SVrfInfo> &lstDest, QList<SVrfInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SVrfInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SVrfInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SVrfInfo &dest : lstDest )
        {
            if ( origin.interfaz == dest.interfaz )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.vrf = origin.vrf;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

VrfInfo::VrfInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{}

VrfInfo::VrfInfo(const VrfInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_vlans = other.m_vlans;
    m_lstVrf = other.m_lstVrf;
    m_lstVrfInfo = other.m_lstVrfInfo;
    m_vrf = other.m_vrf;
    m_rt = other.m_rt;
}

VrfInfo::~VrfInfo()
{}

void VrfInfo::getVRFsFromVLans()
{
    m_vlans = m_queriesConfiguration.values("VRFfVlans_Vlans",m_ip,m_os,m_conexionID);

    if ( m_vlans.size() == 0 )
    {
        finished();
        return;
    }

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextFromVlans()));   
    if ( m_os == "IOS XR" )
        termSendText( "show ipv4 vrf all interface" );
    else if ( m_os == "IOS" )
        termSendText( "show ip vrf" );
    else if ( m_os == "VRP" )
        termSendText( "display ip vpn-instance verbose" );
    else
        finished();
}

void VrfInfo::getVRFfromRT()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "VrfInfo::getVRFfromRT()" << m_brand << "no soportado";
        finished();
        return;
    }

    m_rt = m_queriesConfiguration.value("VRFfRT_RT",m_ip,m_os,m_conexionID);

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextFromRT()));
    if ( m_os == "IOS XR" )
        termSendText( "show vrf all | i "+m_rt );
    else
        termSendText("show ip vrf | i "+m_rt);
}

void VrfInfo::getVRFs()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextVRFs()));
    if ( m_brand == "Cisco" )
    {
        if ( m_os == "IOS XR" )
            termSendText( "show ipv4 vrf all interface" );
        else
            termSendText("show ip vrf");
    }
    else if ( m_brand == "HUAWEI" )
    {
        if ( m_os == "VRP" )
            termSendText("display ip vpn-instance verbose");
    }
    else
        finished();
}

void VrfInfo::on_term_receiveTextFromVlans()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QString lastVrf;
    QString lastVlan;
    bool hinterface=false; //en los huawei si esta en true la linea sera una interfaz:       Vlanif136
    bool hagregar=false; //huawei agregar vrf

    QStringList lines = txt.split("\n");    

    foreach (QString line, lines)
    {
        if ( m_os == "IOS XR" )
        {
            exp.setPattern("\\S+\\.(\\d+) is (Up|Down|Shutdown)\\,.+");
            if ( line.contains(exp) )
            {
                lastVlan = exp.cap(1);
                if ( m_vlans.contains(lastVlan) )
                    break; //se continua con la siguiente linea para ver su vrf
                else
                    lastVlan.clear(); //vlan que no esta en el listado, no nos interesa
            }

            exp.setPattern("Vrf is (\\S+) \\(.+\\)");
            if ( line.contains(exp) && !lastVlan.isEmpty() )
            {
                lastVrf = exp.cap(1);
                if ( ! m_lstVrf.contains( lastVrf ) )
                    m_lstVrf.append( lastVrf );
            }
        }
        else if ( m_os == "IOS" )
        {
            exp.setPattern("^.+#\\s*$");
            if ( line.contains("name",Qt::CaseInsensitive) ||
                 line.contains(exp))
                continue;

            QString interface;
            QStringList data = line.split(" ",QString::SkipEmptyParts);

            if ( data.size() >= 3 )
            {
                lastVrf = data.at(0).simplified();
                interface = data.at(2).simplified();
            }
            else
                interface = data.at(0).simplified();

            QString interfaceVlan;

            exp.setPattern("(Vl|BD)+(\\d+)$");
            if ( interface.contains(exp) )
                interfaceVlan = exp.cap(2);

            exp.setPattern(".+\\d+/\\d+\\.(\\d+)$");
            if ( interface.contains(exp) )
                interfaceVlan = exp.cap(1);

            if ( m_vlans.contains(interfaceVlan) )
            {
                if ( ! m_lstVrf.contains( lastVrf ) )
                    m_lstVrf.append( lastVrf );
            }
        }
        else if ( m_os == "VRP" )
        {
            //VPN-Instance Name and ID : ADMIN_RAD_ISLAS_METRO, 7
            exp.setPattern("VPN-Instance Name and ID : (.+), \\d+");
            if ( line.contains(exp) )
            {
                lastVrf=exp.cap(1);
                continue;
            }

            //Interfaces : Vlanif115,
            exp.setPattern("Interfaces : (.+)$");
            if ( line.contains(exp) )
            {
                lastVlan = exp.cap(1).simplified();
                lastVlan.replace("Vlanif","");
                hagregar=true;

                if ( lastVlan.contains(",") )
                {
                    //hay mas de dos interfaces con esa vrf
                    lastVlan.replace(",","");
                    hinterface=true;
                }
            }

            //               Vlanif136
            if (hinterface)
            {
                if ( line.contains("Address family") )
                {
                    //se acabaron las interfaces de esta vrf
                    hinterface=false;
                    continue;
                }
                if ( lastVlan.isEmpty() )
                {
                    lastVlan = line.simplified();
                    lastVlan.replace("Vlanif","");
                    lastVlan.replace(",","");
                }
                hagregar=true;
            }

            if (hagregar)
            {
                hagregar=false;
                if ( m_vlans.contains(lastVlan) )
                {
                    if ( ! m_lstVrf.contains( lastVrf ) )
                        m_lstVrf.append( lastVrf );
                    lastVlan.clear();
                }
            }
        }
    }
    finished();
}

void VrfInfo::on_term_receiveTextFromRT()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    foreach (QString line, txt.split("\n"))
    {
        if ( m_os == "IOS XR" )
        {            
            if ( line.contains(m_rt) &&
                 !line.contains("IPV4") &&
                 !line.contains("show"))
            {
                QStringList data = line.split(" ",QString::SkipEmptyParts);
                m_vrf = data.at(0).simplified();
                break;
            }
        }
        else
        {
            exp.setPattern("\\d+\\:\\d+");

            if ( ! line.contains(exp) || line.contains("show") )
                continue;

            QStringList data = line.split(" ",QString::SkipEmptyParts);
            if ( data.size() < 3 )
                continue;

            m_vrf = data.at(0).simplified();
        }
    }
    finished();
}

void VrfInfo::on_term_receiveTextVRFs()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QString lastInterface;
    QString lastVrf;

    bool hinterface=false;
    bool hagregar=false;
    foreach (QString line, txt.split("\n"))
    {
        if ( m_os == "IOS XR" )
        {
            exp.setPattern("(\\S+) is (Up|Down|Shutdown)\\,.+");
            if ( line.contains(exp) )
            {
                lastInterface = estandarizarInterfaz( exp.cap(1) );
                qDebug() << "line interfaz" <<  lastInterface;
                continue;
            }

            exp.setPattern("Vrf is (\\S+) \\(.+\\)");
            if ( line.contains(exp) )
            {
                lastVrf = exp.cap(1).simplified();
                if ( lastVrf == "default" )
                    lastVrf.clear();

                SVrfInfo vi;
                vi.interfaz = lastInterface;
                vi.vrf = lastVrf;
                vi.datetime = QDateTime::currentDateTime();
                vi.operativo = true;
                m_lstVrfInfo.append(vi);

                qDebug() << "agrgando vrf";

                continue;
            }
        }
        else if ( m_os == "IOS" )
        {
            exp.setPattern("^.+#\\s*$");
            if ( line.contains("name",Qt::CaseInsensitive) ||
                 line.contains("show ip vrf") ||
                 line.contains(exp))
                continue;

            QStringList data = line.split(" ",QString::SkipEmptyParts);

            if ( data.size() >= 3 )
            {
                lastVrf = data.at(0).simplified();
                lastInterface = estandarizarInterfaz( data.at(2).simplified() );
            }
            else if ( data.size() == 2 )
                lastVrf = data.at(0).simplified();
            else if ( data.size() == 1 )
                lastInterface = estandarizarInterfaz( data.at(0).simplified() );            

            SVrfInfo vi;
            vi.interfaz = lastInterface;
            vi.vrf = lastVrf;
            vi.datetime = QDateTime::currentDateTime();
            vi.operativo = true;
            m_lstVrfInfo.append(vi);

            continue;
        }
        else if ( m_os == "VRP" )
        {
            //VPN-Instance Name and ID : ADMIN_RAD_ISLAS_METRO, 7
            exp.setPattern("VPN-Instance Name and ID : (.+), \\d+");
            if ( line.contains(exp) )
            {
                lastVrf=exp.cap(1);
                continue;
            }

            //Interfaces : Vlanif115,
            exp.setPattern("Interfaces : (.+)$");
            if ( line.contains(exp) )
            {
                lastInterface = exp.cap(1).simplified();
                hagregar=true;
                if ( lastInterface.contains(",") )
                {
                    //hay mas de dos interfaces con esa vrf
                    lastInterface.replace(",","");
                    hinterface=true;
                }
            }

            //               Vlanif136
            if (hinterface)
            {
                if ( line.contains("Address family") )
                {
                    //se acabaron las interfaces de esta vrf
                    hinterface=false;
                    continue;
                }
                if ( lastInterface.isEmpty() )
                {
                    lastInterface = line.simplified();
                    lastInterface.replace(",","");
                }
                hagregar=true;
            }

            if (hagregar)
            {
                hagregar=false;
                SVrfInfo vi;
                vi.interfaz = lastInterface;
                vi.vrf = lastVrf;
                vi.datetime = QDateTime::currentDateTime();
                vi.operativo = true;
                m_lstVrfInfo.append(vi);
                lastInterface.clear();
            }
        }
    }
    finished();
}

QString VrfInfo::vrfFromInterface(QString interface)
{
    for (SVrfInfo vi : m_lstVrfInfo)
    {
        if ( vi.interfaz == interface )
            return vi.vrf;
    }
    return "";
}

QDataStream& operator<<(QDataStream& out, const VrfInfo& info)
{
    out << info.m_lstVrf;
    out << info.m_vrf;
    out << info.m_lstVrfInfo;
    out << info.m_queryoption;
    return out;
}

QDataStream& operator>>(QDataStream& in, VrfInfo& info)
{
    in >> info.m_lstVrf;
    in >> info.m_vrf;
    in >> info.m_lstVrfInfo;
    in >> info.m_queryoption;
    return in;
}

QDataStream& operator<<(QDataStream& out, const VrfInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, VrfInfo*& info)
{
    info = new VrfInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const VrfInfo &info)
{
    dbg.nospace() << "VrfInfo:\n";

    if ( !info.m_vrf.isEmpty() )
        dbg.space() << "VrfFromRT" << info.m_vrf << "\n";

    dbg.space() << "\nVRFfvlans:" << "\n";
    foreach (QString i, info.m_lstVrf)
        dbg.space() << i << "\n";

    dbg.space() << "\nVRFs:" << "\n";
    foreach (SVrfInfo i, info.m_lstVrfInfo)
        dbg.space() << i.interfaz << i.vrf << i.datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
