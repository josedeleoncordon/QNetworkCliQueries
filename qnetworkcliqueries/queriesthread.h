#ifndef QUERIESTHREAD_H
#define QUERIESTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include "queries.h"

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
    void setLstIP(QStringList lst) { m_lstIP = lst; }
    void setLstIPsConsultaAnterior(QStringList lst) { lstIPsConsultaAnterior=lst; }
    void setConsultaSoloEquiposNuevos(bool enable) { m_soloequiposnuevos = enable; }
    void setConsultaLinksEnSegmentos(QStringList lstSegmentos) { m_lstLinksEnSegmentos=lstSegmentos; }
    void setInterval(int interval) { m_interval = interval; }
    void setSimultaneos(int simultaneos) { m_simultaneos = simultaneos; }
    void setMaxParalelos(int max) { m_maxparalelos = max; }
    void setOpciones(int opciones) { m_opciones = opciones; }
    void setEquipmentNeighborsConsultarVecinos(bool consultar) { m_equipmentNeighborsConsultarVecinos=consultar; }
    void setEquipmentNeighborsOSPFMismoDominio(bool consultar) { m_consultarVecinosOSPFMismoDominio=consultar; }
    void setEquipmentNeighborsOSPFArea(QString area) { m_consultaOSPFArea=area; }

    void iniciar();
    void iniciarSync();
    void detener(); //no se consultan nuevos equipos y espera a que se terminen las consultas ya en ejeucución.
    void cancelar(); //elimina las consultas en ejecución y se finaliza.

    int equiposTotal() { return m_lstIP.size(); }
    int equiposSimultaneos() { return m_consultaSimultaneos; }
    int equiposConsultados() { return m_equiposConsultados; }
    int equiposExitosos() { return m_equiposExitosos; }
    int equiposConErrores() { return m_conerrores; }
    int equiposSinConexion() { return m_sinconexion; }
    int equiposConexionConErrores() { return m_conexionerrores; }
    QStringList equiposEnConsulta() { return m_equiposenconsulta; }
    QList<Queries*> lstQueries() { return m_lstQueries; }
    QMap<QString, QString> errorMap() { return m_errorMap; }
    QStringList lstIPsConectadosPorGW() { return m_lstIPsConectadosPorGW; }

signals:
    void newInformation();
    void finished(bool);

private slots:
    void on_timer_timeOut();
    void on_timerActivity_timeOut();
    void equipoConsultado(Queries *qry);

private:
   QString m_grupo;
   QString m_user;
   QString m_gw;
   QString m_password;
   QString m_user2;
   QString m_pwd2;
   QString m_linuxprompt;
   QString m_consultaOSPFArea;
   QStringList m_lstIP;
   QStringList m_lstIPsAintentarPorGW;
   QStringList m_lstIPsConectadosPorGW;
   QStringList lstIPsConsultaAnterior;
   QRemoteShell::ConnectionProtocol m_connectionprotocol;
   int m_lstIpPos;
   int m_opciones;
   int m_simultaneos; //cantidad de equipos se suman al mismo tiempo al grupo,
   QStringList m_equiposenconsulta;
   QStringList m_lstLinksEnSegmentos;
   QList<Queries*> m_queriesenconsulta;
   int m_maxparalelos; //cantidad de equipos que se estan trabajando
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
   bool m_consultarVecinosOSPFMismoDominio;
   bool m_soloequiposnuevos;
   bool m_principaluserfirst;
   QList<Queries*> m_lstQueries;
   QMap<QString, QString> m_errorMap;
   QTimer *m_timer;
   QTimer *m_timerActivity;
   int m_consultaSimultaneos;
   int m_equiposConsultados;
   QMutex m_mutex;
   QMap<QString,QString> m_mapOSPFVecinosInterfazDondeVienen;

   void conectarOtroEquipo(QString ip, bool gw=false);
   void validarYagregarVecinoAconsulta(Queries *qry,
                                       QString ip,
                                       QString ipOinterfazDondeSeViene,
                                       QString interfazEsteEquipoSalida,
                                       QString interfazSiguienteEquipoEntrada);
};

QMap<QString, QString> updateInfoMapError(QMap<QString, QString> &ant, QMap<QString, QString> &nue,
                                          QStringList &lstIPs);

#endif // QUERIESTHREAD_H
