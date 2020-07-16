#include "queriesthreadworker.h"

QueriesThreadWorker::QueriesThreadWorker(Queries *qry) : QObject(qry)
{
    _qry=qry;
    _qry->setParent(this);
}

void QueriesThreadWorker::start()
{
    _qry->startSync();
    emit finished();
}
