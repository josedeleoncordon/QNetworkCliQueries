#ifndef PLACEINFO_H
#define PLACEINFO_H

#include "base/funcionbase.h"

class PlaceInfo : public FuncionBase
{
    Q_OBJECT
private:
    QString place;
public:
    PlaceInfo(QRemoteShell *terminal, QObject *parent=0);
    void getPlaceInfo();

    //
    QString& placeInfo() { return place; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const PlaceInfo* info);
    friend QDataStream& operator>>(QDataStream& in, PlaceInfo*& info);
    friend QDebug operator<<(QDebug dbg, const PlaceInfo &info);

private slots:
    void on_term_receiveText(QString);
};

#endif // PLACEINFO_H
