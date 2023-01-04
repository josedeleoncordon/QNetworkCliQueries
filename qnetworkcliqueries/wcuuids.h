#ifndef WCUUIDS_H
#define WCUUIDS_H

#include <QMap>

class WCUUIDs
{
    friend class DunetServer;
public:
    static WCUUIDs *Instance();
    bool isAlive(QString);

private:
    explicit WCUUIDs();
    static WCUUIDs *m_instance;

    QStringList _lstUUIDs;
    void addUUID(QString);
    void removeUUID(QString);    
};

#endif // WCUUIDS_H
