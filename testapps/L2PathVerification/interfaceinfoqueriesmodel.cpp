#include "interfaceinfoqueriesmodel.h"
#include "queries.h"

#include <QBrush>

InterfaceInfoQueriesModel::InterfaceInfoQueriesModel(QObject *parent) :
    QAbstractTableModel(parent)
{}

InterfaceInfoQueriesModel::~InterfaceInfoQueriesModel() {}

int InterfaceInfoQueriesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    int c=0;
    foreach (Queries *qry, m_lstQueries)
        c+=qry->interfacesInfo().size();
    return c;
}

int InterfaceInfoQueriesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 18;
}

QVariant InterfaceInfoQueriesModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        switch (role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole :
        {
            int c=-1;
            foreach (Queries *qry, m_lstQueries)
            {
                foreach (SInterfaceInfo *ii, qry->interfacesInfo())
                {
                    c++;
                    if ( index.row() == c )
                    {
                        switch (index.column())
                        {
                        case 0: return qry->hostName();
                        case 1: return ii->interfaz;
                        case 2: return ii->bandwidth;
                        case 3: return ii->duplex;
                        case 4: return ii->speed;
                        case 5: return ii->ratein;
                        case 6: return ii->rateout;
                        case 7: return ii->dropsin;
                        case 8: return ii->dropsout;
                        case 9: return ii->errorsin;
                        case 10: return ii->errorsout;
                        case 11: return ii->CRC;
                        case 12: return ii->overrun;
                        case 13: return ii->collisions;
                        case 14: return ii->mediaType;
                        case 15: return ii->mtu;
                        case 16: return ii->opticalRx;
                        case 17: return ii->description;
                        }
                    }
                }
            }
        }
        case Qt::BackgroundRole:
            if ( m_lstIndexesConErrores.contains( QString::number(index.row())+","+QString::number(index.column()) ) )
                return QVariant(QBrush(QColor(255,192,193)));
            else
                return QVariant();
            break;
        }
    }
    return QVariant();
}

QVariant InterfaceInfoQueriesModel::headerData(int section, Qt::Orientation orientation,int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if ( orientation == Qt::Vertical )
        return section+1;
    if ( section >= 18 )
        return QVariant();

    switch (section)
    {
    case 0: return "Equipo";
    case 1: return "Interfaz";
    case 2: return "BW";
    case 3: return "Duplex";
    case 4: return "Speed";
    case 5: return "RateIn";
    case 6: return "RateOut";
    case 7: return "DropsIn";
    case 8: return "DropsOut";
    case 9: return "ErrorsIn";
    case 10: return "ErrorsOut";
    case 11: return "CRCs";
    case 12: return "Overruns";
    case 13: return "Collisions";
    case 14: return "MediaType";
    case 15: return "MTU";
    case 16: return "Optical Rx";
    case 17: return "Descripcion";
    }
}

Qt::ItemFlags InterfaceInfoQueriesModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsSelectable;

    return flags;
}

void InterfaceInfoQueriesModel::updateModel()
{
    beginResetModel();

    //se verifica la nueva consulta con la anterior y se observa si cambiaron datos de errores, drops, etc
    short int row = -1;
    if ( m_lstQueriesAnterior.size() == m_lstQueries.size() )
    {
        for (int c=0; c<m_lstQueries.size();c++)
        {
            Queries *qrya = m_lstQueriesAnterior.at(c);
            Queries *qryn = m_lstQueries.at(c);

            for ( int i=0; i<qryn->interfacesInfo().size(); i++ )
            {
                row++;
                SInterfaceInfo *siia = qrya->interfacesInfo().at(i);
                SInterfaceInfo *siin = qryn->interfacesInfo().at(i);

                QString txt;

                if ( siia->dropsin != siin->dropsin )
                {
                    txt = QString::number(row)+","+QString::number(7);
                    if ( !m_lstIndexesConErrores.contains( txt ) )
                        m_lstIndexesConErrores.append( txt );
                }
                if ( siia->dropsout != siin->dropsout )
                {
                    txt = QString::number(row)+","+QString::number(8);
                    if ( !m_lstIndexesConErrores.contains( txt ) )
                        m_lstIndexesConErrores.append( txt );
                }
                if ( siia->errorsin != siin->errorsin )
                {
                    txt = QString::number(row)+","+QString::number(9);
                    if ( !m_lstIndexesConErrores.contains( txt ) )
                        m_lstIndexesConErrores.append( txt );
                }
                if ( siia->errorsout != siin->errorsout )
                {
                    txt = QString::number(row)+","+QString::number(10);
                    if ( !m_lstIndexesConErrores.contains( txt ) )
                        m_lstIndexesConErrores.append( txt );
                }
                if ( siia->CRC != siin->CRC )
                {
                    txt = QString::number(row)+","+QString::number(11);
                    if ( !m_lstIndexesConErrores.contains( txt ) )
                        m_lstIndexesConErrores.append( txt );
                }
                if ( siia->overrun != siin->overrun )
                {
                    txt = QString::number(row)+","+QString::number(12);
                    if ( !m_lstIndexesConErrores.contains( txt ) )
                        m_lstIndexesConErrores.append( txt );
                }
                if ( siia->collisions != siin->collisions )
                {
                    txt = QString::number(row)+","+QString::number(13);
                    if ( !m_lstIndexesConErrores.contains( txt ) )
                        m_lstIndexesConErrores.append( txt );
                }
            }
        }
    }

    qDeleteAll(m_lstQueriesAnterior);
    m_lstQueriesAnterior.clear();

    //se guarda la consulta nueva como anterior para poder comparar en la siguiente consulta
    foreach (Queries *qry, m_lstQueries)
    {
        Queries *qry2 = new Queries(*qry);
        m_lstQueriesAnterior.append(qry2);
    }

    endResetModel();
}
