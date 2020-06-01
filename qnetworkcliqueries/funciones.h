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

//listado de grupos
QStringList lstGrupos();

//devuelve el tipo de equipo desde la plataforma
QString equipmentOSFromPlatform(QString platform);

//Abre un archivo de texto
QString openFile(QString path);

//Abre un archivo y lo devuelve en lista
QStringList openFile2List(QString path, bool eliminarDuplicados = false );

//Guarda un archivo de texto
void saveFile(QString txt,QString path,QIODevice::OpenModeFlag openmode = QIODevice::WriteOnly);

//Abre un archivo de consultas
LstQueries openLstQueries(QString path);

//guarda un archivo de consultas
void saveLstQueries(LstQueries &lst, QString path);

//simplifica el nombre de un host eliminado del mismo el CPU, modulo, etx. por ejemplo los nombre de Cisco IOS XR
QString simplicateName(QString name);

//Convierte un rango de numeros a una lista, se usa para convertir los rangos de las vlans, ejemplo:
//2,4,7-10 --> 2,4,7,8,9,10
QStringList numberRangeToLST(QString str);

//Busca un equipo en el listado de queries, ip y nombre
//primero busca el nombre y luego confirma si la ip pertenece al equipo, ya que puede ser un equipo L3 con varias IPs
//regresa un puntero ya que si no existe no podriamos regresar una referencia a algo que no existe
Queries *buscarEquipoPorIPnombre(QList<Queries> &lst, QString ip, QString nombre);

//Busca un equipo en el listado de queries, solo ip
//regresa un puntero ya que si no existe no podriamos regresar una referencia a algo que no existe
Queries *buscarEquipoPorIP(QList<Queries> &lst, QString ip);

//Busca un equipo en el listado de queries, solo nombre
//regresa un puntero ya que si no existe no podriamos regresar una referencia a algo que no existe
Queries *buscarEquipoPorNombre(QList<Queries> &lst, QString nombre);

//verifica si la ip indicada pertenece al equipo indicado
bool validateIpQuery(Queries &qry, QString ip);

//valida que la plataforma del equipo sea valido. No equipos de clientes
bool platformValidate(QString platform);

//valida que el equipo aprendido por EquipmentNeighbors sea un equipo de la red.
//que el equipo no sea de un cliente
bool equipmentNeighborsVecinoValidarEquipo(SEquipmentNeighborsInfo &e);

//devuelve la descripcion de una interfaz
QString interfaceDescription(QList<SInterfaceInfo> &lstInterfaceDescriptions, QString interfaz);

//devuelve el listado de vecinos que se conocen por una interfaz
QList<SEquipmentNeighborsInfo*> interfaceHasEquipmentNeighborsInfo(QList<SEquipmentNeighborsInfo> &lstEquipmentNeighborsInfo, QList<SPortChannel> &lstPCInfo, QString i);

//verifica si la interfa es l2
bool interfaceIsL2(Queries &q, QString interfaz);

//verifica si las dos interfaces son L2
bool interfacesAreL2OneVlanMatches(Queries &q1, QString interfaz1, Queries &q2, QString interfaz2);

//verifica si la interfaz es L3
bool interfaceIsL3(Queries &q, QString interfaz);

//verifica si la interfaz esta en una vrf
bool interfaceIsInVRF(Queries &q, QString interfaz);

//devuelve el area de ospf de una interfaz
QString interfaceOspfArea(Queries &q, QString interfaz);

//devuelve el proceso de ospf de una interfaz
QString interfaceOspfProceso(Queries &q, QString interfaz);

//regresa la Po interfaz a la que pertenece la interfaz.
//Si no pertenece a alguno PO se regresa la misma interfaz enviada.
QString interfaceToPortChannelInterface(QList<SPortChannel> &lst, QString interface, QString plataforma);

//regresa las interfaces que pertenecen a un portchannel
QStringList interfacePortChannelToInterfaces(QList<SPortChannel> &lst, QString pcInterface);

//verifica si las dos ips pertenecen al mismo segmento /30 /31
bool sonIPsParejaMascara30_31( QString ip1, QString ip2 );

//convierte una IP (192.168.1.1) a su forma binaria
QString IP2Binario(QString ip);

//verifica si una IP pertenece a un segmento
bool validarIPperteneceAsegmento(QString IP, QString segmentoIP_mascara2digitos);  //segmentoIP_mascara2digitos ej: 192.168.1.0/24

//buscar la pareja de la interfaz que pertenezca al mismo segmento /30
Queries *buscarEquipoInterfazPorParejaDeIPMascara30(QList<Queries> &lst,
                                                    QString ipBuscar,
                                                    QString &interfaz,
                                                    QString ospfarea);

//verifica si hay un equipmentNeighbors operativo hacia un equipo
bool existeEquipmentNeighborsOperativoHaciaEquipo( QList<SEquipmentNeighborsInfo> &lst, QString hostname );

//elimina caractares menores al ascii 32
QString eliminarCaractaresNoImprimibles(QString txt);

////Busqueda de ARP segun IP o MAC
//SIpInfo* buscarARP(QList<Queries*> lstQueries, QString IPoMAc);

//analiza la información de MPLS LDP discovery e interfaces para decir si el vecino esta arriba, abajo o no configurado
bool mplsInterfaceStatus(QString interface,
                         QString& txtStatusOut,
                         QList<SMplsLdpInfo> &lstDiscovery,
                         QList<SMplsLdpInfo> &lstInterfaces);

//analiza la información de vecinos e interfaces de ospf para decir si el vecino esta arriba, abajo o no configurado
bool ospfInterfaceStatus(QString interface,
                         QString& txtStatusOut,
                         QList<SOSPFInfo> &lstOspfInfo);

//Busca el archivo consultas mas actual en un directorio
QString buscarArchivoConsultasActual(QString path, QString filelabel, QString grupo);

//crea el nombre del archivo de consultas
QString nombreArchivoLstQueries(LstQueries&);

//simplifica el nombre de las interfaces, por ejemplo GigabitEthernet0/1 a Gi0/1
QString estandarizarInterfaz(QString interfaz);

//convierte el nombre de los protocolos de las consultas de tablas de enrutamiento
QString estandarizarProtocoloEnrutamiento(QString proto);

//Indica si la interfaz hacia un segundo equipo pertenece al mismo dominio de OSPF de la interfaz por donde se
//vino del equipo anterior
bool continuarPorsiguienteInterfazMismoDominioOSPF(Queries &q,
                                                   QString interfazOipDondeViene,
                                                   QString interfazSiguienteEquipo );

#endif // FUNCIONES_H
