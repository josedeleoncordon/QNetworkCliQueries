#include "funcioninfo.h"

FuncionInfo::FuncionInfo(QRemoteShell *terminal, int option):
    FuncionBase(terminal,option)
{}

FuncionInfo::FuncionInfo(const FuncionInfo &other):
    FuncionBase(other.term,other.m_queryoption)
{
    m_brand = other.m_brand;
    m_platform = other.m_platform;
    m_os = other.m_os;
    m_name = other.m_name;
    m_ip = other.m_ip;
    _lstTxt = other._lstTxt;
}

FuncionInfo::~FuncionInfo()
{}

void FuncionInfo::getTXT()
{
    qDebug() << m_ip << "FuncionInfo::getTXT()" << m_os << m_conexionID;
    qDebug() << "FuncionInfo::getTXT()" << m_ip << m_os << m_conexionID;    

    connect(term,SIGNAL(readyRead()),SLOT(on_term_receiveText()));

    if ( m_os == "IOS XR" && m_xr64 )
        //si es XR de 64 bits vemos si hay configuraciones especificas para este. Si no abajo buscara configuraciones para XR general
        _lstFunciones = m_queriesConfiguration.values( "FuncionInfo_txt",m_ip,"IOS XR64",m_conexionID,m_queryName );

    if ( _lstFunciones.isEmpty() )
        _lstFunciones = m_queriesConfiguration.values( "FuncionInfo_txt",m_ip,m_os,m_conexionID,m_queryName );

    qDebug() << m_ip << "FuncionInfo::getTXT()" << _lstFunciones;
    qDebug() << "FuncionInfo::getTXT()" << m_ip << _lstFunciones;

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
    out << info.m_queryoption;
    out << info.m_queryName;
    return out;
}

QDataStream& operator>>(QDataStream& in, FuncionInfo& info)
{
    in >> info._lstTxt;
    in >> info.m_queryoption;
    in >> info.m_queryName;
    return in;
}

QDataStream& operator<<(QDataStream& out, const FuncionInfo* info)
{
    out << *info;
    return out;
}

QDataStream& operator>>(QDataStream& in, FuncionInfo*& info)
{
    info = new FuncionInfo;
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
