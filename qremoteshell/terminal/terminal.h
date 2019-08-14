#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QTimer>

namespace Konsole {
class Pty;
}

class Terminal : public QObject
{
    Q_OBJECT

private:
    Konsole::Pty *_shellProcess;
    bool _ready;
    QTimer *_timer;   

    QString _debugIP;

public:
    explicit Terminal(QString debugIP, QObject *parent = nullptr);
    ~Terminal();
    void sendCommand(QString text);
    void sendData(const QByteArray &data);
    void close();        

private slots:
    void onReceiveBlock( const char * buf, int len );
    void on_timerout();
    void shellProcess_finished();    

public slots:    

signals:
    void ready(bool);
    void receivedData(QString);
    void finished();
};

#endif // TERMINAL_H
