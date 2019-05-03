#include "puertosacceso.h"
#include "funciones.h"

QList<MacBuscando*> buscarPuertosAcceso(QList<Queries*> lstARP,
                                        QList<Queries*> lstMACs,
                                        QString pais,
                                        QString debugMacIP,
                                        bool segundoEquipo)
{
    QList<MacBuscando*> lstARPsBuscar;
    QStringList lstVlansDeARP;
    QStringList lstMacsPE;

    //buscamos las macs de los PE
    foreach ( Queries *qry , lstARP)
    {
        foreach ( SIpInfo* sip , qry->arpsInfo())
        {
            if ( sip->age == "-" )
                lstMacsPE.append( sip->mac );
        }
    }    

    //creamos el listado de las MACs a buscar
    foreach ( Queries *qry , lstARP)
    {                
        qDebug() << "111" << qry->hostName() << qry->platform();

        foreach ( SIpInfo* sip , qry->arpsInfo())
        {
            //permitimos solo la mac o ip si se especifico
            if ( !debugMacIP.isEmpty() )
            {
                if ( !lstARPsBuscar.isEmpty() )
                    break;
                else if ( sip->mac != debugMacIP && sip->ip != debugMacIP )
                    continue;
            }

            //No nos interesa las macs de HSRP
            if ( sip->mac.contains("0000.0c07.ac") )
                continue;

            //que no sea una mac de PE
            if ( lstMacsPE.contains( sip->mac ) )
                continue;

            //verificamos si ya se habia agregado
            bool encontrado=false;
            foreach (MacBuscando *mb, lstARPsBuscar)
            {
                if ( mb->ip == sip->ip &&
                     mb->vlan == sip->vlan )
                {
                    //ya se habia agregado
                    encontrado=true;
                    if ( sip->datetime.date() > mb->datetime.date() )
                    {
                        //se actualiza ya que esta es información mas nueva
                        mb->mac = sip->mac;
                        mb->PE = qry->hostName();
                        mb->vrf = sip->vrf;
                    }
                    break;
                }
            }
            if ( !encontrado )
            {                
                //se agrega ya que no se encuentra
                MacBuscando *m = new MacBuscando;
                m->mac = sip->mac;
                m->vlan = sip->vlan;
                m->ip = sip->ip;
                m->PE = qry->hostName();
                m->vrf = sip->vrf;
                m->datetime = sip->datetime;
                m->usandoHistorico = !sip->operativo;
                lstARPsBuscar.append(m);

                if ( !lstVlansDeARP.contains( m->vlan ) )
                    lstVlansDeARP.append( m->vlan );
            }
        }        
        if ( !debugMacIP.isEmpty() && !lstARPsBuscar.isEmpty() )
            break;
    }


    //buscamos las MACs
    bool debug;
    if ( lstARPsBuscar.size() == 1 ) debug = true; else debug = false;

    foreach (Queries *qry, lstMACs)
    {        
        foreach (MacBuscando* arp, lstARPsBuscar) //recorremos las mac a buscar
        {
            foreach ( SMacInfo* mac, qry->macInfo() ) //se recorren las macs conocidas en ese equipo
            {
                //verificamos que sean iguales las mac y las vlan
                if ( arp->mac != mac->mac || arp->vlan != mac->vlan )
                    continue;

                //si la interfaz es una vlan o es drop pasamos al siguiente equipo
                QRegExp exp("^\\d+$");
                if ( mac->interfaz.contains( exp ) ||
                     mac->interfaz == "Drop" )
                    break;

                //que en la interfaz no se aprenda MACs de PEs
                bool encontrado=false;
                foreach ( SMacInfo* mac2, qry->macInfo() )
                {
                    if ( mac2->interfaz == mac->interfaz )
                    {
                        if ( lstMacsPE.contains(mac2->mac) )
                        {
                            encontrado=true;
                            break;
                        }
                    }
                }
                if ( encontrado )
                    continue;

                if ( debug )
                {
                    arp->debug.append("\nTrabajando en este equipo "+qry->hostName()+" "+qry->ip());
                    arp->debug.append("Interfaz: "+mac->interfaz);
                    arp->debug.append("lstVlansDeARP: "+lstVlansDeARP.join(","));
                }

                //averiguamos cuantas macs se aprende de esa interfaz
                short int macsaprendidas = 0;
                foreach ( SMacInfo *m, qry->macInfo() )
                {
                    if ( m->interfaz == mac->interfaz )
                    {
                        if ( lstVlansDeARP.contains(m->vlan) )
                        {
                            macsaprendidas++;
                            if (debug)
                                arp->debug.append("Agregando Mac aprendida a interfaz, vlan: "+m->vlan);
                        }
                    }
                }

                if ( debug )
                    arp->debug.append("Macs aprendidas en interfaz: "+QString::number(macsaprendidas));

                bool equipmentNeighborsinfo;
                bool vlansgestion;
                bool asr900=false;
                bool cambiar=false;
                QString descripcion = interfaceDescription( qry->interfacesDescriptionsInfo(), mac->interfaz );
                qDebug() << "1111" << descripcion;

                if ( qry->platform().contains("ASR-9") ||
                     qry->platform().contains("A901"))
                    asr900=true;

                equipmentNeighborsinfo = interfaceHasEquipmentNeighborsInfo( qry->equipmentNeighborsInfo(), qry->portChannelInfo(), mac->interfaz).size();
                vlansgestion = interfaceLearnedManagementVlans( qry->macInfo(), mac->interfaz, pais ).size();

                if ( debug )
                {
                    arp->debug.append(QString("Vecinos: ")+(equipmentNeighborsinfo?"true":"false")+
                                      QString(" Vlangestion: ")+(vlansgestion?"true":"false")+
                                      QString(" Asr900: ") +(asr900?"true":"false"));
                }

                if ( qry->brand() == "Cisco" )
                {
                    //cisco equipo principal

                    if ( debug )
                    {
                        arp->debug.append( "\nCisco: nueva info a evaluar: "+
                                           mac->interfaz+" "+
                                           mac->datetime.toString("yyyy-MM-dd"));
                    }

                    if ( !equipmentNeighborsinfo &&
                         !vlansgestion &&
                         !asr900 &&
                         mac->datetime.date() >= arp->datetime.date() &&
                         ( arp->lastLearnedMac == 0 || macsaprendidas < arp->lastLearnedMac ) )
                    {
                        //encontrada

                        if ( debug )
                        {
                            arp->debug.append( "Antes: "+
                                               arp->equipo+" "+
                                               arp->interfaz+" "+
                                               arp->interfaceDescription+" "+
                                               arp->datetime.toString("yyyy-MM-dd")+" "+
                                               "Macs aprendidas "+QString::number(arp->lastLearnedMac));
                        }

                        arp->interfaz = mac->interfaz;
                        arp->equipo = simplicateName( qry->hostName() );
                        arp->equipoip = qry->ip();
                        arp->equipoplatform = qry->platform();
                        arp->interfaceDescription = descripcion;
                        arp->ultimoEsfuerzo=false;
                        arp->datetime = mac->datetime;
                        arp->lastLearnedMac=macsaprendidas;

                        if ( !mac->operativo )
                            arp->usandoHistorico=true;

                        if ( debug )
                        {
                            arp->debug.append( "Nueva: "+
                                               arp->equipo+" "+
                                               arp->interfaz+" "+
                                               arp->interfaceDescription+" "+
                                               arp->datetime.toString("yyyy-MM-dd")+" "+
                                               "Macs aprendidas "+QString::number(arp->lastLearnedMac));
                        }
                    }
                    else
                    {
                        //ultimo esfuerzo

                        if ( arp->interfaceUltimoEsfuerzo.isEmpty() ) //primera vez
                        {
                            arp->interfaceUltimoEsfuerzo = mac->interfaz;
                            arp->equipoUltimoEsfuerzo = simplicateName( qry->hostName() );
                            arp->equipoipUltimoEsfuerzo = qry->ip();
                            arp->equipoplatformUltimoEsfuerzo = qry->platform();
                            arp->interfaceDescripcionUltimoEsfuerzo = descripcion;
                            arp->lastLearnedMacUltimoEsfuerzo=macsaprendidas;
                            arp->lastEquipmentNeighborsInfo=equipmentNeighborsinfo;
                            arp->lastAsr900=asr900;
                            arp->lastDateTimeUltimoEsfuerzo = mac->datetime;
                            arp->lastVlansGestion=vlansgestion;
                            if ( !mac->operativo )
                                arp->usandoHistoricoUltimoEsfuerzo=true;

                            if ( debug )
                            {
                                arp->debug.append( "\nUltimo esfuerzo primera vez: "+
                                                   arp->equipoUltimoEsfuerzo+" "+
                                                   arp->interfaceUltimoEsfuerzo+" "+
                                                   arp->interfaceDescripcionUltimoEsfuerzo+" "+
                                                   arp->lastDateTimeUltimoEsfuerzo.toString("yyyy-MM-dd")+" "+
                                                   "Macs aprendidas "+QString::number(arp->lastLearnedMacUltimoEsfuerzo));
                            }
                        }
                        else //segunda vez en adelante
                        {
                            if ( !arp->lastLearnedMacUltimoEsfuerzo )
                                break;

                            if ( debug )
                            {
                                arp->debug.append( "\nUltimo esfuerzo antes: "+
                                                   arp->equipoUltimoEsfuerzo+" "+
                                                   arp->interfaceUltimoEsfuerzo+" "+
                                                   arp->interfaceDescripcionUltimoEsfuerzo+" "+
                                                   arp->lastDateTimeUltimoEsfuerzo.toString("yyyy-MM-dd")+" "+
                                                   "Macs aprendidas "+QString::number(arp->lastLearnedMacUltimoEsfuerzo));
                            }

                            if ( mac->datetime.date() >= arp->lastDateTimeUltimoEsfuerzo.date() &&
                                 macsaprendidas < arp->lastLearnedMacUltimoEsfuerzo )
                            {
                                if ( debug )
                                    arp->debug.append( "cambiando: Macs aprendidas son menores que el anterior" );
                                cambiar=true;
                            }
//                            else if ( macsaprendidas == arp->lastLearnedMacUltimoEsfuerzo )
//                            {
//                                if ( (arp->lastAsr900 || asr900) &&
//                                     macsaprendidas < arp->lastLearnedMacUltimoEsfuerzo)
//                                {
//                                    if ( debug )
//                                        arp->debug.append( "cambiando: ASR900 y Macs aprendidas son menores que el anterior" );
//                                    cambiar=true;
//                                }
//                                else if ( !equipmentNeighborsinfo &&
//                                          !vlansgestion &&
//                                          !arp->lastEquipmentNeighborsInfo &&
//                                          !arp->lastVlansGestion &&
//                                          macsaprendidas < arp->lastLearnedMacUltimoEsfuerzo )
//                                {
//                                    if ( debug )
//                                        arp->debug.append( "cambiando: nuevo sin info de vecions, vlasn de gestion y menores macs " );
//                                    cambiar=true;
//                                }
//                                else if ( !equipmentNeighborsinfo &&
//                                          !vlansgestion &&
//                                          (arp->lastEquipmentNeighborsInfo || arp->lastVlansGestion) )
//                                {
//                                    if (debug)
//                                        arp->debug.append("cambiando: antes tenia info de vecinos y/o vlans de gestio, nuevo no");
//                                    cambiar=true;
//                                }
//                                else if ( (equipmentNeighborsinfo || vlansgestion) &&
//                                          (arp->lastEquipmentNeighborsInfo || arp->lastVlansGestion) &&
//                                          macsaprendidas < arp->lastLearnedMacUltimoEsfuerzo )
//                                {
//                                    if ( debug )
//                                        arp->debug.append("Cambiando: antes y nuevo tenian vecinos y/o vlans de gestion pero nuevo tiene menos macs aprendidas");
//                                    cambiar=true;
//                                }
//                            }

                            if ( cambiar )
                            {
                                arp->interfaceUltimoEsfuerzo = mac->interfaz;
                                arp->equipoUltimoEsfuerzo = simplicateName( qry->hostName() );
                                arp->equipoipUltimoEsfuerzo = qry->ip();
                                arp->equipoplatformUltimoEsfuerzo = qry->platform();
                                arp->interfaceDescripcionUltimoEsfuerzo = descripcion;
                                arp->lastLearnedMacUltimoEsfuerzo=macsaprendidas;
                                arp->lastEquipmentNeighborsInfo=equipmentNeighborsinfo;
                                arp->lastAsr900=asr900;
                                arp->datetime = mac->datetime;
                                arp->lastVlansGestion=vlansgestion;
                                if ( !mac->operativo )
                                    arp->usandoHistoricoUltimoEsfuerzo=true;

                                if ( debug )
                                {
                                    arp->debug.append( "\nUltimo esfuerzo nuevo: "+
                                                       arp->equipoUltimoEsfuerzo+" "+
                                                       arp->interfaceUltimoEsfuerzo+" "+
                                                       arp->interfaceDescripcionUltimoEsfuerzo+" "+
                                                       arp->lastDateTimeUltimoEsfuerzo.toString("yyyy-MM-dd")+" "+
                                                       "Macs aprendidas "+QString::number(arp->lastLearnedMacUltimoEsfuerzo));
                                }
                            }
                        }
                    }
                }
                else if ( segundoEquipo )
                {
                    //ATN ETX etc

                    if ( !vlansgestion &&
                         mac->datetime.date() >= arp->datetime2.date() &&
                         ( arp->lastLearnedMac2 == 0 || macsaprendidas < arp->lastLearnedMac2 ) )
                    {
                        //encontrada

                        qDebug() << "dsadsadasd as" << descripcion;

                        arp->interfaz2 = mac->interfaz;
                        arp->equipo2 = simplicateName( qry->hostName() );
                        arp->equipo2ip = qry->ip();
                        arp->equipo2platform = qry->platform();
                        arp->interface2Description = descripcion;
                        arp->datetime2 = mac->datetime;
                        arp->lastLearnedMac2=macsaprendidas;
                        if ( !mac->operativo )
                            arp->usandoHistorico=true;
                    }
                }
                break;
            }
        }
    }

    //se encuentra el problema de que hay MACs en la vlan de gestion que desaparecen de los equipo debido a que no hay trafico
    //solo se encuentra la de trafico
    //buscamos todos los que terminaron con ultimo esfuerzo y buscamos si existe la misma MAC en otra vlan para copiar la interfaz
    foreach (MacBuscando* arp, lstARPsBuscar)
    {
        if ( !arp->ultimoEsfuerzo )
            continue;

        foreach (MacBuscando* arpotro, lstARPsBuscar)
        {
            if ( arp->mac == arpotro->mac && !arpotro->ultimoEsfuerzo )
            {
                arp->interfaz = arpotro->interfaz;
                arp->equipo = arpotro->equipo;
                arp->interfaceDescription = arpotro->interfaceDescription;
                arp->ultimoEsfuerzo=false;
                arp->datetime = arpotro->datetime;
                break;
            }
        }
    }

    return lstARPsBuscar;
}

