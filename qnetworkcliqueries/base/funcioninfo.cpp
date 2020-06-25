#include "funcioninfo.h"

FuncionInfo::FuncionInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{}

FuncionInfo::FuncionInfo(const FuncionInfo &other):
    FuncionBase(other.term,other.parent())
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_name = other.m_name;
    m_ip = other.m_ip;
    m_queryoption = other.m_queryoption;
    _lstTxt = other._lstTxt;
}

FuncionInfo::~FuncionInfo()
{}

void FuncionInfo::getTXT()
{
    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));
    _lstFunciones = QueriesConfiguration::instance()->values( "FuncionInfo_txt",m_ip,m_os );
    _siguienteFuncion();
}

void FuncionInfo::_siguienteFuncion()
{
    if ( !_lstFunciones.isEmpty() )
        termSendText( _lstFunciones.takeFirst() );
    else
        finished();
}

void FuncionInfo::on_term_receiveText()
{
    txt.append( term->dataReceived() );
    if ( !allTextReceived() )
        return;

    _lstTxt.append(txt);
    _siguienteFuncion();
}

QDataStream& operator<<(QDataStream& out, const FuncionInfo& info)
{
    out << info._lstTxt;
    return out;
}

QDataStream& operator>>(QDataStream& in, FuncionInfo& info)
{
    in >> info._lstTxt;
    return in;
}

QDataStream& operator<<(QDataStream& out, const FuncionInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, FuncionInfo*& info)
{
    info = new FuncionInfo(nullptr,nullptr);
    in >> *info;
    return in;
}

QDebug operator<<(QDebug dbg, const FuncionInfo &info)
{
    dbg.nospace() << "FuncionInfo:\n";
    dbg.space().noquote() << info._lstTxt << "\n";
    dbg.space() << "\n";

    return dbg.maybeSpace();
}
