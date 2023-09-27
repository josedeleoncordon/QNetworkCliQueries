#ifndef QUERIESTHREAD_H
#define QUERIESTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include "queries.h"
#include "queriesthreadworker.h"

class Host;

class QNETWORKCLIQUERIES_EXPORT QueriesThread : public QObject
{
    Q_OBJECT
public:
    explicit QueriesThread(QObject *parent = 0);
    ~QueriesThread();
    void setGrupo(QString grupo) { m_grupo = grupo; }
    void setUser(QString user) { m_user = user; }
    void setGW(QString gw) { m_gw = gw; }
    void setPassword(QString password) { m_password=password; }
    void setUser2(QString user) { m_user2=user; }
    void setPassword2(QString pwd) { m_pwd2=pwd; }
    void setPrincipalUserFirst(bool principalfirst) { m_principaluserfirst=principalfirst; }
    void setLinuxPrompt(QString prompt) { m_linuxprompt = prompt; }
    void setConnectionProtocol( QRemoteShell::ConnectionProtocol cp ) { m_connectionprotocol = cp; }
    void setRemoteShellUsersPasswords(QStringList lst) { lstRemoteShellUsersPasswords=lst; }
    void setLstIP(QStringList lst);
    void setQueriesConfiguration(QueriesConfiguration configuration);
    void setLstIPsConsultaAnterior(QStringList lst) { lstIPsConsultaAnterior=lst; }
    void setConsultaSoloEquiposNuevos(bool enable) { m_soloequiposnuevos = enable; }
    void setConsultaAgregarVecinosLinksEnSegmentos(QStringList lstSegmentos) { m_lstLinksEnSegmentos=lstSegmentos; }
    void setConsultaAgregarVecinosLoopbacksEnSegmentos(QStringList lstSegmentos) { m_lstLoopbacksEnSegmentos=lstSegmentos; }
    void setInterval(int interval) { m_interval = interval; }
    void setSimultaneos(int simultaneos) { m_simultaneos = simultaneos; }
    void setMaxParalelos(int max) { m_maxparalelos = max; }
    void setOptions(QList<int> opciones);
    void setEquipmentNeighborsConsultarVecinos(bool consultar) { m_equipmentNeighborsConsultarVecinos=consultar; }
    void setEquipmentNeighborsOSPFMismoDominio(bool consultar) { m_consultarVecinosOSPFMismoDominio=consultar; }
    void setEquipmentNeighborsOSPFEquipoRaizProceso(QString proceso) { m_consultarVecinosOSPFEquipoRaizProceso=proceso; }
    void setEquipmentNeighborsRegExpOSPFArea(QString area) { m_consultaOSPFArea=area; }
    void setNewThreadWorker(QueriesThreadWorker*(*ThreadWorker)(void));    
    void setUUID(QString uuid) { m_uuid = uuid; }

    void iniciar();
    void iniciarSync();
    void detener(); //no se consultan nuevos equipos y espera a que se terminen las consultas ya en ejeucución.
    void cancelar(); //elimina las consultas en ejecución y se finaliza.

    int equiposTotal() { return m_lstIPsMismoEquipoCounter + m_lstIPsCounter; }
    int equiposSimultaneos() { return m_consultaSimultaneos; }
    int equiposConsultados() { return m_equiposConsultados.size(); }
    int equiposExitosos() { return m_equiposExitosos; }
    int equiposConErrores() { return m_conerrores; }
    int equiposSinConexion() { return m_sinconexion; }
    int equiposConexionConErrores() { return m_conexionerrores; }
    QStringList equiposEnConsulta() { return m_equiposenconsulta; }
    QList<Queries> lstQueries();
    QList<Queries*> lstQueriesPointers();
    Queries queriesFromIP(QString ip);
    QMap<QString, QString> errorMap() { return m_errorMap; }
    QStringList lstIPsConectadosPorGW() { return m_lstIPsConectadosPorGW; }

signals:
    void newInformation();
    void status(QString,QString);
    void finished(bool);
    void canceled(QString);

private slots:
    void on_timer_timeOut();
    void on_timerActivity_timeOut();
    void on_uuidCanceled(QString);

protected slots:
    void equipoConsultado(Queries *qry);

protected:
   QString m_grupo;
   QString m_user;
   QString m_gw;
   QString m_password;
   QString m_user2;
   QString m_pwd2;
   QString m_linuxprompt;
   QString m_consultaOSPFArea;
   QStringList m_lstIP;
   QStringList m_lstIPsMismoEquipo; //Para varias consultas de un equipo en varias conexiones: 172.16.30.253_1,172.16.30.253_2
   short int m_lstIPsMismoEquipoCounter;
   short int m_lstIPsCounter;
   QStringList m_lstIPsAintentarPorGW;   
   QStringList m_lstIPsConectadosPorGW;
   QStringList lstIPsConsultaAnterior;
   QRemoteShell::ConnectionProtocol m_connectionprotocol;
   QStringList lstRemoteShellUsersPasswords;
   QList<int> m_opciones;
   int m_simultaneos; //cantidad de equipos se suman al mismo tiempo al grupo,
   QStringList m_equiposenconsulta;
   QStringList m_lstLinksEnSegmentos;
   QStringList m_lstLoopbacksEnSegmentos;
   QList<Queries*> m_queriesenconsulta;
   int m_maxparalelos; //cantidad de equipos que se estan trabajando
   int m_maxparalelosmismoequipo; //para consultas divididas en varias conexiones
   int m_equiposExitosos;
   int m_interval;
   int m_conerrores;
   int m_equiposPorGWenConsulta;
   int m_conexionerrores;
   int m_sinconexion;   
   QString m_logpath;
   bool m_detener;
   bool m_cancelar;
   bool m_equipmentNeighborsConsultarVecinos;
   QString m_uuid;

   //si el equipo por donde se empieza la consulta tiene mas de un proceso recorrera todos. ASBR UMPLS
   bool m_consultarVecinosOSPFMismoDominio;
   QString m_consultarVecinosOSPFEquipoRaizProceso; //con este especificamos si deseamos solo recorrer uno

   bool m_soloequiposnuevos;
   bool m_principaluserfirst;
   QList<Queries*> m_lstQueries;
   QMap<QString, QString> m_errorMap;
   QTimer *m_timer;
   QTimer *m_timerActivity;
   int m_consultaSimultaneos;   
   QStringList m_equiposConsultados;
   QMutex m_mutex;
   QMap<QString,QString> m_mapOSPFVecinosInterfazDondeVienen;
   QueriesConfiguration m_queriesconfiguration;

   QueriesThreadWorker*(*ThreadWorker)(void) = nullptr;

   void _emitNewInformation();
   void _emitFinished(bool);

   void _clear();
//   QueriesThreadWorker *newThreadWorker();
   bool conectarSiguienteMismoEquipo(QString IP);
   void siguienteEquipo(QString IP, bool gw=false);

   void validarYagregarVecinoAconsulta(Queries *qry,
                                       QString ip,
                                       QString ipOinterfazDondeSeViene,
                                       QString interfazEsteEquipoSalida,
                                       QString interfazSiguienteEquipoEntrada);

   //verifica si las configuraciones de parametros trae equipos con consultas en diferentes conexiones para actualizar
   //la lista de IPs
   void verificarLstIPsQueriesConfiguration();
};

QMap<QString, QString> updateInfoMapError(QMap<QString, QString> &ant, QMap<QString, QString> &nue,
                                          QStringList &lstIPs);

#endif // QUERIESTHREAD_H
