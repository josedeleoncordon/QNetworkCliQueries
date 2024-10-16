#include "ospfinfohuawei.h"

OSPFInfoHuawei::OSPFInfoHuawei(QRemoteShell *terminal, int option):
    OSPFInfo(terminal,option)
{
    //actualizar para neighbors, para que consulte los de las vrfs
}

OSPFInfoHuawei::OSPFInfoHuawei(const OSPFInfoHuawei &other):
    OSPFInfo(other.term,other.m_queryoption)
{}

OSPFInfoHuawei::~OSPFInfoHuawei()
{}

void OSPFInfoHuawei::getOSPFInfo()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextNeighbors()));
    termSendText("display ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " peer brief");
}

void OSPFInfoHuawei::on_term_receiveTextNeighbors()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    QString proceso;
    QString lastArea;
    QString lastPID;
    foreach (QString line, lines)
    {
        line = line.simplified();

        exp.setPattern("OSPF Process (\\d+) with Router");
        if ( line.contains(exp,&match) )
        {
            proceso = match.captured(1);
            continue;
        }

        exp2.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
        if ( !line.contains(exp2) )
            continue;

        SOSPFInfo oii;
        oii.datetime = QDateTime::currentDateTime();
        oii.operativo = true;
        QStringList lstColumns = line.split(" ",Qt::SkipEmptyParts);

        oii.id = lstColumns.at(2).simplified();
        oii.state = lstColumns.at(3).simplified();
        oii.area = lstColumns.at(0).simplified();
        oii.interfaz = estandarizarInterfaz( lstColumns.at(1).simplified() );
        oii.process = proceso;

        if ( lastArea.isEmpty() )
            lastArea = oii.area;
        else if ( lastArea != oii.area )
            m_abr=true;

        if ( lastPID.isEmpty() )
            lastPID = oii.process;
        else if ( lastPID != oii.process )
            m_asbr=true;

        m_lstOSPFInfo.append(oii);
    }

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextInterfaces()));
    termSendText("display ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " interface");
}

void OSPFInfoHuawei::on_term_receiveTextInterfaces()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    foreach (QString line, lines)
    {
        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
        if ( !line.contains(exp,&match) )
            continue;

        QStringList lstColumns = line.split(" ",Qt::SkipEmptyParts);

        for ( SOSPFInfo &oii : m_lstOSPFInfo )
        {
            if ( oii.interfaz == estandarizarInterfaz( lstColumns.at(0).simplified() ) )
            {
                oii.cost = lstColumns.at(4).simplified();
                oii.address = lstColumns.at(1).simplified();
            }
        }
    }
    finished();
}



