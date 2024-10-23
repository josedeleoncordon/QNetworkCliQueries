#include "queriesthreadcontroller.h"
#include "qnetworkquerieslogging.h"
#include <QMutexLocker>
#include <QDebug>

QueriesThreadController * QueriesThreadController::m_instance = nullptr;

QueriesThreadController * QueriesThreadController::Instance()
{
    if (!m_instance)
        m_instance = new QueriesThreadController();

    return m_instance;
}

QueriesThreadController::QueriesThreadController()
{
    m_maxparalelos=40;
    m_maxparalelosfijos=25;
    m_maxparalelosmismoequipo=4;
    m_maxparalelosuuiddescrubimiento=0;
}

QueriesThreadController::~QueriesThreadController()
{
    delete m_instance;
    m_instance = 0;
}

void QueriesThreadController::registerQueriesThreadUUID(QString uuid)
{
    if ( !m_mapUUIDParalelosDescubrimiento.contains(uuid) )
    {
        // qCDebug(queriesthreadController) << Q_FUNC_INFO << "Registrando UUID" << uuid;
        m_mapUUIDParalelosDescubrimiento.insert(uuid,0);
        m_calcularMaxParalelosUUID();
        serverstatus();
    }
}

void QueriesThreadController::registerQueriesThreadUUID(QString uuid, int cantidadEquipos)
{    
    if ( !m_mapUUIDParalelosFijo.contains(uuid) )
    {
        // qCDebug(queriesthreadController) << Q_FUNC_INFO << "Registrando UUID" << uuid << "cantidad" << cantidadEquipos;
        m_mapUUIDParalelosFijo.insert(uuid,0);
        m_mapUUIDFijoCantidadRegistrada.insert(uuid,cantidadEquipos);
        m_calcularMaxParalelosUUID();
        serverstatus();
    }
}

void QueriesThreadController::unregisterQueriesThreadUUID(QString uuid)
{
    if ( m_mapUUIDParalelosFijo.contains(uuid) )
    {
        // qCDebug(queriesthreadController) << Q_FUNC_INFO << "Eliminado registro UUID" << uuid;
        m_mapUUIDParalelosFijo.remove(uuid);
        m_mapUUIDFijoCantidadRegistrada.remove(uuid);
        m_mapUUIDFijoCantidadPermitida.remove(uuid);
    }
    else if ( m_mapUUIDParalelosDescubrimiento.contains(uuid) )
        m_mapUUIDParalelosDescubrimiento.remove(uuid);

    m_calcularMaxParalelosUUID();
    serverstatus();
}

void QueriesThreadController::m_calcularMaxParalelosUUID()
{
    if ( m_mapUUIDFijoCantidadRegistrada.isEmpty() && m_mapUUIDParalelosDescubrimiento.isEmpty() )
        return;

    short cantidadtotalfijos =0 ;
    QMapIterator<QString, short> imap(m_mapUUIDFijoCantidadRegistrada);
    while (imap.hasNext())
    {
        imap.next();
        cantidadtotalfijos += imap.value();
    }

    // qCDebug(queriesthreadController) << Q_FUNC_INFO << "cantidadtotalfijos" << cantidadtotalfijos;

    //calculando fijos
    if ( cantidadtotalfijos )
    {
        float percent;
        if ( cantidadtotalfijos <= m_maxparalelosfijos )
            percent=1;
        else
            percent=((m_maxparalelosfijos*100)/(float)cantidadtotalfijos)/100.0;

        // qCDebug(queriesthreadController) << Q_FUNC_INFO << "Calculando fijos percent" << percent;

        m_mapUUIDFijoCantidadPermitida.clear();
        imap.toFront();
        while (imap.hasNext())
        {
            imap.next();
            short cantidad = imap.value()*percent;
            if ( !cantidad ) cantidad = 1;
            m_mapUUIDFijoCantidadPermitida.insert(imap.key(),cantidad);
        }

        // qCDebug(queriesthreadController) << Q_FUNC_INFO << "m_mapUUIDFijoCantidadPermitida" << m_mapUUIDFijoCantidadPermitida;
    }

    //calculamos descubrimiento
    if ( !m_mapUUIDParalelosDescubrimiento.isEmpty() )
    {
        if ( cantidadtotalfijos > m_maxparalelosfijos )
            cantidadtotalfijos = m_maxparalelosfijos;

        // qCDebug(queriesthreadController) << Q_FUNC_INFO <<
        //                                     "Calculando descubrimiento cantidadtotalfijos referencia" <<
        //                                     cantidadtotalfijos;

        short permitidosdescubrimiento = m_maxparalelos - cantidadtotalfijos;
        short div = permitidosdescubrimiento / m_mapUUIDParalelosDescubrimiento.size();
        if ( div > 0 )
            m_maxparalelosuuiddescrubimiento = div;
        else
            m_maxparalelosuuiddescrubimiento = 1;

        // qCDebug(queriesthreadController) << Q_FUNC_INFO <<
        //                                     "Calculando descubrimiento m_maxparalelosuuiddescrubimiento" <<
        //                                     m_maxparalelosuuiddescrubimiento;
    }
}

