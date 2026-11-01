#ifndef FUNCIONINFO_H
#define FUNCIONINFO_H

#include <QObject>
#include "funcionbase.h"

struct SLabelTxtRcvd
{
    QString label;
    QString txt;
};

QDataStream& operator<<(QDataStream& out, const SLabelTxtRcvd &data);
QDataStream& operator>>(QDataStream& in, SLabelTxtRcvd& data);

class QNETWORKCLIQUERIES_EXPORT FuncionInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QStringList _lstTxt;
    QList<QueriesConfigurationValue> _lstFuncionesLabeled;
    QStringList _lstFunciones;
    QList<SLabelTxtRcvd> _lstLabeledTxt;
    QString _label;
    void _siguienteFuncion();

public:
    FuncionInfo() {}
    FuncionInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    FuncionInfo(const FuncionInfo &other);
    ~FuncionInfo();
    void getTXT();

    //
    QString txtReceived() { return _lstTxt.join("\n"); }
    QStringList lstTxtReceived() { return _lstTxt; }
    QList<SLabelTxtRcvd> lstLabeledTxtReceived() { return _lstLabeledTxt; }
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
