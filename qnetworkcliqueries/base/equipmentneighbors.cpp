#include "equipmentneighbors.h"
#include "funciones.h"

QDataStream& operator<<(QDataStream& out, const SEquipmentNeighborsInfo& data)
{
    out << data.nombreequipo;
    out << data.ip;
    out << data.interfazestesalida;
    out << data.interfazotroentrada;
    out << data.plataforma;
    out << data.lstEquipos;
    //infobase
    out << data.datetime;
    out << data.operativo;
    return out;
}

QDataStream& operator>>(QDataStream& in, SEquipmentNeighborsInfo& data)
{
    in >> data.nombreequipo;
    in >> data.ip;
    in >> data.interfazestesalida;
    in >> data.interfazotroentrada;
    in >> data.plataforma;
    in >> data.lstEquipos;
    //infobase
    in >> data.datetime;
    in >> data.operativo;
    return in;
}

void updateInfoList(QList<SEquipmentNeighborsInfo> &lstDest, QList<SEquipmentNeighborsInfo> &lstOrigin )
{
    //actualiza la lista anterior con la información de la nueva
    //origen = nuevo
    //destino = anterior

    //borramos los datos anteriores que tengan mas de 30 dias

    for ( int c=0; c<lstDest.size(); )
    {
        SEquipmentNeighborsInfo &dest = lstDest[c];
        if ( dest.datetime.date().daysTo( QDate::currentDate() )  > 30 && !dest.operativo)
             lstDest.removeAt( c );
        else
        {
            dest.operativo=false; //se marca como no operativo, si en la consulta nueva esta se volvera a poner en true
            c++;
        }
    }

    //actualizamos los datos del anterior con la nueva, se agrega la nueva
    for ( SEquipmentNeighborsInfo &origin : lstOrigin )
    {
        bool encontrado=false;
        for ( SEquipmentNeighborsInfo &dest : lstDest )
        {
            //si ya esta operativo es por que en un ciclo anterior se actualizo
            if ( dest.operativo )
                continue;

            if ( origin.interfazestesalida == dest.interfazestesalida )
            {
                //Si se encontro, actualizamos los datos
                dest.datetime = origin.datetime;
                dest.operativo = true;
                dest.interfazestesalida = origin.interfazestesalida;
                dest.interfazotroentrada = origin.interfazotroentrada;
                dest.ip = origin.ip;
                dest.nombreequipo = origin.nombreequipo;
                dest.plataforma = origin.plataforma;
                encontrado=true;
                break;
            }
        }
        if ( !encontrado )
            //no se encontro, se agrega
            lstDest.append( origin );
    }
}

EquipmentNeighborsInfo::EquipmentNeighborsInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{}

EquipmentNeighborsInfo::EquipmentNeighborsInfo(const EquipmentNeighborsInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_lstEquipos = other.m_lstEquipos;
    m_queryName = other.m_queryName;
}

EquipmentNeighborsInfo::~EquipmentNeighborsInfo()
{}

void EquipmentNeighborsInfo::addEquipmentNeighborsInfo(SEquipmentNeighborsInfo i) { m_lstEquipos.append( i ); }

void EquipmentNeighborsInfo::getEquipmentNeighborsInfo()
{
    if ( m_brand == "Cisco" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_CDP()));
        termSendText("show cdp neighbors detail");
    }
    else if ( m_brand == "Huawei" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_LLDP_Huawei()));
        termSendText("display lldp neighbor");
    }
    else
        finished();
}

