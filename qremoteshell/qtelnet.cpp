#include "qtelnet.h"

QTelnet::QTelnet(QString ip, QString port, QObject *parent):
    QObject(parent)
{
    socket = NULL;
    sendAllData=false;
    _ip = ip;
    _port = port;    

    uc = pc = 0;
}

QTelnet::~QTelnet()
{}

void QTelnet::host_connect()
{
    if ( socket )
        return;

    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()),SLOT(socketConnected()));
    connect(socket, SIGNAL(disconnected()),SIGNAL(disconnected()));
    connect(socket, SIGNAL(readyRead()),SLOT(on_socket_readyRead()));

    //Se conecta al equipo, si se logra recibiremos texto en onDataRecv para realizar la autenticacion

    //this is not blocking call
    socket->connectToHost( _ip,_port.toInt() );

    //we need to wait...
    if(!socket->waitForConnected(5000))
        host_disconnect();    
}

void QTelnet::host_disconnect()
{
    if ( socket )
    {
        //se elimina el socket
        socket->disconnectFromHost();
        socket->disconnect();
        socket->deleteLater();
        socket = NULL;
        emit disconnected();
    }    
}

void QTelnet::socketConnected()
{
    emit connected();
    sendData(DO,SuppressGoAhead);
    sendData(WILL,LineMode);
    sendData(DO,Status);
}

void QTelnet::sendData(const char operation, const char option)
{
    const char c[3] = { IAC, operation, option };
    QByteArray a(c, 3);
    sendData(a);
}

void QTelnet::sendData(const QByteArray &command)
{
    if ( socket )
        socket->write(command);    
}

void QTelnet::on_socket_readyRead()
{
    const QByteArray data = socket->readAll();
    m_dataReceived = QString::fromStdString(data.toStdString());

    int currpos = 0;
    int prevpos = -1;        

    if ( sendAllData )
    {
        emit readyRead();
        return;
    }    

    while (prevpos < currpos && currpos < data.size())
    {
        prevpos = currpos;
        const uchar c = uchar(data[currpos]);

        if (c == DM)
            ++currpos;
        else if (c == IAC )
            currpos += parseIAC(data.mid(currpos));
        else // asumimos que es texto plano
        {
            QString txt = data.mid(currpos);
            currpos += txt.size();            

            if ( txt.isEmpty() )
                return;

            emit readyRead();
        }
    }
}

int QTelnet::parseIAC(QByteArray data)
{
    if (data.isEmpty())
        return 0;

    if (data.size() >= 3 && isOperation(data[1]) )
    {
        // IAC, Operation, Option
        const uchar operation = data[1];
        const uchar option = data[2];

        if (replyNeeded(operation, option))
        {
            bool allowed = allowOption(operation, option);

            sendData(opposite(operation, allowed), option);
            setMode(operation, option);

//            if (option == NAWS && modes[NAWS])
//                sendWindowSize();
        }
        return 3;
    }

    if (data.size() >= 2 && data[1] == DM )
    { // IAC Command
        return 2;
    }

    QByteArray suboption = getSubOption(data);
    if (suboption.isEmpty())
        return 0;

    // IAC SB Operation SubOption [...] IAC SE

    if ( suboption[0] == TerminalType )
        parseSubTT(suboption);
//    else if ( suboption[0] == NAWS )
//        parseSubNAWS(data);
    else
        qWarning("QtTelnetPrivate::parseIAC: unknown suboption %d",
                 quint8(suboption.at(0)));

    return suboption.size() + 4;
}

bool QTelnet::isOperation(const uchar c)
{
    return (c == WILL || c == WONT
            || c == DO ||c == DONT);
}

bool QTelnet::replyNeeded(uchar operation, uchar option)
{
    if (operation == DO || operation == DONT) {
        // RFC854 requires that we don't acknowledge
        // requests to enter a mode we're already in
        if (operation == DO && modes[option])
            return false;
        if (operation == DONT && !modes[option])
            return false;
    }
    return true;
}

bool QTelnet::allowOption(int /*oper*/, int opt)
{
    if (opt == SuppressGoAhead ||
            opt == LineMode ||
            opt == Status ||
            opt == Logout ||
            opt == TerminalType ||
            opt == Echo )
        return true;
    if (opt == NAWS /*&& q->isValidWindowSize()*/ )
        return true;
    return false;
}

uchar QTelnet::opposite(uchar operation, bool positive)
{
    if (operation == DO)
        return (positive ? WILL : WONT);
    else if (operation == DONT) // Not allowed to say WILL
        return WONT;
    else if (operation == WILL)
        return (positive ? DO : DONT);
    else if (operation == WONT) // Not allowed to say DO
        return DONT;
    return 0;
}

void QTelnet::setMode(uchar operation, uchar option)
{
    if (operation != DO && operation != DONT)
        return;

    modes[option] = (operation == DO);
}

QByteArray QTelnet::getSubOption(const QByteArray &data)
{
    if (data.size() < 4 || uchar(data[1]) != SB)
        return QByteArray();

    for (int i = 2; i < data.size() - 1; ++i) {
        if (uchar(data[i]) == IAC && uchar(data[i+1]) == SE) {
            return data.mid(2, i-2);
        }
    }
    return QByteArray();
}

void QTelnet::parseSubTT(const QByteArray &data)
{
    if (data.size() < 2 || data[1] != SEND)
        return;

    const char c1[4] = { IAC, SB,
                         TerminalType, IS };

    QByteArray a(c1,sizeof(c1));
    sendData(a);
    sendData("vt100");
    const char c2[2] = { IAC, SE };
    QByteArray a2(c2,sizeof(c2));
    sendData(a2);
}

void QTelnet::sendCommand(QString str)
{
    if ( socket )
    {
        socket->write(str.toLocal8Bit());
        socket->write("\n");
    }
}
