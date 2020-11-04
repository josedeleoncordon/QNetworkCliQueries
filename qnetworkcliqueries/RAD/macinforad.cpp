#include "macinforad.h"
#include "funciones.h"

MacInfoRAD::MacInfoRAD(QRemoteShell *terminal, int option):
    MacInfo(terminal,option)
{
    etxTimer = nullptr;
    menuintentos=0;
}

MacInfoRAD::MacInfoRAD(const MacInfoRAD &other):
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

MacInfoRAD::~MacInfoRAD()
{
    if ( etxTimer )
        delete etxTimer;
}

void MacInfoRAD::getMacInfo()
{
	m_mac = m_queriesConfiguration.value("MAC_MAC",m_ip,m_os);
    if ( m_platform == "ETX-1" )
    {
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextETX1()));
        termSendText("show configure bridge 1 mac-address-table all");
    }
    else if ( m_platform == "ETX-102/201/202" ||
              m_platform == "ETX-201" ||
              m_platform == "ETX-102" ||
              m_platform == "ETX-1300" )
    {
        if ( m_platform == "ETX-1300" )
            ext201menus << "Monitoring" << "Status" << "Bridge" << "View MAC Table" << "View MAC Table";
        else
            ext201menus << "Monitoring" << "Bridge" << "MAC Table" << "View MAC Table";

        pageMacCounter=0;
        totalMacs=-1;
        etxAgregadaMAC=false;
        etxTimer = new QTimer;
        etxTimer->setSingleShot(true);
        etxTimer->setInterval(3000);

        connect(etxTimer,SIGNAL(timeout()),SLOT(on_ext_timer_timeout()));
        connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveTextETX1002001300()));

        //actualizamos el menu
        termSendText("");
    }
    else
        finished();
}

void MacInfoRAD::on_term_receiveTextETX1()
{
    txt.append(term->dataReceived());
    if ( !allTextReceived() )
        return;

    QRegExp expmac("(\\d+) *(\\S{17}) +(\\d+)");

    QStringList lines = txt.split("\n");
    foreach (QString line, lines)
    {
        line = line.simplified();

        if ( line.contains( expmac ) )
        {
            QString mac = expmac.cap(2);

            mac.replace("-","");
            mac.insert(4,".");
            mac.insert(9,".");

            SMacInfo m;
            m.datetime = QDateTime::currentDateTime();
            m.operativo = true;
            m.mac = mac.toLower();
            m.interfaz = expmac.cap(3);
            m.vlan = expmac.cap(1);

            //verificando si la mac esta en la vlans permitidas, si se configuro
			QStringList vlanfilter = m_queriesConfiguration.values("MAC_vlansFilter",m_ip,m_os);
            if ( vlanfilter.isEmpty() )
                m_lstMacs.append(m);
            else
            {
                if ( vlanfilter.contains( m.vlan ) )
                    m_lstMacs.append(m);
            }
        }
    }
    finished();
}

