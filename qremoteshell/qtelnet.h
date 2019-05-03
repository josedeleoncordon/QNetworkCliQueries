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

    //TODO cambiar los uchar a char

    const uchar CEOF  = 236;
    const uchar SUSP  = 237;
    const uchar ABORT = 238;
    const uchar SE    = 240;
    const uchar NOP   = 241;
    const uchar DM    = 242;
    const uchar BRK   = 243;
    const uchar IP    = 244;
    const uchar AO    = 245;
    const uchar AYT   = 246;
    const uchar EC    = 247;
    const uchar EL    = 248;
    const uchar GA    = 249;
    const uchar SB    = 250;
    const uchar WILL  = 251;
    const uchar WONT  = 252;
    const uchar DO    = 253;
    const uchar DONT  = 254;
    const uchar IAC   = 255;

    // Types
    const uchar IS    = 0;
    const uchar SEND  = 1;

    const uchar Authentication = 37; // RFC1416,
                                    // implemented to always return NULL
    const uchar SuppressGoAhead = 3; // RFC858
    const uchar Echo = 1; // RFC857, not implemented (returns WONT/DONT)
    const uchar LineMode = 34; // RFC1184, implemented
    const uchar LineModeEOF = 236, // RFC1184, not implemented
                LineModeSUSP = 237,
                LineModeABORT = 238;
    const uchar Status = 5; // RFC859, should be implemented!
    const uchar Logout = 18; // RFC727, implemented
    const uchar TerminalType = 24; // RFC1091,
                                  // implemented to always return UNKNOWN
    const uchar NAWS = 31; // RFC1073, implemented
    const uchar TerminalSpeed = 32; // RFC1079, not implemented
    const uchar FlowControl = 33; // RFC1372, should be implemented?
    const uchar XDisplayLocation = 35; // RFC1096, not implemented
    const uchar EnvironmentOld = 36; // RFC1408, should not be implemented!
    const uchar Environment = 39; // RFC1572, should be implemented
    const uchar Encrypt = 38; // RFC2946, not implemented

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
