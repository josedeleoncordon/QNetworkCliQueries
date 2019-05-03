#include "macinfohuawei.h"
#include "funciones.h"

MacInfoHuawei::MacInfoHuawei(QRemoteShell *terminal, QObject *parent):
    MacInfo(terminal,parent)
{
}

MacInfoHuawei::MacInfoHuawei(const MacInfoHuawei &other):
    MacInfo(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_mac = other.m_mac;
    m_vlan = other.m_vlan;
    foreach (SMacInfo *ii, other.m_lstMacs)
    {
        SMacInfo *ii2 = new SMacInfo(*ii);
        m_lstMacs.append(ii2);
    }
}

MacInfoHuawei::~MacInfoHuawei()
{}

void MacInfoHuawei::getMacInfo()
{
    m_mac = QueriesConfiguration::instance()->mapQueries.value("MAC_MAC");
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

        SMacInfo *mac = nullptr;
        QStringList data = line.split(" ",QString::SkipEmptyParts);
        QString m = data.at(0).simplified();
        m.replace("-",".");

        if ( m_platform.contains("ATN") )
        {
            mac = new SMacInfo;
            mac->queryParent = m_queriesParent;
            mac->datetime = QDateTime::currentDateTime();
            mac->operativo = true;
            mac->mac = m;
            mac->vlan = data.at(1).simplified();
            mac->interfaz = interfazSimplifier(data.at(4).simplified());
        }
        else if ( m_platform.contains("NE40E") )
        {
            if ( data.size() > 3 )
            {
                mac = new SMacInfo;
                mac->queryParent = m_queriesParent;
                mac->datetime = QDateTime::currentDateTime();
                mac->operativo = true;
                mac->mac = m;
                mac->vlan = data.at(2).simplified();
                mac->interfaz = interfazSimplifier(data.at(5).simplified());
            }
        }

        if ( !mac )
            continue;

        //verificando si la mac esta en la vlans permitidas, si se configuro
        QStringList vlanfilter = QueriesConfiguration::instance()->mapQueriesToList("MAC_vlansFilter");
        if ( vlanfilter.isEmpty() )
            m_lstMacs.append(mac);
        else
        {
            if ( vlanfilter.contains( mac->vlan ) )
                m_lstMacs.append(mac);
            else
                delete mac;
        }
    }

    finished();
}
