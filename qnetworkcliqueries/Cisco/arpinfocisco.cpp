#include "arpinfocisco.h"

ArpInfoCisco::ArpInfoCisco(QRemoteShell *terminal, QueryOpcion option):
    ArpInfo(terminal,option)
{}

ArpInfoCisco::ArpInfoCisco(const ArpInfoCisco &other):
    ArpInfo(other.term,other.m_queryoption)
{}

ArpInfoCisco::~ArpInfoCisco()
{}

void ArpInfoCisco::getArpInfo()
{
	m_vrfs = m_queriesConfiguration.values("Arp_VRFs",m_ip,m_os);
	m_macip = m_queriesConfiguration.value("ARP_MacIP",m_ip,m_os);

    if ( m_vrfs.isEmpty() )
        m_vrfs.append(""); //para la global

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));
    m_siguienteVRF();
}

void ArpInfoCisco::m_siguienteVRF()
{
    if ( m_vrfsPos < m_vrfs.size()-1 )
    {
        m_vrfsPos++;
        m_vrf = m_vrfs.at( m_vrfsPos );

        if ( m_os == "IOS XR" )
            termSendText("sh arp "+ ( !m_vrf.isEmpty() ? " vrf "+m_vrf+" " : "" ) +
                         ( !m_macip.isEmpty() ? m_macip : "" ) + " | exclude Incomplete" );
        else
            termSendText("sh ip arp "+ ( !m_vrf.isEmpty() ? " vrf "+m_vrf+" " : "" ) +
                         ( !m_macip.isEmpty() ? m_macip : "" ) + " | exclude Incomplete" );        
    }
    else
        finished();
}

void ArpInfoCisco::on_term_receiveText()
{
    txt.append( term->dataReceived() );
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}.+\\w{4}\\.\\w{4}\\.\\w{4}");
        if ( !line.contains(exp) )
            continue;

        QStringList data = line.split(" ",QString::SkipEmptyParts);
        QString _ip;
        QString _mac;
        QString _vlan;
        QString _age;
        QString _interfaz;

        if ( m_os == "IOS XR" )
        {
            _ip = data.at(0).simplified();
            _mac = data.at(2).simplified();
            _age = data.at(1).simplified();
            _interfaz = data.at(5).simplified();

            QString vlan = data.at(5).simplified();
            if ( vlan.contains(QRegExp("(Vlan|BDI|BVI)")) )
                vlan.replace(QRegExp("(Vlan|BDI|BVI)"),"");
            else if ( vlan.contains(".") )
                vlan = vlan.right( vlan.size() - vlan.indexOf(".") -1 );

            _vlan = vlan;
        }
        else if ( m_os == "IOS NX" )
        {
            _ip = data.at(0).simplified();
            _mac = data.at(2).simplified();
            _vlan = data.at(3).simplified(); //TODO corregir, sacar la vlan de la interfaz
            _age = data.at(1).simplified();
        }
        else
        {
            _ip = data.at(1).simplified();
            _mac = data.at(3).simplified();
            _age = data.at(2).simplified();

            if ( data.size() >= 6 )
            {
                _interfaz = data.at(5).simplified();
                QString vlan = data.at(5).simplified();
                if ( vlan.contains(QRegExp("(Vlan|BDI)")) )
                    vlan.replace(QRegExp("(Vlan|BDI)"),"");
                else if ( vlan.contains(".") )
                    vlan = vlan.right( vlan.size() - vlan.indexOf(".") -1 );

                _vlan = vlan;
            }
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
