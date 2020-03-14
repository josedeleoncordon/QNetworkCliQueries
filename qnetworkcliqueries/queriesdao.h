#ifndef QUERIESDAO_H
#define QUERIESDAO_H

#include <QObject>
#include <qremoteshell.h>


class Queries;

class QueriesDAO : public QObject
{
    Q_OBJECT
public:
    explicit QueriesDAO(Queries *qry, QObject *parent = nullptr);
    explicit QueriesDAO(Queries *qry, QString user, QString pwd, QString linuxprompt, QObject *parent = nullptr);
    ~QueriesDAO();

    enum Opcion {
        Null = 0,
        Connect = 4,
        Platform = 8,
        EquipmentNeighbors = 16,
        MacAddress = 32,
        InterfaceInformation = 64,
        InterfacePermitedVlans = 128,
        InterfaceDescription = 256,
        InterfaceIpAddresses = 512,
        Ospf = 1024,
        MplsTEtunnels = 2048,
        MplsLdpDiscovery = 4096,
        MplsLdpNeighbors = 8192,
        MplsLdpInterfaces = 16384,
        PimInterfaces = 32768,
        PortChannel = 65536,
        VRFfVlans = 131072,
        VRFfRT = 262144,
        VRFs = 524288,
        Arp = 1048576,
        BGPNeig = 2097152,
        IpRoutes = 4194304,
        Configuration = 8388608,
        Exit = 16777216
    };

    QString ipOinterfazMismoDominioOSPFDondeSeViene() { return m_ipOinterfazMismoDominioOSPFdondeSeViene; }

    void setConnectionProtocol(QRemoteShell::ConnectionProtocol cp) { m_connectionprotol = cp; }
    void setGW(QString GW);
    void setRemoteShell(QRemoteShell *remoteShell) { term = remoteShell; }
    void setOptions(unsigned int);
    void setIpOInterfazMismoDominioOSPFDondeSeViene( QString interfaz ) { m_ipOinterfazMismoDominioOSPFdondeSeViene=interfaz; } //
    void setUser2(QString user) { m_user2=user; }
    void setPassword2(QString pwd) { m_pwd2=pwd; }

    static QMap<QString,QString> queriesArgumentosAceptados();
    static void updateInfoQueries( QList<Queries*> &lstDest, QList<Queries*> &lstOrigin, QStringList lstIPsConsulta );

    void createEmptyQueries();
    void clear();
    void startSync();

public slots:
    virtual void start();

protected slots:
    void processConnectToHostReachable();
    void processConnectToHostConnected();
    void processConnectToHostDisconnected();
    void processPlatform();
    void processConfigFinished();
    void processFinished();
    void processKeepWorking();
    void on_m_keepAliveTimer_timeout();
    void on_query_lastCommand(QString);
    void on_queryTimer_timeout();

signals:
    void lastCommand(Queries*,QString);
    void finished(Queries*);

protected:
    Queries *m_qry;
    QRemoteShell *term;
    unsigned int flags;
    unsigned int opcionActual;
    QTimer *queryTimer;
    bool m_queriescreated;
    bool m_contieneconsultas;
    short int m_consultaIntentos;
    bool m_reintentandoConsulta;
    QRemoteShell::ConnectionProtocol m_connectionprotol;
    QString m_ip;
    QTimer *m_keepAliveTimer;
    QString m_gw;
    QString m_user;
    QString m_user2;
    QString m_pwd;
    QString m_pwd2;
    QString m_linuxprompt;
    bool m_conectionSecondTriedOtherUserPassword;
    QString m_ipOinterfazMismoDominioOSPFdondeSeViene;

    void _iniciar();
    void _createQueries(QueriesDAO::Opcion option = QueriesDAO::Null);
    void _conectarAequipo(QString ip, QString user, QString pwd, QString platform, QString linuxprompt);
    void _nextProcess();
    void _borrarTerminal();
    void _limpiarConsultas();

    //despues de terminar una consulta se crean las funciones que no se configuraron
    //por si el usuario trata a accesder a ellas tenga consultas vacias
    void _crearFuncionesFaltantes();
};

#endif // QUERIESDAO_H
