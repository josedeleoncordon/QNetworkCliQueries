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
    _expIP.setPattern(QRegularExpression::anchoredPattern("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}"));

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

    _time = QTime::currentTime();

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
        _otherS << msg << Qt::endl;
        _otherS.flush();
        break;
    }
    case QtDebugMsg:
    {
        QString dt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QStringList data = msg.split(" ",Qt::SkipEmptyParts);                

        if ( !data.isEmpty() )
        {
            short int min;
            short int seg;
            QTime current = QTime::currentTime();
            short int secto = _time.secsTo( current );
            min = secto/60;
            seg = secto%60;

            // if ( false )
            if ( data.first().replace("\"","").contains(_expIP) )
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

                *l->ts << QDateTime::currentDateTime().toString("hh:mm:ss") <<
                          " Time min: " << QString::number(min)+" seg: " << QString::number(seg) << " " <<
                    msg << Qt::endl;

                l->ts->flush();

                //cerrando archivo si finalizo la consulta en el equipo
                if ( msg.contains("QueriesThread::equipoConsultado FIN") )
                {
                    l->file->close();
//                    l->file->remove();
                    l->file->deleteLater();
                    _lstLogs.removeOne( l );
//                    lstIPsfinalizadas.append(ip);

                    _dbgS << "Cerrando archivo log " << l->ip << Qt::endl;
                    _dbgS << "lstLogs size" << _lstLogs.size() << Qt::endl;
                    _dbgS.flush();
                }
            }
            else
            {
                _dbgS << QDateTime::currentDateTime().toString("hh:mm:ss") <<
                         " Time min: " << QString::number(min)+" seg: " << QString::number(seg) << " " <<
                    msg << Qt::endl;
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
