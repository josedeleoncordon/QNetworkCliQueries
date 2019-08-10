#ifndef PIMINFO_H
#define PIMINFO_H

#include "funcionbase.h"

struct SPIMInfo : InfoBase
{
    QString interfaz;
    QString nbrCount;
};

QDataStream& operator<<(QDataStream& out, const SPIMInfo* data);
QDataStream& operator>>(QDataStream& in, SPIMInfo*& data);

void updateInfoList(QList<SPIMInfo*> &lstDest, QList<SPIMInfo*> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT PIMInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SPIMInfo*> m_lstPIMNeighbors;
    QList<SPIMInfo*> m_lstPimInterfaces;
    QStringList m_lstRouterPim;
    QStringList m_lstMulticastRouting;

public:
    PIMInfo(QRemoteShell *terminal, QObject *parent=0);
    PIMInfo(const PIMInfo &other);
    ~PIMInfo();
//    void getPIMNeighbors();
    virtual void getPIMInterfaces();

    //
    QList<SPIMInfo*>& pimIntefacesInfo() { return m_lstPIMNeighbors; }

    //
    bool interfaceStatus(QString interfaz, QString& txtStatusOut );

    //
    friend QDataStream& operator<<(QDataStream& out, const PIMInfo* info);
    friend QDataStream& operator>>(QDataStream& in, PIMInfo*& info);
    friend QDebug operator<<(QDebug dbg, const PIMInfo &info);

private slots:

//    void on_term_receiveTextNeighbors(QString);
    void on_term_receiveTextInterfaces();
    void on_term_receiveTextRouterPim();
    void on_term_receiveTextMulticastRouting();
};

#endif // PIMINFO_H
