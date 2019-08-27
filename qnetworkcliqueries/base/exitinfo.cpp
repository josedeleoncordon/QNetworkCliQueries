#include "exitinfo.h"
#include "properties.h"

ExitInfo::ExitInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{}

void ExitInfo::exit()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));
    if ( m_brand == "Cisco" )
        termSendText("exit");
    else if ( m_brand == "Huawei" )
        termSendText("quit");
    else
        finished();
}

void ExitInfo::on_term_receiveText()
{
    txt.append(term->dataReceived());
    if ( !txt.contains(QRegExp(Properties::Instance()->linuxpromt)) )
        return;

    connect(term,SIGNAL(disconnected()),SLOT(on_term_disconnected()));
    term->host_disconnect();
}

void ExitInfo::on_term_disconnected()
{
    finished();
}