QList<MacBuscando*> buscarPuertosAccesoRutaCompleta(QList<Queries*> lstTopologia,
                                                    QList<Queries*> lstARP,
                                                    QString pais,
                                                    QString IP,
                                                    QString VRF)
{
    QList<MacBuscando*> lstARPsBuscar;
    QStringList lstMacsPE;

    //buscamos las macs de los PE
    foreach ( Queries *qry , lstARP)
    {
        foreach ( SIpInfo* sip , qry->arpsInfo())
        {
            if ( sip->age == "-" )
                lstMacsPE.append( sip->mac );
        }
    }

    bool terminado=false;
    //creamos el listado de las MACs a buscar
    foreach ( Queries *qry , lstARP)
    {
        foreach ( SIpInfo* sip , qry->arpsInfo())
        {
            //Si se establecio la IP unicamente
            if ( !IP.isEmpty() )
            {
                if ( sip->ip != IP || sip->vrf != VRF )
                    continue;
            }

            //No nos interesa las macs de HSRP
            if ( sip->mac.contains("0000.0c07.ac") )
                continue;

            //que no sea una mac de PE
            if ( lstMacsPE.contains( sip->mac ) )
                continue;

            //verificamos si ya se habia agregado
            bool encontrado=false;
            foreach (MacBuscando *mb, lstARPsBuscar)
            {
                if ( mb->ip == sip->ip &&
                     mb->vlan == sip->vlan )
                {
                    //ya se habia agregado
                    encontrado=true;
                    if ( sip->datetime.date() > mb->datetime.date() )
                    {
                        //se actualiza ya que esta es información mas nueva
                        mb->mac = sip->mac;
                        mb->PE = qry->hostName();
                        mb->PEip = qry->ip();
                        mb->vrf = sip->vrf;
                    }
                    break;
                }
            }
            if ( !encontrado )
            {
                //se agrega ya que no se encuentra
                MacBuscando *m = new MacBuscando;
                m->mac = sip->mac;
                m->vlan = sip->vlan;
                m->interfaz = sip->interfaz;
                m->ip = sip->ip;
                m->PE = qry->hostName();
                m->PEip = qry->ip();
                m->vrf = sip->vrf;
                m->datetime = sip->datetime;
                m->usandoHistorico = !sip->operativo;
                lstARPsBuscar.append(m);

                if ( !IP.isEmpty() )
                {
                    terminado=true;
                    break;
                }
            }
        }
        if ( terminado )
            break;
    }    

    //buscamos
    Queries *qryLastEq;
    foreach (MacBuscando *m, lstARPsBuscar)
    {
        //se empieza a buscar la ruta

        QStringList path;
        path.append( m->PE ); //agregando el primer equipo, el PE

        int c = 0;

        qryLastEq = buscarEquipoPorIPnombrePais(lstTopologia,m->PEip,m->PE,pais);
        if ( !qryLastEq )
            continue;

        forever
        {
            c++;

            if ( !qryLastEq->operativo() )
                m->usandoHistorico = true;

            QString interfaz;
            foreach ( SMacInfo *mi , qryLastEq->macInfo() )
            {
                if ( m->mac == mi->mac )
                {
                    interfaz = mi->interfaz;
                    if ( !mi->operativo )
                        m->usandoHistorico = true;
                    break;
                }
            }
            if ( interfaz.isEmpty() )
            {
                if ( c == 1 && !m->interfaz.isEmpty() )
                {
                    //usamos la interfaz aprendida del arp del pe ya que es una interfaz L3 y no hay mac

                    //quitamos la subinterfaz
                    QString inter = m->interfaz;
                    if ( inter.contains(".") )
                        inter = inter.split(".").first();
                    interfaz = interfazSimplifier(inter);
                    m->interfaz.clear(); //la limpiamos para que no este disponible en un equipo mas adelante
                }
                else
                {
                    m->interfaceDescription = "Sin información de MAC";
                    path.append("Error");
                    break;
                }
            }

            QRegExp expip("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
            if ( interfaz.contains( expip ) )
            {
                //interfaz indica la IP del PE hacia donde va el PW, se busca dicho equipo para saltar a el

                QString ip = expip.cap(0);
                path.append("PW to "+ip);

                //se busca equipo por la IP
                qryLastEq = buscarEquipoPorIP(lstTopologia,ip,pais);
                if ( !qryLastEq )
                {
                    m->interfaceDescription = "Error. No se encontró consulta de equipo para continuar";
                    path.append("Error");
                    break;
                }

                path.append( qryLastEq->hostName() );
            }
            else
            {
                //Interfaz es una normal, se busca vecino de cdp o lldp para saltar al proximo equipo

                //agregamos la interfaz
                path.append( interfaz );

                //se busca el siguiente equipo por medio de informacion de vecinos
                QList<SEquipmentNeighborsInfo*> lstequipmentNeighbors = interfaceHasEquipmentNeighborsInfo( qryLastEq->equipmentNeighborsInfo(),qryLastEq->portChannelInfo(),interfaz);
                SEquipmentNeighborsInfo* equipmentNeighbors=NULL;
                if ( lstequipmentNeighbors.isEmpty() )
                {
                    //ya no se encuentra equipo por cdp o lldp

                    if ( qryLastEq->interfacesInfoQuery->interfaceInfo(interfaz) )                    
                        m->interfaceDescription = qryLastEq->interfacesInfoQuery->interfaceInfo(interfaz)->description;                                            

                    break;
                }
                else if ( lstequipmentNeighbors.size() > 1 )
                {
                    m->interfaceDescription =  "Se conocen varios equipos en medio multiacceso, no se puede continuar";
                    path.append("Error");
                    break;
                }
                else
                    equipmentNeighbors = lstequipmentNeighbors.at(0);

                if ( !equipmentNeighbors->operativo )
                    m->usandoHistorico = true;

                QString siguienteEquipo=simplicateName( equipmentNeighbors->nombreequipo );
                if ( path.contains(siguienteEquipo) )
                    break;

                path.append( siguienteEquipo );

                qryLastEq = buscarEquipoPorIPnombrePais(lstTopologia,equipmentNeighbors->ip,siguienteEquipo,pais);
                if ( !qryLastEq )
                {
                    m->interfaceDescription = "Error. No se encontró consulta de equipo para continuar";
                    path.append("Error");
                    break;
                }
            }
        }
        m->path = path;
    }

    return lstARPsBuscar;
}

bool puertosAccesoVerificarEquipoInterfazMac( QList<MacBuscando*> lst,
                                              QString equipo,
                                              QString interfaz,
                                              QString mac)
{
    foreach (MacBuscando *macPuertoAcceso, lst)
    {
        if ( macPuertoAcceso->mac == mac &&
             macPuertoAcceso->equipo == equipo &&
             macPuertoAcceso->interfaz == interfaz )
            return true;
    }
    return false;
}
