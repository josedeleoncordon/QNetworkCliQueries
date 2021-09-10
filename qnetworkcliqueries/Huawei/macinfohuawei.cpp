#include "macinfohuawei.h"
#include "funciones.h"

MacInfoHuawei::MacInfoHuawei(QRemoteShell *terminal, int option):
    MacInfo(terminal,option)
{
}

MacInfoHuawei::MacInfoHuawei(const MacInfoHuawei &other):
    MacInfo(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_mac = other.m_mac;
    m_vlan = other.m_vlan;
    m_lstMacs = other.m_lstMacs;
}

MacInfoHuawei::~MacInfoHuawei()
{}

void MacInfoHuawei::getMacInfo()
{
    m_mac = m_queriesConfiguration.value("MAC_MAC",m_ip,m_os,m_conexionID);
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));
    termSendText("display mac-address");
}

void MacInfoHuawei::on_term_receiveText()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( (! line.contains("dynamic",Qt::CaseInsensitive ) &&
             ! line.contains("static",Qt::CaseInsensitive )) ||
             line.contains("drop",Qt::CaseInsensitive) )
             continue;

        QStringList data = line.split(" ",QString::SkipEmptyParts);
        QString m = data.at(0).simplified();
        m.replace("-",".");

        SMacInfo mac;
        mac.datetime = QDateTime::currentDateTime();
        mac.operativo = true;
        mac.mac = m;

        if ( data.size() == 7 )
        {
            mac.vlan = data.at(1).simplified();
            mac.interfaz = estandarizarInterfaz(data.at(4).simplified());
        }
        else if ( data.size() > 7 )
        {
            mac.vlan = data.at(2).simplified();
            mac.interfaz = estandarizarInterfaz(data.at(5).simplified());
        }

        //verificando si la mac esta en la vlans permitidas, si se configuro
        QStringList vlanfilter = m_queriesConfiguration.values("MAC_vlansFilter",m_ip,m_os,m_conexionID);
        if ( vlanfilter.isEmpty() )
            m_lstMacs.append(mac);
        else
            if ( vlanfilter.contains( mac.vlan ) )
                m_lstMacs.append(mac);
    }

    finished();
}
