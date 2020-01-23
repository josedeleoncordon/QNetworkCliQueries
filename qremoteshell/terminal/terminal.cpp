#include "terminal.h"

#include <QDir>
#include <QDebug>

#include "Pty.h"

#include "../qremoteshelllogging.h"

using namespace Konsole;

Terminal::Terminal(QString debugIP, QString linuxprompt, QObject *parent) : QObject(parent)
{
    _debugIP = debugIP;

    _ready=false;
    _shellProcess = new Pty();

    connect( _shellProcess,SIGNAL(receivedData(const char *,int)),this,
             SLOT(onReceiveBlock(const char *,int)) );
    connect( _shellProcess,SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(shellProcess_finished()) );

    m_linuxprompt = linuxprompt;

    _timer=new QTimer;
    _timer->setInterval(3000);
    _timer->setSingleShot(true);
    connect(_timer,SIGNAL(timeout()),SLOT(on_timerout()));

    _shellProcess->setUtf8Mode( true );
    _shellProcess->setWorkingDirectory(QDir::homePath());
    _shellProcess->setFlowControlEnabled(true);   

    QString exec = QString::fromLocal8Bit(qgetenv("SHELL"));
    QFile excheck(exec);
    excheck.setFileName(exec);

    _timer->start();

    int result = _shellProcess->start(exec,
                                      QStringList() << exec,
                                      QStringList() << QLatin1String("COLORFGBG=15;0"),
                                      0,
                                      false);

    if (result < 0)
    {
        qCDebug(terminal) << _debugIP << "SHELL CRASHED! result: " << result;
        return;
    }
    _shellProcess->setWriteable(false);  // We are reachable via kwrited.
}

Terminal::~Terminal()
{
    qCDebug(terminal) << _debugIP << "Terminal::~Terminal()";
    delete _timer;
}

void Terminal::onReceiveBlock( const char * buf, int len )
{    
    QString received = QString::fromLatin1( buf, len ).toLatin1();

    if ( !_ready )
    {
        _timer->stop();
        if ( received.split("\n").last().contains(QRegExp(m_linuxprompt)) )
        {
            _ready=true;
            emit ready(true);
        }
    }
    else
        emit receivedData( received );
}

void Terminal::on_timerout()
{
    qCDebug(terminal) << _debugIP << "Terminal::on_timerout()";
    emit ready(false);
}

void Terminal::sendCommand(QString txt)
{
    txt.append("\n");
    sendData( txt.toLocal8Bit() );
}

void Terminal::sendData(const QByteArray &data)
{
    if ( _ready )
        _shellProcess->sendData( data, data.size() );
    else
        qCDebug(terminal) << _debugIP << "Terminal::sendCommand() not ready";
}

void Terminal::shellProcess_finished()
{    
    qCDebug(terminal) << _debugIP << "Terminal::shellProcess_finished()";
    _ready=false;
    _shellProcess->deleteLater();
    emit finished();
}

void Terminal::close()
{            
    //_shellProcess emitira finished y se ejecutara shellProcess_finished()

    _shellProcess->close();
//    int result = ::kill(_shellProcess->pid(),SIGHUP);
//    qDebug() << _debugIP << "Terminal::close()";
}

