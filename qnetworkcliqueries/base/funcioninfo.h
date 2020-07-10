#ifndef FUNCIONINFO_H
#define FUNCIONINFO_H

#include <QObject>
#include "funcionbase.h"

class QNETWORKCLIQUERIES_EXPORT FuncionInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QStringList _lstTxt;
    QStringList _lstFunciones;

    void _siguienteFuncion();

public:
    FuncionInfo() {}
    FuncionInfo(QRemoteShell *terminal, QueryOpcion option=QueryOpcion::Null);
    FuncionInfo(const FuncionInfo &other);
    ~FuncionInfo();
    void getTXT();

    //
    QString txtReceived() { return _lstTxt.join("\n"); }
    QStringList &lstTxtReceived() { return _lstTxt; }

    //

    //
    friend QDataStream& operator<<(QDataStream& out, const FuncionInfo& info);
    friend QDataStream& operator>>(QDataStream& in, FuncionInfo& info);
    friend QDataStream& operator<<(QDataStream& out, const FuncionInfo* info);
    friend QDataStream& operator>>(QDataStream& in, FuncionInfo*& info);
    friend QDebug operator<<(QDebug dbg, const FuncionInfo &info);

private slots:

    void on_term_receiveText();
};

#endif // FUNCIONINFO_H
