#include "ospfinfoipi.h"

OSPFInfoIPI::OSPFInfoIPI(QRemoteShell *terminal, int option):
    OSPFInfo(terminal,option)
{}

OSPFInfoIPI::OSPFInfoIPI(const OSPFInfoIPI &other):
    OSPFInfo(other.term,other.m_queryoption)
{
}

OSPFInfoIPI::~OSPFInfoIPI()
{}

void OSPFInfoIPI::getOSPFInfo()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextNeighbors()));
    termSendText("sh ip ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " neighbor");
}

void OSPFInfoIPI::on_term_receiveTextNeighbors()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    foreach (QString line, lines)
    {
        line = line.simplified();

        exp.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
        if ( !line.contains(exp,&match) )
            continue;

        SOSPFInfo oii;
        oii.datetime = QDateTime::currentDateTime();
        oii.operativo = true;
        QStringList lstColumns = line.split(" ",Qt::SkipEmptyParts);

        oii.id = lstColumns.at(0).simplified();
        oii.state = lstColumns.at(2).simplified();
        oii.remoteaddress = lstColumns.at( lstColumns.size()-3 ).simplified();
        oii.interfaz = estandarizarInterfaz( lstColumns.at( lstColumns.size()-2 ).simplified() );

        m_lstOSPFInfo.append(oii);
    }


    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextInterfaces()));
    termSendText("sh ip ospf "+ (m_process > 0 ? QString::number(m_process) : "") + " interface brief");
}

void OSPFInfoIPI::on_term_receiveTextInterfaces()
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
        if ( !line.contains(exp,&match) )
            continue;

        QStringList lstColumns = line.split(" ",Qt::SkipEmptyParts);

        QString interfaz = lstColumns.at(0).simplified();
        if ( interfaz.contains(QRegularExpression("Vl\\d+")) )
            interfaz.replace("Vl","Vlan");
        if ( interfaz.contains(QRegularExpression("BV\\d+")) )
            interfaz.replace("BV","BVI");

        for ( SOSPFInfo &oii : m_lstOSPFInfo )
        {
            if ( oii.interfaz == estandarizarInterfaz( interfaz ) )
            {
                bool ok;
                oii.process = lstColumns.at(1).simplified();
                oii.cost = lstColumns.at(4).simplified();
                oii.area = QString::number(IP2Binario(lstColumns.at(2).simplified()).toInt(&ok,2));

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

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextIRun()));
    termSendText("sh run | i ospf router-id");
}

void OSPFInfoIPI::on_term_receiveTextIRun()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");
    QRegularExpression exp(" router-id (\\S+)");
    QRegularExpressionMatch match;
    for (QString line : lines)
    {
        if ( line.contains(exp,&match) )
            m_lstRouterIDs.append(match.captured(1));
    }
    emit finished();
}