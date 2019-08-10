#include "placeinfo.h"

PlaceInfo::PlaceInfo(QRemoteShell *terminal, QObject *parent):
    FuncionBase(terminal,parent)
{}

void PlaceInfo::getPlaceInfo()
{
    if ( m_platform != "" )
    {
        finished();
        return;
    }

    connect(term,SIGNAL(receiveText(QString)),SLOT(on_term_receiveText(QString)));
    termSendText("show ");
}

void PlaceInfo::on_term_receiveText(QString str)
{
    txt.append(str);
    if ( !allTextReceived() )
        return;

    //
    //

    finished();
}

QDataStream& operator<<(QDataStream& out, const PlaceInfo* info)
{
    out << info->place;
    return out;
}

QDataStream& operator>>(QDataStream& in, PlaceInfo*& info)
{
    info =new PlaceInfo(NULL,NULL);
    in >> info->place;
    return in;
}

QDebug operator<<(QDebug dbg, const PlaceInfo &info)
{
    dbg.nospace() << "PlaceInfo:" << info.place << "\n";
    dbg.nospace() << "\n";

    return dbg.maybeSpace();
}
