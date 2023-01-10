#ifndef WCUUIDS_H
#define WCUUIDS_H

#include <QObject>
#include <QMap>

class WCUUIDs : public QObject
{
    Q_OBJECT

public:
    static WCUUIDs *Instance();    
    void addUUID(QString);
    void removeUUID(QString);
    void cancelUUID(QString);
    void setStillAlive(QStringList);

    bool isAlive(QString);

signals:
    void uuidCanceled(QString);

private:
    explicit WCUUIDs();
    static WCUUIDs *m_instance;

    QStringList _lstUUIDs;    
};

#endif // WCUUIDS_H