void MacInfoRAD::on_term_receiveTextETX1002001300()
{
    emit working();

    QString &str = term->dataReceived();

    txt.append(str);

    if ( ext201menus.size() > 0 )
    {
        //navegando por los menus
        QString menu = ext201menus.first();

//        qDebug() << "\n\nbuscando el menu:" << menu << "en el texto" << txt;

        exp.setPattern("(\\d+)\\. "+menu);
        if ( txt.contains( exp ) )
        {
            ext201menus.removeFirst();
            QString este = exp.cap(1);
            txt.clear();
            menuintentos=0;

            termSendText( este );
        }
        else
        {
            if ( menuintentos < 5 )
            {
                menuintentos++;
                etxTimer->start();
            }
            else
            {
                etxTimer->stop();
                finished();
            }
        }
    }
    else
    {
        //en la parte de abajo de la pagina indica la cantidad de macs que conoce el etx
        if ( totalMacs == -1 )
        {
            exp.setPattern("(\\d+) entries in range out of");
            QRegExp exp2("Total (\\d+) MAC Entries");

            if ( txt.contains( exp ) )
                totalMacs = exp.cap(1).toInt();
            else if ( txt.contains( exp2 ) )
                totalMacs = exp2.cap(1).toInt();

            if ( totalMacs == 0 )
            {
                etxTimer->stop();
                finished();
                return;
            }
        }

        qDebug() << "total de MACs" << totalMacs;

        //recorriendo las lineas
        QRegExp expmac("(\\w{12}) +Learned +(User \\d+|Network)+ +(\\d+)"); //etx-202 102
        QRegExp expmac2("\\d+ +(\\d+) +((\\w|\\-)+) +(\\w+) +Dynamic"); //etx-1300
        QStringList data = txt.split("\r",QString::SkipEmptyParts);
        foreach (QString line, data)
        {
            line = line.simplified();

            qDebug() << "\nbuscando mac en" << line;

            bool encontradamac = false;
            QString iface;
            QString vlan;
            QString mac;
            if ( line.contains(expmac) ) //etx-202 102
            {
                mac = expmac.cap(1);
                mac.insert(4,".");
                mac.insert(9,".");
                mac = mac.toLower();
                iface = expmac.cap(2);
                vlan = expmac.cap(3);

                pageMacCounter++;
                encontradamac=true;
            }
            else if ( line.contains(expmac2) ) //etx-1300
            {
                mac = expmac2.cap(2);

                qDebug() << "todo" << expmac2.cap(0);
                qDebug() << "mac" << mac;

                mac.replace("-","");
                mac.insert(4,".");
                mac.insert(9,".");
                mac = mac.toLower();
                iface = expmac2.cap(4);
                vlan = expmac2.cap(1);

                qDebug() << "encontrada la mac" << mac << iface << vlan;

                pageMacCounter++;
                encontradamac=true;
            }

            if ( encontradamac )
            {
                //buscando si ya existe la mac
                bool encontrado=false;
                for (SMacInfo &m : m_lstMacs)
                {
                    if ( m.mac == mac &&
                         m.vlan == vlan )
                    {
                        encontrado=true;
                        break;
                    }
                }

                if ( !encontrado )
                {
                    //agregamos esta mac ya que es nueva
                    SMacInfo m;
                    m.datetime = QDateTime::currentDateTime();
                    m.mac = mac;
                    m.interfaz = iface;
                    m.vlan = vlan;
                    m_lstMacs.append( m );
                    etxAgregadaMAC=true;
                }
            }
        }        

        //si la la cantidad de macs que tenemos es igual a la cantidad que reporte el etx terminamos.
        if ( m_lstMacs.size() >= totalMacs && totalMacs != -1 )
        {
            etxTimer->stop();
            finished();
            return;
        }

        //si no se ha terminado pasamos a la siguiente pagina
        //13 son las macs que muestra el etx201-102, por cada pagina
        //10 son las macs que muestra el etx1300 por cada pagina
        if ( ((m_platform == "ETX-102/201/202" ||
               m_platform == "ETX-201" ||
               m_platform == "ETX-102") &&
              pageMacCounter >= 13 ) ||
             ( m_platform == "ETX-1300" &&
               pageMacCounter >= 10) )
        {
            //leimos todas las lineas de la pagina

            //aun no consultamos todas las paginas

            if ( etxAgregadaMAC )
            {
                //se agregaron macs en la pagina
                //ETX-201 muestra 13 mac por pagina, pasamos a la siguiente
                //enviando ctrl+D (ascii 4) para que baje
                //ETX-1300 muestra 10 mac por pagina, pasamosa a la siguiente
                //enviando fecla para abajo para que baje (vt100 ^[B)
                pageMacCounter=0;
                if ( m_platform == "ETX-1300" )
                {
                    const char c[3] = { 27, 91 ,66 };
                    QByteArray a(c, 3);
                    term->sendData( a );
                }
                else
                    term->sendData( QByteArray(1,4) );
            }
            else
                //ya leimos las lineas y no se agrego ninguna, significa que no hay nuevos porque estamos en la ultima pagina
                //si llegamos aca es porq totalMacs != lstMacs.size(), es porque el ETX cambio la cantidad de MACs
                //aprendidas y ya no seria posible igualarlas a menos que se empezara desde cero.
                finished();
        }
        else
            //no se han leido todas las lineas, empezamos el reloj para actualizar la pagina
            //por si no se termina de recibir todo el texto.
            etxTimer->start();
    }
}

void MacInfoRAD::on_ext_timer_timeout()
{
    //actualizamos la pagina y reiniciamos contadores
    etxAgregadaMAC=false;
    pageMacCounter=0;
    termSendText("\r");
}
