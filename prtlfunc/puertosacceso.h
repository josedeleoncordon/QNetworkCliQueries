#ifndef PUERTOSACCESO_H
#define PUERTOSACCESO_H

#include "queries.h"
#include "arpinfo.h"

struct MacBuscando : SIpInfo
{
    short int lastLearnedMac; //equipo 1 cisco
    short int lastLearnedMac2; //equipo 2 atn etx etc
    short int lastLearnedMacUltimoEsfuerzo;
    QDateTime lastDateTimeUltimoEsfuerzo;
    bool usandoHistorico;
    bool usandoHistoricoUltimoEsfuerzo;
    bool lastEquipmentNeighborsInfo;
    bool lastVlansGestion;
    bool lastAsr900;
    bool lastOperativo;
    bool lastOperativoUltimoEsfuerzo;
    bool ultimoEsfuerzo;

    QString PE;
    QString PEip;

    QString equipo;
    QString equipoip;
    QString equipoplatform;
    //interfaz de equipo 1 es la de la estructura de MAC
    QString interfaceDescription;

    QString equipoUltimoEsfuerzo;
    QString equipoipUltimoEsfuerzo;
    QString equipoplatformUltimoEsfuerzo;
    QString interfaceUltimoEsfuerzo;
    QString interfaceDescripcionUltimoEsfuerzo;        

    QString equipo2;
    QString equipo2ip;
    QString equipo2platform;
    QString interfaz2;
    QString interface2Description;
    QDateTime datetime2;

    QStringList path; //para la ruta completa

    QStringList debug;

    MacBuscando()
    {
        lastLearnedMac=0;
        lastLearnedMac2=0;        
        lastLearnedMacUltimoEsfuerzo=0;
        lastEquipmentNeighborsInfo=false;
        lastVlansGestion=false;
        lastAsr900=false;
        ultimoEsfuerzo=false;
        usandoHistorico=false;
        usandoHistoricoUltimoEsfuerzo=false;
        lastOperativo=false;
        lastOperativoUltimoEsfuerzo=false;
    }
};

QList<MacBuscando*> buscarPuertosAcceso(QList<Queries*> lstARP,
                                        QList<Queries*> lstMACs,
                                        QString pais, QString debugMacIP,
                                        bool segundoEquipo=false);

QList<MacBuscando*> buscarPuertosAccesoRutaCompleta(QList<Queries*> lstTopologia,
                                                    QList<Queries*> lstARP,
                                                    QString pais,
                                                    QString IP,
                                                    QString VRF);

bool puertosAccesoVerificarEquipoInterfazMac( QList<MacBuscando*> lst,
                                              QString equipo,
                                              QString interfaz,
                                              QString mac);

#endif // PUERTOSACCESO_H
