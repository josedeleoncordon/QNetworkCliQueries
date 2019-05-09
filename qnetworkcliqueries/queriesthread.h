#ifndef QUERIESTHREAD_H
#define QUERIESTHREAD_H

#include <QObject>
#include <QThread>

#include "queries.h"

class Host;

class QNETWORKCLIQUERIES_EXPORT QueriesThread : public QObject
{
    Q_OBJECT
public:
    explicit QueriesThread(QObject *parent = 0);
    ~QueriesThread();
    void setPais(QString pais) { m_pais = pais; }
    void setUser(QString user) { m_user = user; }
    void setGW(QString gw) { m_gw = gw; }
    void setPassword(QString password) { m_password=password; }
    void setUserOtro(QString user) { m_userother=user; }
    void setPasswordOtro(QString pwd) { m_pwdother=pwd; }
    void setPrincipalUserFirst(bool principalfirst) { m_principaluserfirst=principalfirst; }
    void setConnectionProtocol( QRemoteShell::ConnectionProtocol cp ) { m_connectionprotocol = cp; }
    void setLstIP(QStringList lst);
    void setInterval(int interval) { m_interval = interval; }
    void setSimultaneos(int simultaneos) { m_simultaneos = simultaneos; }
    void setMaxParalelos(int max) { m_maxparalelos = max; }
    void setOpciones(int opciones) { m_opciones = opciones; }
    void setLogPath(QString path);
    void setEquipmentNeighborsConsultarVecinos(bool consultar) { m_equipmentNeighborsConsultarVecinos=consultar; }

    void iniciar();
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

signals:
    void newInformation();
    void finished(bool);

private slots:
    void on_timer_timeOut();
    void on_timerActivity_timeOut();
    void equipoConsultado(Queries *qry);

private:
   QString m_pais;
   QString m_user;
   QString m_gw;
   QString m_password;
   QString m_userother;
   QString m_pwdother;
   QList<Host*> m_lstIP;
   QStringList lstIPsAgregadosPorVecinos;
   QRemoteShell::ConnectionProtocol m_connectionprotocol;
   int m_lstIpPos;
   int m_opciones;
   int m_simultaneos; //cantidad de equipos se suman al mismo tiempo al grupo,
   QStringList m_equiposenconsulta;
   QList<Queries*> m_queriesenconsulta;
   int m_maxparalelos; //cantidad de equipos que se estan trabajando
   int m_equiposExitosos;
   int m_interval;
   int m_conerrores;
   int m_conexionerrores;
   int m_sinconexion;   
   QString m_logpath;
   bool m_debug;
   bool m_detener;
   bool m_cancelar;
   bool m_equipmentNeighborsConsultarVecinos;
   bool m_principaluserfirst;
   QList<Queries*> m_lstQueries;
   QMap<QString, QString> m_errorMap;
   QTimer *m_timer;
   QTimer *m_timerActivity;
   int m_consultaSimultaneos;
   int m_equiposConsultados;

   void conectarOtroEquipo();
};

QMap<QString, QString> updateInfoMapError(QMap<QString, QString> &ant, QMap<QString, QString> &nue,
                                          QStringList &lstIPs, short modo);

#endif // QUERIESTHREAD_H