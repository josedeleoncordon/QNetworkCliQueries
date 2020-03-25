#include "funcioninfo.h"

FuncionInfo::FuncionInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{}

FuncionInfo::FuncionInfo(const FuncionInfo &other):
    FuncionBase(other.term,other.parent())
{
    _txt = other._txt;
}

FuncionInfo::~FuncionInfo()
{}

void FuncionInfo::getTXT()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));

    QString txt = QueriesConfiguration::instance()->value( "FuncionInfo_txt",m_ip );

    termSendText(txt);
}

void FuncionInfo::on_term_receiveText()
{
    txt.append( term->dataReceived() );
    if ( !allTextReceived() )
        return;

    _txt = txt;

    finished();
}

QDataStream& operator<<(QDataStream& out, const FuncionInfo* info)
{
    out << info->_txt;
    return out;
}

QDataStream& operator>>(QDataStream& in, FuncionInfo*& info)
{
    info =new FuncionInfo(nullptr,nullptr);
    in >> info->_txt;
    return in;
}

QDebug operator<<(QDebug dbg, const FuncionInfo &info)
{
    dbg.nospace() << "FuncionInfo:\n";
    dbg.space().noquote() << info._txt << "\n";
    dbg.space() << "\n";

    return dbg.maybeSpace();
}
