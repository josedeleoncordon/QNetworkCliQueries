#ifndef QUERIESTHREADCONTROLLER_H
#define QUERIESTHREADCONTROLLER_H

#include <QObject>
#include <QStringList>
#include <QMutex>
#include <QMap>

//Para una misma aplicación que puede recibir multiples solicitudes, controla la cantidad de conexiones a por equipo
//y las conexiones totales a los equipos de la red.

class QueriesThreadController : public QObject
{
    Q_OBJECT
public:
    static QueriesThreadController *Instance();
    void setMaxParalelos(int max) { m_maxparalelos = max; }

    //Para un equipo raiz que descubre sus vecinos y la cantidad de equipos no se conoce
    void registerQueriesThreadUUID(QString uuid);
    void registerQueriesThreadUUID(QString uuid, int cantidadEquipos); //cantidad de equipos conocida
    void unregisterQueriesThreadUUID(QString uuid);

    bool consultarAgregarEquipo(QString uuid, QString ip);
    void consultaFinalizada(QString uuid, QString ip);

private:
    static QueriesThreadController *m_instance;
    QStringList m_equiposenconsultaGeneral;
    short int m_maxparalelos;  //cantidad total de consultas simultaneas
    short int m_maxparalelosfijos; //cantidad total permitida para conexiones paralelas de solicitudes con lista fija. Debe se menor a m_maxparalelos
    short int m_maxparalelosmismoequipo;

    //cantidad maxima de conexiones permitidas por solicitud, una pagina web por ejemplo (Para listado fijo de equipos)    
    QMap<QString,short> m_mapUUIDFijoCantidadRegistrada; //cantidad de conexiones solicitadas por un uuid
    QMap<QString,short> m_mapUUIDFijoCantidadPermitida; //cantidad de conexiones pemitidas por un uuid despues de calculo
    QMap<QString,short> m_mapUUIDParalelosFijo; //cantidad de conexiones en uso por uuid
    //cantidad maxima de conexiones permitidas por solicitud, una pagina web por ejemplo (Para equipo raiz con descubrimiento)    
    QMap<QString,short> m_mapUUIDParalelosDescubrimiento; //cantidad de conexiones en uso por uuid
    short int m_maxparalelosuuiddescrubimiento;

    QMutex m_mutex;    

    explicit QueriesThreadController();
    ~QueriesThreadController();

    //cuando se agregar o elimina una solicitud se calcula cuantas conexiones a equipos se permite
    void m_calcularMaxParalelosUUID();
    void emitStatus();

signals:
    void status(QString);

public slots:
    void serverstatus();
};

#endif // QUERIESTHREADCONTROLLER_H
