#ifndef BDHOSTSUPDATER_H
#define BDHOSTSUPDATER_H

#include <QObject>

#include "qnetworkcliqueries_global.h"

/*Analiza los archivos de backup de un arbol de directorios, Directorio grupo y directorios hijos como subgrupos*/

class QueriesThread;

class QNETWORKCLIQUERIES_EXPORT BDHostsUpdater : public QObject
{
    Q_OBJECT
public:
    explicit BDHostsUpdater(QObject *parent = nullptr);
    void updateDB(QString path);

signals:
    void finished();

private slots:
    void on_actualizarPEs_finished(bool ok);
    void on_actualizarPEsPs_finished();
    void on_actualizarETCHosts_finished();
    void on_actualizarHostALLIPs_finished();

private:
    QString m_path;
    QueriesThread *queriesThread;
    QStringList lstGrupos;
    QString grupo;
    int lstGruposPos;
    QStringList lstPEIPsGeneral;

    void consultarPEs_otroGrupo();    
};

#endif // BDHOSTSUPDATER_H
