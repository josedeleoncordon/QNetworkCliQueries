#ifndef EQUIPMENTNEIGHBORSINFO_H
#define EQUIPMENTNEIGHBORSINFO_H

#include "funcionbase.h"

struct SEquipmentNeighborsInfo : public InfoBase
{
    QString nombreequipo;
    QString ip;
    QString interfazestesalida;
    QString interfazotroentrada;
    QString plataforma;
    QList<SEquipmentNeighborsInfo> lstEquipos;
};

QDataStream& operator<<(QDataStream& out, const SEquipmentNeighborsInfo &data);
QDataStream& operator>>(QDataStream& in, SEquipmentNeighborsInfo& data);

void updateInfoList(QList<SEquipmentNeighborsInfo> &lstDest, QList<SEquipmentNeighborsInfo> &lstOrigin );

class QNETWORKCLIQUERIES_EXPORT EquipmentNeighborsInfo : public FuncionBase
{
    Q_OBJECT
protected:
    QList<SEquipmentNeighborsInfo> m_lstEquipos;

public:
    //usada para sacar las interfaces del comando show lldp neigh y usarlas si lldp neigh detail no las muestra
    QMap<QString,QString> mapInterfazLocales;

    EquipmentNeighborsInfo() {}
    EquipmentNeighborsInfo(QRemoteShell *terminal, int option=QueryOpcion::Null);
    EquipmentNeighborsInfo(const EquipmentNeighborsInfo &other);
    ~EquipmentNeighborsInfo();
    void addEquipmentNeighborsInfo(SEquipmentNeighborsInfo i);
    virtual void getEquipmentNeighborsInfo();

    //
    QList<SEquipmentNeighborsInfo>& equipmentNeighborsInfo() { return m_lstEquipos; }

    //
    QList<SEquipmentNeighborsInfo*> equipmentNeighborsInfoFromInterface(QString interfaz);

    //
    friend QDataStream& operator<<(QDataStream& out, const EquipmentNeighborsInfo& data);
    friend QDataStream& operator>>(QDataStream& in, EquipmentNeighborsInfo& data);
    friend QDataStream& operator<<(QDataStream& out, const EquipmentNeighborsInfo* data);
    friend QDataStream& operator>>(QDataStream& in, EquipmentNeighborsInfo*& data);

    friend QDebug operator<<(QDebug dbg, const EquipmentNeighborsInfo &data);

private slots:
    void on_term_receiveText_CDP();
    void on_term_receiveText_LLDP_Cisco();
    void on_term_receiveText_LLDP_Cisco_detail();
    void on_term_receiveText_LLDP_Huawei();
};

Q_DECLARE_METATYPE(SEquipmentNeighborsInfo)

#endif // EQUIPMENTNEIGHBORSINFO_H
