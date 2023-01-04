#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include "funcionbase.h"

class Queries;

class QNETWORKCLIQUERIES_EXPORT Config : public FuncionBase
{
    Q_OBJECT
    bool m_error;    
    bool m_guardado;
    QStringList m_lstComandos;
    int m_lstComandosPos;
    QStringList m_lstComandosConErrores;

    void siguienteComando();

public:
    Config() {}
    Config(QRemoteShell *terminal, int option=QueryOpcion::Null);
    ~Config();
    void configApply();

    //
    bool error() { return m_error; }
    QStringList lstComandosConErrores() { return m_lstComandosConErrores; }

    //

    //
    friend QDebug operator<<(QDebug dbg, const Config &info);

private slots:
    void on_term_receiveText_configMode();
    void on_term_receiveText_finished();
    void on_term_receiveText_exited();
};

#endif // CONFIG_H
