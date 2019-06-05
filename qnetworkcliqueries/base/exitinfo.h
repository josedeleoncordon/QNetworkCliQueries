#ifndef EXITINFO_H
#define EXITINFO_H

#include "funcionbase.h"

class QNETWORKCLIQUERIES_EXPORT ExitInfo : public FuncionBase
{
    Q_OBJECT
protected:

public:
    ExitInfo(QRemoteShell *terminal, QObject *parent=0);
    void exit();

    //

    //
    QString macFromIP(QString ip);

    //
private slots:
    void on_term_receiveText();
    void on_term_disconnected();
};

#endif // EXITINFO_H
