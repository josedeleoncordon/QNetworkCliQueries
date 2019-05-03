#include "snmpget.h"
//#include "Get.h"

#include <QDebug>

//Delete this

SnmpGet::SnmpGet(QString IP, QStringList community, QString objectId, QObject *parent) :
    QObject(parent),
    IP(IP),
    community(community),
    objectId(objectId)
{
    get=NULL;
    pos=-1;       
}

SnmpGet::SnmpGet(const SnmpGet &other) :
    QObject(other.parent())
{
    get=NULL;
    pos=-1;
    IP = other.IP;
    community = other.community;
    objectId = other.objectId;
    m_response = other.m_response;
}

SnmpGet::~SnmpGet()
{
    if (get)
        delete get;
}

void SnmpGet::execute()
{
    siguienteComm();
}

void SnmpGet::siguienteComm()
{
    if ( pos < community.size()-1 )
    {
        pos++;

        if ( get )
            delete get;

        QString com = community.at(pos);

//        get = new Get( IP,objectId,com,this );
//        connect(get,SIGNAL(responseReceived()),SLOT(on_get_responseReceived()));
//        connect(get,SIGNAL(noResponseReceived()),SLOT(on_get_noResponseReceived()));
//        get->execute();
    }
    else
        emit finished();
}

void SnmpGet::on_get_noResponseReceived()
{
    siguienteComm();
}

void SnmpGet::on_get_responseReceived()
{
//    m_response = get->getResponse();
//    emit finished();
}
