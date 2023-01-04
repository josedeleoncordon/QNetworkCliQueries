#include "wcuuids.h"

#include <QDebug>

WCUUIDs *WCUUIDs::m_instance = nullptr;

WCUUIDs *WCUUIDs::Instance()
{
    if (!m_instance)
        m_instance = new WCUUIDs();
    return m_instance;
}

WCUUIDs::WCUUIDs()
{}

void WCUUIDs::addUUID(QString uuid)
{
//    qDebug() << Q_FUNC_INFO << uuid << _lstUUIDs;
    if ( !_lstUUIDs.contains(uuid) )
        _lstUUIDs.append(uuid);
}

void WCUUIDs::removeUUID(QString uuid)
{
//    qDebug() << Q_FUNC_INFO << uuid << _lstUUIDs;
    _lstUUIDs.removeOne(uuid);
}

bool WCUUIDs::isAlive(QString uuid)
{
//    qDebug() << Q_FUNC_INFO << uuid << _lstUUIDs;
    return _lstUUIDs.contains(uuid);
}
