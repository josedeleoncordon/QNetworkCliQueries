#ifndef BDHOSTSUPDATER_H
#define BDHOSTSUPDATER_H

#include <QObject>

#include "prtlfunc_global.h"

class QueriesThread;

class QPRTLFUNCSHARED_EXPORT BDHostsUpdater : public QObject
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
    QStringList lstPaises;
    QString pais;
    int lstPaisesPos;
    QStringList lstPEIPsGeneral;

    void consultarPEs_otroPais();    
};

#endif // BDHOSTSUPDATER_H
