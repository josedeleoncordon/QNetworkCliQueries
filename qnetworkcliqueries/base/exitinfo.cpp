#include "exitinfo.h"
#include "properties.h"

ExitInfo::ExitInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{
    m_gw = false;
}

void ExitInfo::exit()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));
    if ( m_brand == "Huawei" || m_brand == "MikroTik" )
        termSendText("quit");
    else
        termSendText("exit");
}

void ExitInfo::on_term_receiveText()
{
    txt.append(term->dataReceived());
    // qDebug() << m_ip << "ExitInfo::on_term_receiveText()" << txt;
    // if ( !txt.contains(QRegularExpression(Properties::Instance()->linuxpromt)) &&
    //      !txt.contains(QRegularExpression("^.+#\\s*$") ) )
    //     return;

    // if ( m_gw )
    // {
    //     //TODO asumimos que el GW y este equipo son de la misma marca
    //     qDebug() << m_ip << "equipo conectado por GW, se sale tambien de ese";
    //     m_gw = false;
    //     exit();
    //     return;
    // }

    qDebug() << "ExitInfo::on_term_receiveText()" << txt;

    connect(term,SIGNAL(disconnected()),SLOT(on_term_disconnected()));
    term->host_disconnect();
}

void ExitInfo::on_term_disconnected()
{
    finished();
}
