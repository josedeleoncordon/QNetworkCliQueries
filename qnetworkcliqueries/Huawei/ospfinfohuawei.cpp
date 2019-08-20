#include "ospfinfohuawei.h"

OSPFInfoHuawei::OSPFInfoHuawei(QRemoteShell *terminal, QObject *parent):
    OSPFInfo(terminal,parent)
{
    //actualizar para neighbors, para que consulte los de las vrfs
}

OSPFInfoHuawei::OSPFInfoHuawei(const OSPFInfoHuawei &other):
    OSPFInfo(other.term,other.parent())
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
    foreach (QString line, lines)
    {
        line = line.simplified();

        exp.setPattern("OSPF Process (\\d+) with Router");
        if ( line.contains(exp) )
        {
            proceso = exp.cap(1);
            continue;
        }

        exp2.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
        if ( !line.contains(exp2) )
            continue;

        SOSPFInfo *oii = new SOSPFInfo;
        oii->queryParent = m_queriesParent;
        oii->datetime = QDateTime::currentDateTime();
        oii->operativo = true;
        QStringList lstColumns = line.split(" ",QString::SkipEmptyParts);

        oii->id = lstColumns.at(2).simplified();
        oii->state = lstColumns.at(3).simplified();
        oii->area = lstColumns.at(0).simplified();
        oii->interfaz = estandarizarInterfaz( lstColumns.at(1).simplified() );
        oii->process = proceso;

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
        if ( !line.contains(exp) )
            continue;

        QStringList lstColumns = line.split(" ",QString::SkipEmptyParts);

        for ( SOSPFInfo *oii : m_lstOSPFInfo )
        {
            if ( oii->interfaz == estandarizarInterfaz( lstColumns.at(0).simplified() ) )
            {
                oii->cost = lstColumns.at(4).simplified();
                oii->address = lstColumns.at(1).simplified();
            }
        }
    }
    finished();
}



