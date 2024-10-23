#ifndef QTELNET_H
#define QTELNET_H

#include <QTcpSocket>
#include <QTimer>
#include <QStringList>
#include "qremoteshell_global.h"

class QREMOTESHELLSHARED_EXPORT QTelnet : public QObject
{
    Q_OBJECT
private:

    // Commands

    const char CEOF  = 236;
    const char SUSP  = 237;
    const char ABORT = 238;
    const char SE    = 240;
    const char NOP   = 241;
    const char DM    = 242;
    const char BRK   = 243;
    const char IP    = 244;
    const char AO    = 245;
    const char AYT   = 246;
    const char EC    = 247;
    const char EL    = 248;
    const char GA    = 249;
    const char SB    = 250;
    const char WILL  = 251;
    const char WONT  = 252;
    const char DO    = 253;
    const char DONT  = 254;
    const char IAC   = 255;

    // Types
    const char IS    = 0;
    const char SEND  = 1;

    const char Authentication = 37; // RFC1416,
                                    // implemented to always return NULL
    const char SuppressGoAhead = 3; // RFC858
    const char Echo = 1; // RFC857, not implemented (returns WONT/DONT)
    const char LineMode = 34; // RFC1184, implemented
    const char LineModeEOF = 236, // RFC1184, not implemented
                LineModeSUSP = 237,
                LineModeABORT = 238;
    const char Status = 5; // RFC859, should be implemented!
    const char Logout = 18; // RFC727, implemented
    const char TerminalType = 24; // RFC1091,
                                  // implemented to always return UNKNOWN
    const char NAWS = 31; // RFC1073, implemented
    const char TerminalSpeed = 32; // RFC1079, not implemented
    const char FlowControl = 33; // RFC1372, should be implemented?
    const char XDisplayLocation = 35; // RFC1096, not implemented
    const char EnvironmentOld = 36; // RFC1408, should not be implemented!
    const char Environment = 39; // RFC1572, should be implemented
    const char Encrypt = 38; // RFC2946, not implemented

    QString _ip;
    QString _port;
    QTcpSocket *socket;
    QMap<char, bool> modes;
    int uc,pc;
    bool sendAllData;
    QString m_dataReceived;

    int parseIAC(QByteArray data);    
    bool isOperation(const uchar c);
    bool replyNeeded(uchar operation, uchar option);
    bool allowOption(int /*oper*/, int opt);
    uchar opposite(uchar operation, bool positive);
    void setMode(uchar operation, uchar option);
    QByteArray getSubOption(const QByteArray &data);
    void parseSubTT(const QByteArray &data);
    void sendData(const char operation, const char option);

public:
    explicit QTelnet(QString ip, QString port, QObject *parent = 0);
    virtual ~QTelnet();

    void host_connect();
    void host_disconnect();
    void setSendAllData(bool send) { sendAllData=send; }
    void sendData(const QByteArray &command);
    void sendCommand(QString str);
    QString& dataReceived() { return m_dataReceived; }

private slots:
    void socketConnected();
    void on_socket_readyRead();

signals:
    void connected();
    void disconnected();
    void readyRead();
};

#endif // QTELNET_H