void QueriesThreadController::emitStatus()
{
    short int mapsize = m_mapUUIDParalelosFijo.size() + m_mapUUIDParalelosDescubrimiento.size();
    emit status("Servidor consultando equipos de "+QString::number(mapsize)+" solicitud"+(mapsize==1?"":"es"));
}

void QueriesThreadController::serverstatus()
{
    emitStatus();
}

bool QueriesThreadController::consultarAgregarEquipo(QString uuid, QString ip)
{
    QMutexLocker locker(&m_mutex);

    emitStatus();

    if ( m_equiposenconsultaGeneral.size() >= m_maxparalelos )
    {
        // qDebug() << Q_FUNC_INFO << ip << "m_maxparalelos alcanzado, no es posible iniciar nueva consulta" << m_equiposenconsultaGeneral;
        return false;
    }
    else if ( m_mapUUIDParalelosFijo.contains(uuid) )
    {
        if ( m_mapUUIDParalelosFijo.value(uuid) >= m_mapUUIDFijoCantidadPermitida.value(uuid) )
        {
            // qDebug() << Q_FUNC_INFO << ip << "m_maxparalelosuuidfijo por UUID alcanzado, no es posible iniciar nueva consultar";
            return false;
        }
    }
    else if ( m_mapUUIDParalelosDescubrimiento.contains(uuid) )
    {
        if ( m_mapUUIDParalelosDescubrimiento.value(uuid) >= m_maxparalelosuuiddescrubimiento )
        {
            // qDebug() << Q_FUNC_INFO << ip << "m_maxparalelosuuiddescrubimiento por UUID alcanzado, no es posible iniciar nueva consultar";
            return false;
        }
    }

    int c=0;
    for ( QString ipequipo : m_equiposenconsultaGeneral )
        if ( ip == ipequipo )
            c++;

    if ( c < m_maxparalelosmismoequipo )
    {
        m_equiposenconsultaGeneral.append(ip);

        if ( m_mapUUIDParalelosFijo.contains(uuid) )
            m_mapUUIDParalelosFijo[uuid]++;
        else if ( m_mapUUIDParalelosDescubrimiento.contains(uuid) )
            m_mapUUIDParalelosDescubrimiento[uuid]++;

        // qDebug() << Q_FUNC_INFO << ip << "m_maxparalelosmismoequipo" << m_maxparalelosmismoequipo <<
        //             "c" << c <<
        //             "Se permite nueva consulta" <<
        //             "m_equiposenconsultaGeneral.size()" << m_equiposenconsultaGeneral.size();
        return true;
    }
    else
    {
        // qDebug() << Q_FUNC_INFO << ip << "m_maxparalelosmismoequipo" << m_maxparalelosmismoequipo <<
        //             "c" << c <<
        //             "NO Se permite nueva consulta" <<
        //             "m_equiposenconsultaGeneral.size()" << m_equiposenconsultaGeneral.size();
        return false;
    }
}

void QueriesThreadController::consultaFinalizada(QString uuid, QString ip)
{    
    m_equiposenconsultaGeneral.removeOne(ip);

    if ( m_mapUUIDParalelosFijo.contains(uuid) )
        m_mapUUIDParalelosFijo[uuid]--;
    else if ( m_mapUUIDParalelosDescubrimiento.contains(uuid) )
        m_mapUUIDParalelosDescubrimiento[uuid]--;

    // qDebug() << Q_FUNC_INFO << ip << "m_equiposenconsultaGeneral.size" << m_equiposenconsultaGeneral.size() <<
    //             "m_mapUUIDParalelosFijo size" << m_mapUUIDParalelosFijo.size() <<
    //             "m_mapUUIDParalelosDescubrimiento" << m_mapUUIDParalelosDescubrimiento.size();
}
