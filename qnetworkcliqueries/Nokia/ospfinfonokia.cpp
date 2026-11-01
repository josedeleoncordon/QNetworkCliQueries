#include "ospfinfonokia.h"

OSPFInfoNokia::OSPFInfoNokia(QRemoteShell *terminal, int option)
    : OSPFInfo(terminal,option)
{}

OSPFInfoNokia::OSPFInfoNokia(const OSPFInfoNokia &other):
    OSPFInfo(other.term,other.m_queryoption)
{}

OSPFInfoNokia::~OSPFInfoNokia()
{}

void OSPFInfoNokia::getOSPFInfo()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextNeighbors()));
    termSendText("show router ospf all neighbor detail");
}

void OSPFInfoNokia::on_term_receiveTextNeighbors()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    parsearVecinos(txt);

    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextNeighborsInterface()));
    termSendText("show router ospf all interface detail");
}

void OSPFInfoNokia::on_term_receiveTextNeighborsInterface()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    parsearInterfaces(txt);

    finished();
}

void OSPFInfoNokia::parsearVecinos(const QString &salida)
{
    SOSPFInfo *oii = nullptr;
    QString lastArea;
    QString lastInstance;
    for (QString line : salida.split("\n"))
    {
        line = line.simplified();

        exp.setPattern("Instance (\\S+) Neighbors");
        if (line.contains(exp,&match))
        {
            QString cap = match.captured(1);
            if (!lastInstance.isEmpty())
                if (lastInstance!=cap)
                    m_asbr=true;
            lastInstance=cap;
            lastArea.clear();
            continue;
        }

        exp.setPattern("Neighbor Rtr Id : (\\S+) Interface: (\\S+)$");
        if (line.contains(exp,&match))
        {
            SOSPFInfo oi;
            oi.datetime = QDateTime::currentDateTime();
            oi.operativo = true;
            oi.process = lastInstance;
            oi.id = match.captured(1);
            oi.interfaz = match.captured(2);
            m_lstOSPFInfo.append(oi);
            oii = &m_lstOSPFInfo[m_lstOSPFInfo.size()-1];
        }
        if (!oii) continue;

        exp.setPattern("Neighbor IP Addr : (\\S+)$");
        if (line.contains(exp,&match))
        {
            oii->remoteaddress = match.captured(1);
            continue;
        }

        exp.setPattern("Area Id : (\\S+)");
        if (line.contains(exp,&match))
        {
            QString area=match.captured(1);
            //es ABR si dentro de la misma instancia hay vecinos en mas de un area
            if (!lastArea.isEmpty() && lastArea!=area)
                m_abr=true;
            lastArea = area;
            bool ok;
            oii->area = QString::number(IP2Binario(match.captured(1)).toInt(&ok,2));
            continue;
        }

        exp.setPattern("Neighbor State : (\\S+)");
        if (line.contains(exp,&match))
        {
            oii->state = match.captured(1);
            continue;
        }
    }

    //creamos index de la interfaz para busqueda rapida para las siguientes consultas
    index.clear();
    for (int i = 0; i < m_lstOSPFInfo.size(); i++) {
        index[m_lstOSPFInfo[i].interfaz] = i;
    }

    qDebug() << "HASH";
    qDebug() << "index" << index;
}

void OSPFInfoNokia::parsearInterfaces(const QString &salida)
{
    QString lastInterface;
    for (QString line : salida.split("\n"))
    {
        line = line.simplified();
        qDebug() << "line" << line;
        qDebug() << "lastinterface" << lastInterface;

        exp.setPattern("^Interface : (\\S+)$");
        if (line.contains(exp,&match))
        {
            lastInterface=match.captured(1);
            qDebug() << "interface" << lastInterface;
            continue;
        }

        exp.setPattern("^IP Address : (\\S+)$");
        if (line.contains(exp,&match) && lastInterface=="system")
        {
            m_lstRouterIDs.append(match.captured(1));
            continue;
        }

        exp.setPattern("Oper Metric : (\\S+)");
        if (line.contains(exp,&match))
        {
            if (index.contains(lastInterface))
            {
                m_lstOSPFInfo[index.value(lastInterface)].cost = match.captured(1);
                qDebug() << "------ agregando costo" << lastInterface << index.value(lastInterface);
            }
            continue;
        }
    }
}
