#include "queriesthreadworker.h"

QueriesThreadWorker::QueriesThreadWorker()
{
    qry=nullptr;
}

void QueriesThreadWorker::start()
{
    qry->startSync();
    emit finished(qry);
}

QueriesThreadWorker *newQueriesThreadWorker()
{
    return new QueriesThreadWorker;
}
