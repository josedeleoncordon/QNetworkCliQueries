#include "piminfo.h"
#include "funciones.h"

SPIMInfo::SPIMInfo(const SPIMInfo &other)
{
    interfaz = other.interfaz;
    nbrCount = other.nbrCount;
}

QDataStream& operator<<(QDataStream& out, const SPIMInfo& data)
{
    out << data.interfaz;
    out << data.nbrCount;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SPIMInfo& data)
{
    in >> data.interfaz;
    in >> data.nbrCount;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SPIMInfo> &lstDest, QList<SPIMInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SPIMInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo )
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SPIMInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SPIMInfo &dest : lstDest )
        {
            if ( origin.interfaz == dest.interfaz )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.nbrCount = origin.nbrCount;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

PIMInfo::PIMInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{}

PIMInfo::PIMInfo(const PIMInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_lstPIMNeighbors = other.m_lstPIMNeighbors;
    m_lstPimInterfaces = other.m_lstPimInterfaces;
    m_lstRouterPim = other.m_lstRouterPim;
    m_lstMulticastRouting = other.m_lstMulticastRouting;
}

PIMInfo::~PIMInfo()
{}

//void PIMInfo::getPIMNeighbors()
//{
//    connect(term,SIGNAL(receiveText(QString)),SLOT(on_term_receiveTextNeighbors(QString)));
//    termSendText("show ip pim neighbor");
//}

void PIMInfo::getPIMInterfaces()
{
    if ( m_brand != "Cisco" )
    {
        qDebug() << "PIMInfo::getPIMInterfaces()" << m_brand << "no soportado";
        finished();
        return;
    }

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextInterfaces()));
    termSendText("show ip pim interface");
}

//void PIMInfo::on_term_receiveTextNeighbors(QString str)
//{
//    txt.append(str);
//    if ( !allTextReceived() )
//        return;

//    QStringList lines = txt.split("\n");
//    foreach (QString line, lines)
//    {
//        line = line.simplified();

//        exp.setPattern("([a-zA-Z]|\\-)+\\d+(/\\d+)*(\\.\\d+)*");
//        exp.setMinimal(false);
//        if ( line.contains(exp) )
//        {
//            SPIMInfo *pi = new SPIMInfo;
//            pi->interface = estandarizarInterfaz( exp.cap(0) );
//            lstPIMNeighbors.append(pi);
//        }
//    }
//    finished();
//}

void PIMInfo::on_term_receiveTextInterfaces()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QRegExp exp("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( line.contains(exp) )
        {
            QStringList data=line.split(" ",QString::SkipEmptyParts);

            SPIMInfo pi;
            pi.interfaz = estandarizarInterfaz( data.at(1) );
            pi.nbrCount = data.at(3);
            pi.datetime = QDateTime::currentDateTime();
            pi.operativo = true;
            m_lstPimInterfaces.append(pi);
        }
    }

    if ( m_os == "IOS XR" )
    {
        //Si es XR en show pim interfaces salen interfaces que no se tienen configurados en router pim ni en multicat-routing
        //por eso consultamos el run de router pim para ver si si esta configurada

        txt.clear();
        term->disconnectReceiveTextSignalConnections();
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextRouterPim()));
        termSendText("show run router pim");
    }
    else
    {
        m_lstPIMNeighbors=m_lstPimInterfaces;
        finished();
    }
}

void PIMInfo::on_term_receiveTextRouterPim()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QRegExp exp("^interface (([a-zA-Z]|\\-)+\\d+(/\\d+)*(\\.\\d+)*)");
    QString lastInterface;
    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        //Se verifica la interfaz
        if ( line.contains(exp) )
        {
            lastInterface=estandarizarInterfaz(exp.cap(1));
            continue;
        }

        //Si despues de la interfaz hay un enable la interfaz de la consulta anterior
        if ( !lastInterface.isEmpty() &&
             line == "enable")
        {
            m_lstRouterPim.append( lastInterface );

            lastInterface.clear();
            continue;
        }
    }

    txt.clear();
    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextMulticastRouting()));
    termSendText("show run multicast-routing");
}

void PIMInfo::on_term_receiveTextMulticastRouting()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QRegExp exp("^interface (([a-zA-Z]|\\-)+\\d+(/\\d+)*(\\.\\d+)*)");
    QString lastInterface;
    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        //Se verifica la interfaz
        if ( line.contains(exp) )
        {
            lastInterface=estandarizarInterfaz(exp.cap(1));
            continue;
        }

        //Si despues de la interfaz hay un enable la interfaz de la consulta anterior
        if ( !lastInterface.isEmpty() &&
             line == "enable")
        {
            m_lstMulticastRouting.append(lastInterface);

            lastInterface.clear();
            continue;
        }
    }

    for (SPIMInfo &ii : m_lstPimInterfaces)
    {
        bool encontrado=false;
        for (QString &interfaz : m_lstRouterPim)
        {
            if ( ii.interfaz == interfaz )
            {
                m_lstPIMNeighbors.append( ii );
                encontrado=true;
                break;
            }
        }

        if ( !encontrado )
        {
            for (QString &interfaz : m_lstMulticastRouting)
            {
                if ( ii.interfaz == interfaz )
                {
                    m_lstPIMNeighbors.append( ii );
                    break;
                }
            }
        }
    }

    finished();
}

bool PIMInfo::interfaceStatus(QString interfaz, QString& txtStatusOut)
{
    for (SPIMInfo &p : m_lstPIMNeighbors)
    {
        if ( p.interfaz == interfaz)
        {
            if ( p.nbrCount == "0" )
            {
                txtStatusOut = "down";
                return false;
            }
            else
            {
                txtStatusOut = "up";
                return true;
            }
        }
    }

    //no esta configurado pim en la interfaz, regresamo ok ya que esta bien
    txtStatusOut = "No config";
    return true;
}

QDataStream& operator<<(QDataStream& out, const PIMInfo& info)
{
    out << info.m_lstPIMNeighbors;
    return out;
}

QDataStream& operator>>(QDataStream& in, PIMInfo& info)
{
    in >> info.m_lstPIMNeighbors;
    return in;
}

QDataStream& operator<<(QDataStream& out, const PIMInfo* info)
{
    out << info->m_lstPIMNeighbors;
    return out;
}

QDataStream& operator>>(QDataStream& in, PIMInfo*& info)
{
    info = new PIMInfo;
    in >> info->m_lstPIMNeighbors;
    return in;
}

QDebug operator<<(QDebug dbg, const PIMInfo &info)
{
    dbg.nospace() << "PIMInfo:\n";
    for (SPIMInfo i : info.m_lstPIMNeighbors)
        dbg.space() << i.interfaz << i.nbrCount  << i.datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
