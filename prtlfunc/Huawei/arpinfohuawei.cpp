#include "arpinfohuawei.h"

ArpInfoHuawei::ArpInfoHuawei(QRemoteShell *terminal, QObject *parent):
    ArpInfo(terminal,parent)
{}

ArpInfoHuawei::ArpInfoHuawei(const ArpInfoHuawei &other):
    ArpInfo(other.term,other.parent())
{}

ArpInfoHuawei::~ArpInfoHuawei()
{}

void ArpInfoHuawei::getArpInfo()
{
    m_vrfs = QueriesConfiguration::instance()->mapQueriesToList("Arp_VRFs");
    m_macip = QueriesConfiguration::instance()->mapQueries.value("ARP_MacIP");    

    qDebug() << "ArpInfoHuawei::getArpInfo()" << m_vrfs  << m_macip;

    if ( m_vrfs.isEmpty() )
    {
        //TODO consultar tabla global. Problema es que para sacar el de esta hay consultar el de todas las VRFs
        //y filtrar por el que no tenga

        qDebug() << "ArpInfoHuawei::getArpInfo() consulta de arp de la tabla global no implementado";
        finished();
        return;
    }

    if ( m_vrfs.isEmpty() )
        m_vrfs.append(""); //para la global

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));
    m_siguienteVRF();
}

void ArpInfoHuawei::m_siguienteVRF()
{
    if ( m_vrfsPos < m_vrfs.size()-1 )
    {
        m_vrfsPos++;
        m_vrf = m_vrfs.at( m_vrfsPos );        

        if ( m_macip.isEmpty() )
            termSendText("display arp vpn-instance "+m_vrf+" slot 0 dynamic" );
        else
            termSendText("display arp network "+m_macip+" dynamic | i "+m_vrf );
    }
    else
        finished();
}

void ArpInfoHuawei::on_term_receiveText()
{
    txt.append( term->dataReceived() );
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        //10.31.15.104    0020-d237-ac51  5         D-0         Vlanif115        ADMIN_RAD_ISLAS_METRO
        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}.+\\w{4}\\-\\w{4}\\-\\w{4} +\\d+ +D");
        if ( !line.contains(exp) )
            continue;

        QStringList data = line.split(" ",QString::SkipEmptyParts);
        QString _ip;
        QString _mac;
        QString _vlan;
        QString _age;
        QString _interfaz;

        if ( m_os == "VRP" )
        {
            _ip = data.at(0).simplified();
            _mac = data.at(1).simplified().replace("-",".");
            _age = data.at(2).simplified();
            _interfaz = interfazSimplifier( data.at(4).simplified() );

            QString vlan = _interfaz;
            if ( vlan.contains("Vlan") )
                vlan.replace("Vlan","");
            else if ( vlan.contains(".") )
                vlan = vlan.right( vlan.size() - vlan.indexOf(".") -1 );

            _vlan = vlan;
        }        

        SIpInfo *mac = new SIpInfo;
        mac->ip = _ip;
        mac->vlan = _vlan;
        mac->interfaz = _interfaz;
        mac->age = _age;
        mac->mac = _mac;
        mac->queryParent = m_queriesParent;
        mac->operativo = true;
        mac->datetime = QDateTime::currentDateTime();
        mac->vrf = m_vrf;
        m_lstArp.append(mac);
    }
    m_siguienteVRF();
}