void EquipmentNeighborsInfo::on_term_receiveText_CDP()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lstEntries = txt.split("-------------------------");

    foreach (QString entry, lstEntries)
    {
        if ( entry.contains("show cdp neighbors") )
            continue;                       

        QStringList lstValues = entry.split("\n");
        SEquipmentNeighborsInfo *nuevo=nullptr;
        foreach (QString line, lstValues)
        {
            line = line.simplified();

            exp.setPattern("Device ID: *.+");
            if ( line.contains(exp,&match) )
            {
                line.replace(" ","");
                QStringList lst = line.split(":");
                QString nombre = lst.at(1);

                nuevo = new SEquipmentNeighborsInfo;
                nuevo->nombreequipo = nombre.simplified();
                nuevo->datetime = QDateTime::currentDateTime();
                nuevo->operativo = true;
            }

            if ( line.contains("Platform:") && nuevo )
            {
                QStringList lst = line.split(",");
                QString primero = lst.at(0);
                nuevo->plataforma = primero.split(":").at(1).simplified();
            }

            if ( line.contains("Interface:") && nuevo )
            {
                QStringList lst = line.split(",");

                //IOS
                if ( lst.size() == 2 )
                {
                    QString primero = lst.at(0);
                    primero.replace(" ","");
                    QString segundo = lst.at(1);
                    segundo.replace(" ","");

                    nuevo->interfazestesalida = estandarizarInterfaz( primero.split(":").at(1).simplified() );
                    nuevo->interfazotroentrada = estandarizarInterfaz( segundo.split(":").at(1).simplified() );
                }

                //XR
                if (  lst.size() == 1 )
                {
                    QString primero = lst.at(0);
                    primero.replace(" ","");
                    nuevo->interfazestesalida = estandarizarInterfaz( primero.split(":").at(1).simplified() );
                }
            }

            //XR
            if ( line.contains("Port ID (outgoing port):") && nuevo  )
            {
                if ( nuevo->interfazotroentrada.isEmpty() )
                {
                    line.replace(" ","");
                    QStringList lst = line.split(":");
                    nuevo->interfazotroentrada = estandarizarInterfaz( lst.at(1).simplified() );
                }
            }

            exp.setPattern("IP(v4)* (a|A)ddress:");
            if ( line.contains(exp,&match) && nuevo  )
            {
                if ( nuevo->ip.isEmpty() )
                {
                    line.replace(" ","");
                    QStringList lst = line.split(":");
                    nuevo->ip = lst.at(1).simplified();
                }
            }
        }
        if ( nuevo )
        {
            m_lstEquipos.append(*nuevo);
            delete nuevo;
        }
    }

    //se consulta ahora lldp, algunas versiones de ios no muestra la interfaz local por lo que hay que hacer dos consultas
    term->disconnectReceiveTextSignalConnections();
    if ( m_os == "IOS XR" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_LLDP_Cisco_detail()));
        termSendText("show lldp neighbors detail");
    }
    else
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_LLDP_Cisco()));
        termSendText("show lldp neighbors");
    }
}

void EquipmentNeighborsInfo::on_term_receiveText_LLDP_Cisco()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    exp.setPattern("(.+) +\\d+ +(B|R|T|C|W|P|S|O|,)+ +.+");
    int con=-1;
    foreach (QString line, txt.split("\n"))
    {
        if ( line.contains(exp,&match) )
        {
            //Device ID           Local Intf     Hold-time  Capability      Port ID
            //GMIXGTEMN1D1102A25ENTe1/1          120        B,R             GigabitEthernet0/2/24
            //GNCYGTEMN1D1103A40EITe2/2          120        B,R             GigabitEthernet2/0/1
            con++;
            QStringList col = line.split("  ",Qt::SkipEmptyParts);
            if ( col.size() == 5  )
                mapInterfazLocales.insert(col.at(0)+"_"+QString::number(con),estandarizarInterfaz(col.at(1)));            
            if ( col.size() == 4 )
            {
                //nombre de equipo muy largo pegado a la interfaz como se ve arriba
                QString equipo = col.at(0).left(20);
                QString interfaz = col.at(0);
                interfaz.replace(equipo,"");
                mapInterfazLocales.insert(equipo+"_"+QString::number(con),estandarizarInterfaz(interfaz));
            }
        }
    }

    //se consulta ahora lldp detail
    term->disconnectReceiveTextSignalConnections();
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText_LLDP_Cisco_detail()));
    termSendText("show lldp neighbors detail");
}

void EquipmentNeighborsInfo::on_term_receiveText_LLDP_Cisco_detail()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QStringList lstEntries = txt.split("-------------------------");

    int con=-1;
    foreach (QString entry, lstEntries)
    {
        if ( entry.contains("show lldp neighbors") )
            continue;

        QStringList lstValues = entry.split("\n",Qt::SkipEmptyParts);
        SEquipmentNeighborsInfo *nuevo=nullptr;
        QString localint;
        QString otroint;
        bool systemdescriptiontxt=false; //cuando se ve este texto significa que la siguiente linea contiene la plataforma

        foreach (QString line, lstValues)
        {
            line = line.simplified();

            exp.setPattern("Local (Intf|Interface): (.+)$");
            if ( line.contains(exp,&match) )
            {
                localint = estandarizarInterfaz(match.captured(2).simplified());
                continue;
            }

            exp.setPattern("Port id: (.+)$");
            if ( line.contains(exp,&match) )
            {
                otroint = estandarizarInterfaz(match.captured(1).simplified());
                continue;
            }

            exp.setPattern("System Name: *.+$");
            if ( line.contains(exp,&match) )
            {
                con++;
                line.replace(" ","");
                QStringList lst = line.split(":");

                nuevo = new SEquipmentNeighborsInfo;
                nuevo->nombreequipo = lst.at(1).simplified();
                nuevo->interfazotroentrada = otroint;

                if ( !localint.isEmpty() )
                    nuevo->interfazestesalida = localint;
                else
                    nuevo->interfazestesalida = mapInterfazLocales.value(simplicateName(nuevo->nombreequipo.left(20))+
                                                                         "_"+
                                                                         QString::number(con));                

                nuevo->datetime = QDateTime::currentDateTime();
                nuevo->operativo = true;
                continue;
            }

            if ( !nuevo )
                continue;

            if ( line.contains("System Description:") )
            {
                systemdescriptiontxt=true;
                continue;
            }

            if ( systemdescriptiontxt )
            {
                QRegularExpression exp3("\\((\\s|\\w|&|_|-){4,}\\)");
                if ( line.contains(exp3,&match) )
                {
                    nuevo->plataforma = match.captured(0).replace("(","").replace(")","");
                    systemdescriptiontxt=false;
                    continue;
                }
            }

            exp.setPattern("IP(v4)*( address)*: (\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})$");
            if ( line.contains(exp,&match) )
            {
                nuevo->ip = match.captured(3).simplified();
                continue;
            }
        }
        if ( nuevo )
        {
            if ( !nuevo->interfazestesalida.isEmpty() )
            {
                bool encontrado = false;
                for (SEquipmentNeighborsInfo &vecino : m_lstEquipos)
                {
                    if ( vecino.nombreequipo == nuevo->nombreequipo &&
                         vecino.interfazestesalida == nuevo->interfazestesalida )
                    {
                        encontrado=true;
                        break;
                    }
                }
                if ( !encontrado )
                    m_lstEquipos.append(*nuevo);
                else
                    delete nuevo;
            }
            else
                delete nuevo;
        }
    }
    finished();
}

