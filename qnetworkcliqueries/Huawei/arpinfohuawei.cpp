#include "arpinfohuawei.h"

ArpInfoHuawei::ArpInfoHuawei(QRemoteShell *terminal, int option):
    ArpInfo(terminal,option)
{}

ArpInfoHuawei::ArpInfoHuawei(const ArpInfoHuawei &other):
    ArpInfo(other.term,other.m_queryoption)
{}

ArpInfoHuawei::~ArpInfoHuawei()
{}

void ArpInfoHuawei::getArpInfo()
{
    m_vrfs = m_queriesConfiguration.values("Arp_VRFs",m_ip,m_os,m_conexionID,m_queryName);
    m_macip = m_queriesConfiguration.value("ARP_MacIP",m_ip,m_os,m_conexionID,m_queryName);

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

        termSendText("display arp all | include "+m_vrf+" | exclude Incomplete" );
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
        //10.35.188.46    -          802d.bf17.f729  Interface  ARPA  BVI600
        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}.+\\w{4}\\-\\w{4}\\-\\w{4} (\\d+)* *(D|I)");
        if ( !line.contains(exp,&match) )
            continue;

        QStringList data = line.split(" ",Qt::SkipEmptyParts);
        QString _ip;
        QString _mac;
        QString _vlan;
        QString _age;
        QString _interfaz;
        QRegularExpression expNum("^\\d+$");

        if ( m_os == "VRP" )
        {
            _ip = data.at(0).simplified();
            _mac = data.at(1).simplified().replace("-",".");
            QString col3 = data.at(2);
            if (col3.contains("I") || col3.contains("D") )
                _age = "-";
            else if (col3.contains(expNum))
                _age = col3;
            _interfaz = estandarizarInterfaz( data.at(4).simplified() );

            QString vlan = _interfaz;
            if ( vlan.contains("Vlan") )
                vlan.replace("Vlan","");
            else if ( vlan.contains(".") )
                vlan = vlan.right( vlan.size() - vlan.indexOf(".") -1 );
            else
                vlan.clear();

            _vlan = vlan;
        }        

        SIpInfo mac;
        mac.ip = _ip;
        mac.vlan = _vlan;
        mac.interfaz = _interfaz;
        mac.age = _age;
        mac.mac = _mac;
        mac.operativo = true;
        mac.datetime = QDateTime::currentDateTime();
        mac.vrf = m_vrf;
        m_lstArp.append(mac);
    }
    m_siguienteVRF();
}
