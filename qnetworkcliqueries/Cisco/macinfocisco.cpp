#include "macinfocisco.h"
#include "funciones.h"

MacInfoCisco::MacInfoCisco(QRemoteShell *terminal, QObject *parent):
    MacInfo(terminal,parent)
{
    m_consultarXconnect=false;    
}

MacInfoCisco::MacInfoCisco(const MacInfoCisco &other):
    MacInfo(other.term,other.parent())
{}

MacInfoCisco::~MacInfoCisco()
{}

void MacInfoCisco::getMacInfo()
{
    qDebug() << "MacInfoCisco::getMacInfo()";

    m_mac = QueriesConfiguration::instance()->value("MAC_MAC",m_ip);
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));

    if ( m_platform.contains("ASR-9") ||
         m_platform.contains("A901"))
        termSendText("sh mac-address-table " + ( !m_vlan.isEmpty() ? " bdomain "+m_vlan : "" ) +
                     ( !m_mac.isEmpty() ? " | i "+m_mac : "" ) );
    else if ( m_platform.contains("6524") ||
              m_platform.contains("4503") )
        termSendText("sh mac-address-table " + ( !m_vlan.isEmpty() ? " vlan "+m_vlan : "" ) +
                     ( !m_mac.isEmpty() ? " | i "+m_mac : "" ) );
    else if ( m_os == "IOS XR" )
        termSendText("sh l2vpn forwarding bridge-domain mac-address location " +m_xr_location+ " "+
                     ( !m_vlan.isEmpty() || !m_mac.isEmpty() ? QString(" | i \"") : "" ) +
                     ( !m_mac.isEmpty() ? m_mac : "" ) +
                     ( !m_vlan.isEmpty() ? ".+BD id: "+m_vlan : "" )+
                     ( !m_vlan.isEmpty() || !m_mac.isEmpty() ? "\"" : "" ) );
    else
        termSendText("sh mac address-table " +
                     ( !m_mac.isEmpty() ? " address "+m_mac : "" )+
                     ( !m_vlan.isEmpty() ? " vlan "+m_vlan : "" ));
}

void MacInfoCisco::on_term_receiveText()
{    
    txt.append(term->dataReceived());

    if ( !allTextReceived() )
        return;

    exp.setPattern("Eth VLAN \\d+$");    

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( (! line.contains("dynamic",Qt::CaseInsensitive ) &&
              ! line.contains("static",Qt::CaseInsensitive )) ||
             line.contains("drop",Qt::CaseInsensitive) )
            continue;

        SMacInfo mac;
        mac.datetime = QDateTime::currentDateTime();
        mac.operativo = true;

        line.replace("* ","");

        QStringList data = line.split(" ",QString::SkipEmptyParts);

        if ( m_os == "IOS XR" )
        {
            //TODO como el concepto del XR es bridge-domains se supone que el numero de las subinterfaces y los pw-id
            //de los PW coinciden con la vlan dot1q.

            mac.mac = data.at(0).simplified();

            QRegExp expip("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
            if ( data.at(2).contains(expip) ) //PW
            {
                //000e.50ef.4340 dynamic (10.192.65.146, 322261)     0/0/CPU0   0d 0h 0m 18s       N/A
                //XR no es necesario consultar los PW, en la misma tabla de mac se muestra hacia donde va
                mac.interfaz = data.at(2)+" "+data.at(3);
                mac.vlan = data.at(3);
                mac.vlan.replace(")","");
            }
            else
            {
                QString interfaz;
                interfaz = data.at(2).simplified();

                QStringList inter = interfaz.split(".",QString::SkipEmptyParts);
                mac.interfaz = estandarizarInterfaz( inter.at(0) );
                if ( inter.size() > 1 )
                    mac.vlan = inter.at(1);
                else
                    mac.vlan = "1";
            }
        }
        else
        {
            mac.mac = data.at(1).simplified();
            mac.vlan = data.at(0).simplified();

            QString interface;            
            if ( line.contains("efp_id") )
                //1666  0025.9ed3.10a8   dynamic  Yes          0   Te3/1 efp_id 1666
                interface = estandarizarInterfaz( data.at(data.size()-3).simplified() );
            else if ( line.contains(exp) )
            {
                //3222  001a.302b.35c0   dynamic  Yes        160   Eth VLAN 3222
                //En IOS se tendra que conultar los xconnect para ver hacia que PE va el PW
                m_consultarXconnect=true;
                interface = "PW_"+data.last();
            }
            else if ( data.last().size() == 1 )
                interface = estandarizarInterfaz( data.at(data.size()-2).simplified() );
            else
                //3737  00d0.d650.c84d   dynamic  Yes        190   Gi3/5
                interface = estandarizarInterfaz( data.last().simplified() );

            if (interface.contains("."))
            {
                int i = interface.indexOf(".");
                interface = interface.left( i );
            }

            if ( interface.contains("CPU") ||
                 interface.contains("Router") ||
                 interface.contains("Switch") ||
                 interface.contains("Stby") ||
                 interface.contains(",") ||
                 interface.contains(":") ||
                 interface.contains("slot") ||
                 interface.contains("engine") ||
                 interface.contains("unicast") ||
                 interface.contains("flood") )
            {
                continue;
            }

            mac.interfaz = interface;
        }

        //verificando si la mac esta en la vlans permitidas, si se configuro
        QStringList vlanfilter = QueriesConfiguration::instance()->values("MAC_vlansFilter",m_ip);
        if ( vlanfilter.isEmpty() )
            m_lstMacs.append(mac);
        else
        {
            if ( vlanfilter.contains( mac.vlan ) )
                m_lstMacs.append(mac);
        }
    }

    //se consulta los xconnect en IOS si se encontro una mac conocida por uno de estos
    if ( m_consultarXconnect )
    {
        term->disconnectReceiveTextSignalConnections();
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_iosXconnect()));
        termSendText( "show xconnect all" );
    }
    else
        finished();
}

void MacInfoCisco::on_term_receiveText_iosXconnect()
{
    txt.append(term->dataReceived());

    if ( !allTextReceived() )
        return;

    //UP pri   ac Vl154:154(Eth VLAN)          UP mpls 10.192.33.71:506             UP
    exp.setPattern("^UP .+ Vl(\\d+):.+ UP .+ (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}):(\\d+) +UP$");
    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( !line.contains(exp) )
            continue;

        QString vlan = exp.cap(1);
        QString ip = exp.cap(2);
        QString id = exp.cap(3);

        for (SMacInfo &mi : m_lstMacs)
        {
            if ( mi.interfaz.contains("PW_"+vlan) )
                //reemplazamos la interfaz creada a la hora de consultar la tabla de MAC y ponemos la IP hacia
                //donde va el PW
                mi.interfaz = "("+ip+","+id+")";
        }
    }

    finished();
}
