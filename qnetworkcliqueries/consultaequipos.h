#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include "queriesthread.h"
#include "queries.h"
#include "funcionbase.h"

class QueriesConfigurationValue;

class ConsultaEquipos : public QObject
{
    Q_OBJECT        
private:
    QueriesThread *m_queriesThread;
    QString m_grupo;

    QString m_lstQpath;
    QString m_lstLabel;
    QString m_lstTipo;

    bool m_consultaAnteriorBorrar;
    bool m_consultaConErrores;
    bool m_consultaAgregarVecinos;
    bool m_consultaSoloEquiposNuevos;
    bool m_consultaOSPFMismoDominio;

    LstQueries _lstQ; //origen
    LstQueries _lstQN; //destino
    QList<QueryOpcion> opciones;
    QString m_lstQfilepath;
    QList<Queries> lstQueries;
    QList<Queries> lstQueriesConsultados;
    QMap<QString,QString> errorMap;
    QList<QueriesConfigurationValue> lstQueriesParameters;
    QStringList lstIPAnteriores;
    QStringList lstIP;
    QStringList m_lstLinksEnSegmentos; //agregar los vecinos cuyo link pertenezca a los segmentos establecidos
    QStringList m_lstLoopbacksEnSegmentos; //agregar los vecinos cuya loopback este en el los segmentos establecidos
    QString m_ip;
    QString m_lstIPsPath;
    QString m_gw;
    QString m_user;
    QString m_pwd;
    QString m_user2;
    QString m_pwd2;
    QString m_consultaOSPFArea;
    QString m_linuxprompt;
    int m_interval,m_simultaneos,m_maxParalelos;
    QRemoteShell::ConnectionProtocol m_connectionprotocol;
    QString _error;
    QueriesConfiguration m_queriesConfiguration;

public:    
    explicit ConsultaEquipos(QObject *parent = nullptr);
    ~ConsultaEquipos();
    QueriesThread *queriesThread() { return m_queriesThread; }
    QString newInformationHumanReadable();

    void setGrupo(QString grupo) { m_grupo=grupo; }
    void setUser(QString user) { m_user=user; }
    void setPassword(QString pwd) { m_pwd=pwd; }
    void setLstQPath(QString path) { m_lstQpath = path; }
    void setLstQlabel(QString label) { m_lstLabel = label; }
    void setLstIPsPath(QString path) { m_lstIPsPath = path; }
    void setLstQ(LstQueries &lQ);
    void setIP(QString ip) { m_ip=ip; }
    void setLstIPs(QStringList lstIps) { lstIP = lstIps; }
    void setConsultaAnteriorBorrar(bool enable) { m_consultaAnteriorBorrar=enable; }
    void setConsultaConErrores(bool enable) { m_consultaConErrores=enable; }
    void setConsultaAgregarVecinos(bool enable) { m_consultaAgregarVecinos=enable; }
    void setConsultaSoloEquiposNuevos(bool enable) { m_consultaSoloEquiposNuevos=enable; }
    void setConsultaOSPFMismoDominio(bool enable) { m_consultaOSPFMismoDominio=enable; }
    void setConsultaOSPFArea(QString area) { m_consultaOSPFArea = area; }
    void setConsultaLinksEnSegmentos(QStringList lstSegmentos) { m_lstLinksEnSegmentos=lstSegmentos; }
    void setConsultaLoopbacksEnSegmentos(QStringList lstSegmentos) { m_lstLoopbacksEnSegmentos=lstSegmentos; }
    void setGW(QString gw) { m_gw=gw; }
    void setConsultaInterval(int interval) { m_interval=interval; }
    void setConsultaSimultaneos(int simultaneos) { m_simultaneos=simultaneos; }
    void setConsultaMaxParalelos(int maxParalelos) { m_maxParalelos=maxParalelos; }
    void setConnectionProtocol( QRemoteShell::ConnectionProtocol cp ) { m_connectionprotocol = cp; }
    void setUser2(QString user) { m_user2=user; }
    void setPassword2(QString pwd) { m_pwd2=pwd; }

    void addOpcionesConsulta(QList<QueryOpcion> lst);
    void addParametrosConsulta(const QList<QueriesConfigurationValue>&);

    void consultarEquiposSync();
    LstQueries& lstQ() { return _lstQN; }
    QString errorTxt() { return _error; }

signals:
    void error();
    void queriesThread_newInformation();
    void finished();

private slots:
    void on_queriesThread_finished(bool);

public slots:
    void consultarEquipos();
};

#endif // MAINCLASS_H
