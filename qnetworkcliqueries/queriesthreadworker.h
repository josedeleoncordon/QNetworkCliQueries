#ifndef QUERIESTHREADWORKER_H
#define QUERIESTHREADWORKER_H

#include <QObject>
#include "queries.h"

class QueriesThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit QueriesThreadWorker();
    void setQueries(Queries *query) { qry=query; }   

public slots:
    virtual void start();

signals:
    void finished(Queries*);

protected:
    Queries *qry;

};

QueriesThreadWorker *newQueriesThreadWorker();

#endif // QUERIESTHREADWORKER_H
