#include "portchannelsinfo.h"
#include "funciones.h"

SPortChannel::SPortChannel(const SPortChannel &other)
{
    interfaz = other.interfaz;
    group = other.group;
}

QDataStream& operator<<(QDataStream& out, const SPortChannel* data)
{
    out << data->group;
    out << data->interfaz;
    //infobase
    out << data->datetime;
    out << data->operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SPortChannel*& data)
{
    data = new SPortChannel;
    in >> data->group;
    in >> data->interfaz;
    //infobase
    in >> data->datetime;
    in >> data->operativo;
    return in;
}

void updateInfoList(QList<SPortChannel *> &lstDest, QList<SPortChannel *> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias
    for ( int c=0; c<lstDest.size(); )
    {
        SPortChannel *dest = lstDest.at(c);
        if ( dest->datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest->operativo )
             lstDest.removeAt( c );
        else
        {
            dest->operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    foreach ( SPortChannel *origin, lstOrigin )
    {
        bool encontrado=false;
        foreach ( SPortChannel *dest, lstDest )
        {
            if ( origin->interfaz == dest->interfaz )
            {
                //Si se encontro, actualizamos los datos
                dest->datetime = origin->datetime;
                dest->operativo = true;
                dest->group = origin->group;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

PortChannelsInfo::PortChannelsInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{}

PortChannelsInfo::PortChannelsInfo(const PortChannelsInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    foreach (SPortChannel *ii, other.lstInfo)
    {
        SPortChannel *ii2 = new SPortChannel(*ii);
        lstInfo.append(ii2);
    }
}

PortChannelsInfo::~PortChannelsInfo()
{
    qDeleteAll(lstInfo);
}

void PortChannelsInfo::getPortChannelsInfo()
{    
    if ( m_brand == "Cisco" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));
        if ( m_os == "IOS NX" )
            termSendText("show port-channel summary");
        if ( m_os == "IOS XR" )
            termSendText("show bundle");
        else
            termSendText("show etherchannel summary");
    }
    else if ( m_brand == "Huawei" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextHuawei()));
        termSendText("display eth-trunk");
    }
    else
    {
        finished();
        return;
    }
}

void PortChannelsInfo::on_term_receiveText()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    QString lastBundle;
    QRegExp expGroup;

    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( m_os == "IOS XR" )
        {
            expGroup.setPattern("^Bundle-Ether(\\d+)$");
            if ( line.contains(expGroup) )
            {
                lastBundle = expGroup.cap(1);
                continue;
            }

            exp.setPattern("\\w+\\d/\\d.+Active");
            if ( line.contains(exp) )
            {
                SPortChannel *pc = new SPortChannel;
                pc->queryParent = m_queriesParent;
                pc->interfaz = estandarizarInterfaz( line.split(" ",QString::SkipEmptyParts).at(0).simplified() );
                pc->group = lastBundle;
                pc->datetime = QDateTime::currentDateTime();
                pc->operativo = true;
                lstInfo.append( pc );
                continue;
            }
        }
        else
        {
            int c=0;

            expGroup.setPattern("Po(\\d+)");
            if ( line.contains(expGroup) )
            {
                lastBundle = expGroup.cap(1);

                //Si la linea contiene la interfaz Po1 entonces se empieza en la cuarta o quinta columna
                //Si la linea no contiene la interfaz se comienza en la primera

//                Group  Port-channel  Protocol    Ports
//                ------+-------------+-----------+-----------------------------------------------
//                1      Po1(SU)         LACP      Fa0/14(P)   Fa0/22(P)   Fa0/23(P)
//                                                 Fa0/24(P)
                if ( m_os == "IOS NX" )
                    c = 4;
                else if ( line.split(" ").size() == 3 )
                    //hay un problema en los ASR9:
                    //1Po1(RU)LACP Gi0/3/0(bndl) Gi0/3/4(bndl)
                    //si se encuentra una linea con solo 3 columanas empezamos desde la segunda
                    c = 1;
                else
                    c = 3;
            }

            exp.setPattern("\\d+\\((P|bndl)\\)");
            if ( line.contains(exp) && !lastBundle.isEmpty() )
            {
                QStringList data = line.split(" ",QString::SkipEmptyParts);

                QString interfaz;
                for ( ; c<data.size(); c++ )
                {
                    interfaz = data.at(c).simplified();
                    exp.setPattern("\\((P|bndl)\\)");
                    if ( interfaz.contains(exp) )
                    {
                        interfaz.replace(exp,"");
                        interfaz = estandarizarInterfaz( interfaz );

                        SPortChannel *pc=new SPortChannel;
                        pc->queryParent = m_queriesParent;
                        pc->interfaz = interfaz;
                        pc->group = lastBundle;
                        pc->datetime = QDateTime::currentDateTime();
                        pc->operativo = true;
                        lstInfo.append( pc );
                    }
                }
            }
        }
    }
    finished();
}

void PortChannelsInfo::on_term_receiveTextHuawei()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lines = txt.split("\n");

    QString lastBundle;
    for (QString line : lines)
    {
        line = line.simplified();

        //LAG ID: 1                   WorkingMode: STATIC
        exp.setPattern("LAG ID: (\\d+) +WorkingMode.+");
        if ( line.contains(exp) )
        {
            lastBundle = exp.cap(1);
            continue;
        }

        exp.setPattern("([a-zA-Z]+\\d+/\\d+(/\\d+)*) +\\d+ +\\w+\\-\\w+\\-\\w+");
        if ( line.contains(exp) )
        {
            SPortChannel *pc=new SPortChannel;
            pc->queryParent = m_queriesParent;
            pc->interfaz = estandarizarInterfaz(exp.cap(1));
            pc->group = lastBundle;
            pc->datetime = QDateTime::currentDateTime();
            pc->operativo = true;
            lstInfo.append( pc );
        }
    }
    finished();
}

QStringList PortChannelsInfo::portChannelToInterfaces(QString pcInterface)
{
    return interfacePortChannelToInterfaces(lstInfo,pcInterface);
}

QString PortChannelsInfo::interfaceToPortChannel(QString interface)
{
    return interfaceToPortChannelInterface(lstInfo,interface,m_platform);
}

QDataStream& operator<<(QDataStream& out, const PortChannelsInfo* info)
{
    out << info->lstInfo;
    return out;
}

QDataStream& operator>>(QDataStream& in, PortChannelsInfo*& info)
{
    info =new PortChannelsInfo(nullptr,nullptr);
    in >> info->lstInfo;
    return in;
}

QDebug operator<<(QDebug dbg, const PortChannelsInfo &info)
{
    dbg.nospace() << "PortChannelsInfo:\n";
    foreach (SPortChannel *i, info.lstInfo)
        dbg.space() << i->interfaz << i->group << i->datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