void EquipmentNeighborsInfo::on_term_receiveText_LLDP_Huawei()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    bool description=false;
    QString descriptionmie;

    SEquipmentNeighborsInfo *nuevo=nullptr;
    QString localint;
    foreach (QString line, txt.split("\n",Qt::SkipEmptyParts))
    {
        line = line.simplified();

        exp.setPattern("([0-9a-zA-Z]+[0-9](/[0-9]*)+) has \\d+ neighbor");
        if ( line.contains(exp,&match) )
        {
            localint = match.captured(1).simplified();
            continue;
        }

        if ( line.contains(QRegularExpression("Neighbor \\d+:")) ||
             line.contains(QRegularExpression("Neighbor index *:\\d+")))
        {            
            SEquipmentNeighborsInfo e;
            e.interfazestesalida = estandarizarInterfaz(localint);
            e.datetime = QDateTime::currentDateTime();
            e.operativo = true;
            m_lstEquipos.append(e);
            nuevo = &e;
            continue;
        }

        if ( !nuevo )
            continue;

        exp.setPattern("(PortId|Port ID) *: *(.+)$");
        if ( line.contains(exp,&match) )
        {
            nuevo->interfazotroentrada = estandarizarInterfaz(match.captured(2).simplified());
            continue;
        }

        exp.setPattern("(SysName|System name) *: *(.+)$");
        if ( line.contains(exp,&match) )
        {
            nuevo->nombreequipo = match.captured(2).simplified();
            continue;
        }

        if ( line.contains(QRegularExpression("(SysDesc|System description) *: *")) )
        {
            description=true;
            QStringList data = line.split(":");
            if ( data.size() > 1 )
                descriptionmie = data.at(1).simplified();
        }

        exp.setPattern("Management address *:");
        if ( line.contains(exp,&match) )
        {
            description=false;
            if ( nuevo->plataforma.isEmpty() ) nuevo->plataforma = descriptionmie;

            nuevo->ip = line.split(" ",Qt::SkipEmptyParts).last().simplified().replace(":","");
            continue;
        }

        if ( description )
        {
            QRegularExpression exp3("\\((\\s|\\w|&|_|-){4,}\\)");
            if ( line.contains(exp3,&match) )
            {
                nuevo->plataforma = match.captured(0).replace("(","").replace(")","").simplified();
                description=false;
            }                            
            continue;        
        }
    }

    finished();
}

QList<SEquipmentNeighborsInfo*> EquipmentNeighborsInfo::equipmentNeighborsInfoFromInterface(QString interfaz)
{
    QList<SEquipmentNeighborsInfo*> lst;
    for (SEquipmentNeighborsInfo &cdp : m_lstEquipos)
    {
        if ( cdp.interfazestesalida == interfaz )
            lst.append(&cdp);
    }
    return lst;
}

QDataStream& operator<<(QDataStream& out, const EquipmentNeighborsInfo& info)
{
    out << info.m_lstEquipos;
    out << info.m_queryoption;
    out << info.m_queryName;
    return out;
}

QDataStream& operator>>(QDataStream& in, EquipmentNeighborsInfo& info)
{
    in >> info.m_lstEquipos;
    in >> info.m_queryoption;
    in >> info.m_queryName;
    return in;
}

QDataStream& operator<<(QDataStream& out, const EquipmentNeighborsInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, EquipmentNeighborsInfo*& info)
{
    info = new EquipmentNeighborsInfo;
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const EquipmentNeighborsInfo &info)
{
    dbg.nospace() << "EquipmentNeighborsInfo:\n";
    for (SEquipmentNeighborsInfo i : info.m_lstEquipos)
        dbg.space() << i.nombreequipo << i.ip << i.plataforma << i.interfazestesalida
                    << i.interfazotroentrada << i.operativo << i.datetime.toString("yyyy-MM-dd_hh:mm:ss") << "\n";

    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
