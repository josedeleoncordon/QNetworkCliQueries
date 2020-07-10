#include "ospfinfocisco.h"

OSPFInfoCisco::OSPFInfoCisco(QRemoteShell *terminal, QueryOpcion option):
    OSPFInfo(terminal,option)
{
    m_XRconsultaVRFs=false;
    //actualizar para neighbors, para que consulte los de las vrfs
}

OSPFInfoCisco::OSPFInfoCisco(const OSPFInfoCisco &other):
    OSPFInfo(other.term,other.m_queryoption)
{
    m_XRconsultaVRFs = other.m_XRconsultaVRFs;
}

OSPFInfoCisco::~OSPFInfoCisco()
{}

void OSPFInfoCisco::getOSPFInfo()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextNeighbors()));
    if ( m_os == "IOS XR" )
        termSendText("sh ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " neighbor");
    else
        termSendText("sh ip ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " neighbor");
}

void OSPFInfoCisco::on_term_receiveTextNeighbors()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    foreach (QString line, lines)
    {
        line = line.simplified();

        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
        if ( !line.contains(exp) )
            continue;

        SOSPFInfo oii;
        oii.datetime = QDateTime::currentDateTime();
        oii.operativo = true;
        QStringList lstColumns = line.split(" ",QString::SkipEmptyParts);

        oii.id = lstColumns.at(0).simplified();
        oii.state = lstColumns.at(2).simplified();
        oii.address = lstColumns.at( lstColumns.size()-2 ).simplified();
        oii.interfaz = estandarizarInterfaz( lstColumns.last().simplified() );

        m_lstOSPFInfo.append(oii);
    }

    if ( m_os == "IOS XR" &&
         !m_XRconsultaVRFs )
    {
        m_XRconsultaVRFs=true;
        termSendText("sh ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " vrf all neighbor");
    }
    else
    {
        m_XRconsultaVRFs=false;
        term->disconnectReceiveTextSignalConnections();
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextInterfaces()));
        if ( m_os == "IOS XR" )
            termSendText("sh ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " interface brief");
        else
            termSendText("sh ip ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " interface brief");
    }
}

void OSPFInfoCisco::on_term_receiveTextInterfaces()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QString lastArea;
    QString lastPID;

    for (QString line : lines)
    {
        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
        if ( !line.contains(exp) )
            continue;

        QStringList lstColumns = line.split(" ",QString::SkipEmptyParts);

        QString interfaz = lstColumns.at(0).simplified();
        if ( interfaz.contains(QRegExp("Vl\\d+")) )
            interfaz.replace("Vl","Vlan");
        if ( interfaz.contains(QRegExp("BV\\d+")) )
            interfaz.replace("BV","BVI");

        for ( SOSPFInfo &oii : m_lstOSPFInfo )
        {            
            if ( oii.interfaz == estandarizarInterfaz( interfaz ) )
            {                
                oii.process = lstColumns.at(1).simplified();
                oii.cost = lstColumns.at(4).simplified();
                oii.area = lstColumns.at(2).simplified();

                if ( lastArea.isEmpty() )
                    lastArea = oii.area;
                else if ( lastArea != oii.area )
                    m_abr=true;

                if ( lastPID.isEmpty() )
                    lastPID = oii.process;
                else if ( lastPID != oii.process )
                    m_asbr=true;
            }
        }
    }
    if ( m_os == "IOS XR" &&
         !m_XRconsultaVRFs )
    {
        m_XRconsultaVRFs=true;
        termSendText("sh ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " vrf all interface brief");
    }
    else
        finished();
}



