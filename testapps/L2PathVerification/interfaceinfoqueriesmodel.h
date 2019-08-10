#ifndef INTERFACEINFOQUERIESMODEL_H
#define INTERFACEINFOQUERIESMODEL_H

#include <QObject>
#include <QAbstractTableModel>

class Queries;

class InterfaceInfoQueriesModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    QList<Queries*> m_lstQueries;
    QList<Queries*> m_lstQueriesAnterior;
    QStringList m_lstIndexesConErrores;

public:
    explicit InterfaceInfoQueriesModel(QObject *parent = 0);
    ~InterfaceInfoQueriesModel();
    void setQueries(QList<Queries*> lstQueries) { m_lstQueries = lstQueries; }

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role)const;
    QVariant headerData(int section, Qt::Orientation orientation,
                            int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void updateModel();
};

#endif // INTERFACEINFOQUERIESMODEL_H
