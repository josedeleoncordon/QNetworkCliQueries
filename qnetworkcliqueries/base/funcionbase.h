#ifndef FUNCIONBASE_H
#define FUNCIONBASE_H

#include <QObject>
#include <QString>
#include <QDataStream>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QFile>

#include "qnetworkcliqueries_global.h"
#include "qremoteshell.h"
#include "funciones.h"

/*Todas las funciones se basan en esta clase.
 * Esta programado para que al ser usadas por Queries se realicen las consultas y se guarden en un archivo.
 * Al abrir el archivo unicamente serán para consulta, no para volver a consultar.
 * Para una nueva consulta se utilizan nuevas instancias y se fucionan con las abiertas de un archivo.
*/

class Queries;

struct InfoBase
{
    QDateTime datetime;
    bool operativo;
    Queries* queryParent;   

    InfoBase() { operativo=true; queryParent=nullptr; }
};

class QNETWORKCLIQUERIES_EXPORT QueriesConfiguration
{
private:
    static QueriesConfiguration* m_instance;
public:
    QueriesConfiguration();
    static QueriesConfiguration *instance();

    QMap<QString,QString> mapQueries;
    QStringList mapQueriesToList(QString value);

    QMap<QString,QString> mapConfigurations;

    int opciones;
};

class QNETWORKCLIQUERIES_EXPORT FuncionBase : public QObject
{
    Q_OBJECT
protected:
    QRemoteShell *term;
    QString txt;
    QRegExp exp, exp2;
    QString m_brand;
    QString m_platform;
    QString m_os;
    QString m_xr_location;
    QString m_name; //para debug
    QString m_ip; //para debug
    Queries* m_queriesParent;
    bool debug;    

    //debug
    QString m_logPath;
    QFile m_logFile;
    QTextStream m_out;

    QString m_lastCommand;

    void termSendText(QString str);
    void finished();
    bool allTextReceived();
    bool lastCommandFailed;
    void saveLog(QString);

public:
    FuncionBase(QRemoteShell *terminal,QObject *parent = 0);
    ~FuncionBase();

    void setBrand(QString brand) { m_brand = brand; }
    void setPlatform(QString platform);
    void setXRLocation(QString location) { m_xr_location=location; }
    void setHostName(QString name) { m_name=name; }
    void setIp(QString ip) { m_ip = ip; }
    void setLogPath(QString path);

signals:
    void lastCommand(QString);
    void processFinished();
    void working();
};

Q_DECLARE_METATYPE(FuncionBase*);

#endif
