#include "queriesmessagehandler.h"
#include <QMutex>
#include <QDebug>

QueriesMessageHandler *QueriesMessageHandler::m_instance = nullptr;

void QueriesMessageHandler::initHandler(QString path)
{
    if ( m_instance )
        delete m_instance;

    m_instance = new QueriesMessageHandler(path);
}

QueriesMessageHandler* QueriesMessageHandler::instance()
{
    return m_instance;
}

QueriesMessageHandler::QueriesMessageHandler(QString path)
{
    _path = path;
    _expIP.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");

    _dbgFie.setFileName( path+"/ConsultaEquipos_debug.txt" );
    _dbgFie.open(QIODevice::WriteOnly | QIODevice::Append);
    _dbgS.setDevice( &_dbgFie );

    _otherFie.setFileName( path+"/ConsultaEquipos_other.txt" );
    _otherFie.open(QIODevice::WriteOnly | QIODevice::Append);
    _otherS.setDevice( &_otherFie );

    _c=0;
}

QueriesMessageHandler::~QueriesMessageHandler()
{
    for( log *l : _lstLogs )
        l->file->close();
    _dbgFie.close();
    _otherFie.close();

    qDeleteAll( _lstLogs );
}

void QueriesMessageHandler::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)

    _mutex.lock();

    switch (type) {
    case QtInfoMsg:
    {
        printf( qPrintable( QString(context.category)+": "+msg+"\n"));
        break;
    }
    case QtWarningMsg:
    case QtCriticalMsg:
    case QtFatalMsg:
    {
        _otherS << msg << endl;
        _otherS.flush();
        break;
    }
    case QtDebugMsg:
    {
        QStringList data = msg.split(" ",QString::SkipEmptyParts);
        if ( !data.isEmpty() )
        {
            if ( data.first().contains( _expIP ) )
            {
                QString ip = data.takeFirst().replace("\"","");

//                if ( lstIPsfinalizadas.contains(ip) )
//                {
//                    mutex.unlock();
//                    return;
//                }

                log *l = nullptr;
                for ( log* ll : _lstLogs )
                {
                    if ( ll->ip == ip )
                    {
                        l=ll;
                        break;
                    }
                }
                if ( !l )
                {
                    //para los casos donde se intenta conectar por medio de un gw, para que se adjunte al archivo de
                    //la conexion anterior
                    QString numero = _mapIPnumero.value( ip );
                    if ( numero.isEmpty() )
                    {
                        _c++;
                        numero = QString::number(_c);
                        _mapIPnumero.insert( ip,numero );
                    }

                    log* ll = new log;
                    ll->ip = ip;
                    QFile *f = new QFile( _path+"/"+numero+"_"+ip+".txt" );
                    f->open(QIODevice::WriteOnly | QIODevice::Append);
                    QTextStream *s = new QTextStream( f );
                    ll->file = f;
                    ll->ts = s;
                    _lstLogs.append(ll);
                    l=ll;
                }
                *l->ts << data.join(" ") << endl;
                l->ts->flush();

                //cerrando archivo si finalizo la consulta en el equipo
                if ( msg.contains("QRemoteShell::~QRemoteShell()") )
                {
                    l->file->close();
//                    l->file->remove();
                    l->file->deleteLater();
                    _lstLogs.removeOne( l );
//                    lstIPsfinalizadas.append(ip);

                    _dbgS << "lstLogs size" << _lstLogs.size() << endl;
                    _dbgS.flush();
                }
            }
            else
            {
                _dbgS << msg << endl;
                _dbgS.flush();
            }
        }
        break;
    }
    }

    _mutex.unlock();
}

void queriesMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QueriesMessageHandler::instance()->messageHandler( type, context, msg );
}
