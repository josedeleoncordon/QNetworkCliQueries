#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>

namespace Konsole {
class Pty;
}

class Terminal : public QObject
{
    Q_OBJECT

private:
    Konsole::Pty *_shellProcess;
    bool _ready;

public:
    explicit Terminal(QObject *parent = nullptr);
    ~Terminal();
    void sendCommand(QString text);
    void sendData(const QByteArray &data);

private slots:
    void onReceiveBlock( const char * buf, int len );
    void shellProcess_finished();    

public slots:
    void terminate();

signals:
    void ready();
    void receivedData(QString);
    void finished();
};

#endif // TERMINAL_H
