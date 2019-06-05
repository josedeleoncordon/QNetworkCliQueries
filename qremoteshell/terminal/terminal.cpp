#include "terminal.h"

#include <QDir>
#include <QDebug>

#include "Pty.h"

using namespace Konsole;

Terminal::Terminal(QObject *parent) : QObject(parent)
{
    _ready=false;
    _shellProcess = new Pty();

    connect( _shellProcess,SIGNAL(receivedData(const char *,int)),this,
             SLOT(onReceiveBlock(const char *,int)) );
    connect( _shellProcess,SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(shellProcess_finished()) );

    _shellProcess->setUtf8Mode( true );
    _shellProcess->setWorkingDirectory(QDir::homePath());
    _shellProcess->setFlowControlEnabled(true);

    QString exec = QString::fromLocal8Bit(qgetenv("SHELL"));
    QFile excheck(exec);
    excheck.setFileName(exec);

    int result = _shellProcess->start(exec,
                                      QStringList() << exec,
                                      QStringList() << QLatin1String("COLORFGBG=15;0"),
                                      0,
                                      false);

    if (result < 0) {
        qDebug() << "SHELL CRASHED! result: " << result;
        return;
    }
    _shellProcess->setWriteable(false);  // We are reachable via kwrited.
}

Terminal::~Terminal()
{
    delete _shellProcess;
}

void Terminal::onReceiveBlock( const char * buf, int len )
{    
    QString received = QString::fromLatin1( buf, len ).toLatin1();

    if ( !_ready )
    {
        if ( received.split("\n").last().contains(QRegExp("\\[.+@.+:.+\\]\\$")) )
        {
            _ready=true;
            emit ready();
        }
    }
    else
        emit receivedData( received );
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
        qDebug() << "Terminal::sendCommand() not ready";
}

void Terminal::shellProcess_finished()
{    
    _ready=false;
    emit finished();
}

void Terminal::terminate()
{
    _shellProcess->terminate();
}

