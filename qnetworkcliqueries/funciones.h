#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <QFile>
#include <QTextStream>

class Queries;
class LstQueries;
struct SEquipmentNeighborsInfo;
struct SInterfaceInfo;
struct SPortChannel;
struct SMacInfo;
struct SIpInfo;
struct SMplsLdpInfo;
struct SOSPFInfo;
struct MacBuscando;

//listado de paises
QStringList lstPaises();

//devuelve el tipo de equipo desde la plataforma
QString equipmentOSFromPlatform(QString platform);

//devuelve la plataforma desde el OID de SNMP
QString snmpOID2platform(QString oid);

//devuelve la marca del equipo desde el IOD de SNMP
QString snmpOID2Brand(QString oid);

//Abre un archivo de texto
QString openFile(QString path);

//Guarda un archivo de texto
void saveFile(QString txt,QString path,QIODevice::OpenModeFlag openmode = QIODevice::WriteOnly);

//Abre un archivo de consultas
LstQueries* openLstQueries(QString path);

//guarda un archivo de consultas
void saveLstQueries(LstQueries *lst, QString path);

//simplifica el nombre de un host eliminado del mismo el CPU, modulo, etx. por ejemplo los nombre de Cisco IOS XR
QString simplicateName(QString name);

//simplifica el nombre de las interfaces, por ejemplo GigabitEthernet0/1 a Gi0/1
QString interfazSimplifier(QString interfaz);

//Convierte un rango de numeros a una lista, se usa para convertir los rangos de las vlans, ejemplo:
//2,4,7-10 --> 2,4,7,8,9,10
QStringList numberRangeToLST(QString str);

//Busca un equipo en el listado de queries, ip y nombre
//primero busca el nombre y luego confirma si la ip pertenece al equipo, ya que puede ser un equipo L3 con varias IPs
Queries *buscarEquipoPorIPnombrePais(QList<Queries*> lst, QString ip, QString nombre, QString pais);

//Busca un equipo en el listado de queries, solo ip
Queries *buscarEquipoPorIP(QList<Queries*> lst, QString ip,QString pais);

//Busca un equipo en el listado de queries, solo nombre
Queries *buscarEquipoPorNombre(QList<Queries*> lst, QString nombre);

//verifica si la ip indicada pertenece al equipo indicado
bool validateIpQuery(Queries *qry, QString ip, QString pais);

//un equipo puede tener varias IPs configuradas y se usa, por ejemplo, para saber si la IP
//q reporto EquipmentNeighbors pertenece al mismo equipo
bool validateIpHost(QString ip,QString name,QString country);

//valida que la plataforma del equipo sea valido. No equipos de clientes
bool platformValidate(QString platform);

//Regresa el pais al que pertenece la ip del equipo, para saber si el vecino por equipmentNeighbors sigue siendo del mismo pais por ejemplo
QString ipToCountry(QString ip);

//valida que el equipo aprendido por EquipmentNeighbors sea un equipo de la red.
//que el equipo no sea de un cliente
bool equipmentNeighborsVecinoValidarEquipo(SEquipmentNeighborsInfo *e);

//devuelve la descripcion de una interfaz
QString interfaceDescription(QList<SInterfaceInfo*> lstInterfaceDescriptions,QString interfaz);

//devuelve el listado de vecinos que se conocen por una interfaz
QList<SEquipmentNeighborsInfo*> interfaceHasEquipmentNeighborsInfo(QList<SEquipmentNeighborsInfo*> lstEquipmentNeighborsInfo, QList<SPortChannel *> lstPCInfo, QString i, QString pais="");

//devulve las macs en la vlan de gestion de switches que se aprenden en una interfaz
QList<SMacInfo*> interfaceLearnedManagementVlans(QList<SMacInfo*> lst, QString interfaz, QString codigopais);

//verifica si la interfa es l2
bool interfaceIsL2(Queries *q, QString interfaz);

//verifica si las dos interfaces son L2
bool interfacesAreL2OneVlanMatches(Queries *q1, QString interfaz1, Queries *q2, QString interfaz2);

//verifica si la interfaz es L3
bool interfaceIsL3(Queries *q, QString interfaz);

//verifica si la interfaz esta en una vrf
bool interfaceIsInVRF(Queries *q, QString interfaz);

//devuelve el area de ospf de una interfaz
QString interfaceOspfArea(Queries *q, QString interfaz);

//regresa la Po interfaz a la que pertenece la interfaz.
//Si no pertenece a alguno PO se regresa la misma interfaz enviada.
QString interfaceToPortChannelInterface(QList<SPortChannel *> lst, QString interface, QString plataforma);

//regresa las interfaces que pertenecen a un portchannel
QStringList interfacePortChannelToInterfaces(QList<SPortChannel *> lst,QString pcInterface);

//buscar la pareja de la interfaz que pertenezca al mismo segmento /30
Queries *buscarEquipoInterfazPorParejaDeIPMascara30(QList<Queries *> lst,
                                                    QString ipBuscar,
                                                    QString &interfaz,
                                                    QString ospfarea);

//verifica si hay un equipmentNeighbors operativo hacia un equipo
bool existeEquipmentNeighborsOperativoHaciaEquipo( QList<SEquipmentNeighborsInfo*> lst, QString hostname );

//elimina caractares menores al ascii 32
QString eliminarCaractaresNoImprimibles(QString txt);

//Busqueda de ARP segun IP o MAC
SIpInfo* buscarARP(QList<Queries*> lstQueries, QString IPoMAc);

//analiza la información de MPLS LDP discovery e interfaces para decir si el vecino esta arriba, abajo o no configurado
bool mplsInterfaceStatus(QString interface,
                         QString& txtStatusOut,
                         QList<SMplsLdpInfo*> lstDiscovery,
                         QList<SMplsLdpInfo*> lstInterfaces);

//analiza la información de vecinos e interfaces de ospf para decir si el vecino esta arriba, abajo o no configurado
bool ospfInterfaceStatus(QString interface,
                         QString& txtStatusOut,
                         QList<SOSPFInfo*> lstOspfInfo);

//Busca el archivo consultas mas actual en un directorio
QString buscarArchivoConsultasActual(QString pais,QString path,QString filelabel);

//crea el nombre del archivo de consultas
QString nombreArchivoLstQueries(LstQueries*);

#endif // FUNCIONES_H