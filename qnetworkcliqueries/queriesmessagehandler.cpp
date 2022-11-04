#include "queriesmessagehandler.h"
#include <QMutex>
#include <QDebug>
#include <QDir>
#include <QDateTime>

QueriesMessageHandler *QueriesMessageHandler::m_instance = nullptr;

void QueriesMessageHandler::initHandler(QString path, bool savelogs)
{
    if ( m_instance )
        delete m_instance;

    m_instance = new QueriesMessageHandler(path,savelogs);
}

QueriesMessageHandler* QueriesMessageHandler::instance()
{
    return m_instance;
}

QueriesMessageHandler::QueriesMessageHandler(QString path,bool savelogs)
{
    _path = path;
    _savelogs = savelogs;
    _expIP.setPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
    _expIP.setMinimal(false);

    init();

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

void QueriesMessageHandler::init()
{
    QDir dir(_path);
    dir.setNameFilters(QStringList() << "*.log");
    dir.setFilter(QDir::Files);
    for (QString dirFile : dir.entryList())
        dir.remove(dirFile);

    close();

    _dbgFie.setFileName( _path+"/ConsultaEquipos_debug.log" );
    _dbgFie.open(QIODevice::WriteOnly | QIODevice::Append);
    _dbgS.setDevice( &_dbgFie );

    _otherFie.setFileName( _path+"/ConsultaEquipos_other.log" );
    _otherFie.open(QIODevice::WriteOnly | QIODevice::Append);
    _otherS.setDevice( &_otherFie );

    _c=0;
}

void QueriesMessageHandler::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)

    if ( !_savelogs )
        return;

    _mutex.lock();

    switch (type) {
    case QtInfoMsg:
    {
        printf( qPrintable( QString(context.category)+": "+msg+"\n") );
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
//            if ( false )
            if ( _expIP.exactMatch(data.first().replace("\"","")) )
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
                    QFile *f = new QFile( _path+"/"+numero+"_"+ip+".log" );
                    f->open(QIODevice::WriteOnly | QIODevice::Append);
                    QTextStream *s = new QTextStream( f );
                    ll->file = f;
                    ll->ts = s;
                    _lstLogs.append(ll);
                    l=ll;
                }
                *l->ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " " << data.join(" ") << endl;
                l->ts->flush();

                //cerrando archivo si finalizo la consulta en el equipo
                if ( msg.contains("QueriesThread::equipoConsultado FIN") )
                {
                    l->file->close();
//                    l->file->remove();
                    l->file->deleteLater();
                    _lstLogs.removeOne( l );
//                    lstIPsfinalizadas.append(ip);                    

                    _dbgS << "Cerrando archivo log " << l->ip << endl;
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

void QueriesMessageHandler::close()
{
    for ( log* ll : _lstLogs )
    {
        ll->file->close();
        ll->file->deleteLater();
    }
    _lstLogs.clear();

    if ( _otherFie.isOpen() )
    {
        _otherFie.close();
        _dbgFie.close();
    }
}

void queriesMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QueriesMessageHandler::instance()->messageHandler( type, context, msg );
}
