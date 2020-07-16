#include "queriesthreadworker.h"

QueriesThreadWorker::QueriesThreadWorker(Queries *qry) : QObject(qry)
{
    _qry=qry;
}

void QueriesThreadWorker::start()
{
    _qry->startSync();	
	emit finished(_qry);
}
