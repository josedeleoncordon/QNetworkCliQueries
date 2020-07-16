#ifndef QUERIESTHREADWORKER_H
#define QUERIESTHREADWORKER_H

#include <QObject>
#include "queries.h"

class QueriesThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit QueriesThreadWorker(Queries *qry);

public slots:
    void start();

signals:
    void finished();

private:
    Queries *_qry;

};

#endif // QUERIESTHREADWORKER_H
