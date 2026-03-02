#include "properties.h"
#include "funciones.h"
#include <QTextStream>

QString PropertiesEncoderDecoder::encode(QString txt)
{
    return txt;
}

QString PropertiesEncoderDecoder::decode(QString txt)
{
    return txt;
}

PropertiesEncoderDecoder *newPropertiesEncoderDecoder()
{
    return new PropertiesEncoderDecoder;
}

Properties * Properties::m_instance = nullptr;

Properties * Properties::Instance()
{
    if (!m_instance)    
        m_instance = new Properties();

    return m_instance;
}

Properties::Properties()
{
    savelogs=false;   
    ed = &newPropertiesEncoderDecoder;
}

Properties::~Properties()
{
    saveSettings();
    delete m_instance;
    m_instance = 0;
}

void Properties::setNewEnconderDecoder(PropertiesEncoderDecoder*(*encoderdecoder)(void))
{
    ed = encoderdecoder;
}

void Properties::loadSettings()
{
    PropertiesEncoderDecoder *ped = (*ed)();

    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

    SBDConnectionParameters sbdcp;

    settings.beginGroup("BDConnection_msql");
    odbclib = ped->decode(settings.value("ODBCLib").toString());
    settings.endGroup();

    settings.beginGroup("BDConnection_msql_regengadmin");
    sbdcp.name = ped->decode(settings.value("Name").toString());
    sbdcp.BDConnectionUser = ped->decode(settings.value("BDConnectionUser").toString());
    sbdcp.BDConnectionPassword = ped->decode(settings.value("BDConnectionPassword").toString());
    sbdcp.BD = ped->decode(settings.value("BD").toString());
    sbdcp.BDHost = ped->decode(settings.value("BDHost").toString());
    sbdcp.Driver = ped->decode(settings.value("Driver").toString());
    mapBDConnectionParameters.insert(sbdcp.name,sbdcp);
    settings.endGroup();

    settings.beginGroup("BDConnection_mysql_unet");
    sbdcp.name = ped->decode(settings.value("Name").toString());
    sbdcp.BDConnectionUser = ped->decode(settings.value("BDConnectionUser").toString());
    sbdcp.BDConnectionPassword = ped->decode(settings.value("BDConnectionPassword").toString());
    sbdcp.BD = ped->decode(settings.value("BD").toString());
    sbdcp.BDHost = ped->decode(settings.value("BDHost").toString());
    sbdcp.Driver = ped->decode(settings.value("Driver").toString());
    mapBDConnectionParameters.insert(sbdcp.name,sbdcp);
    settings.endGroup();

    settings.beginGroup("BDConnection_msql_UfinetProvisioning");
    sbdcp.name = ped->decode(settings.value("Name").toString());
    sbdcp.BDConnectionUser = ped->decode(settings.value("BDConnectionUser").toString());
    sbdcp.BDConnectionPassword = ped->decode(settings.value("BDConnectionPassword").toString());
    sbdcp.BD = ped->decode(settings.value("BD").toString());
    sbdcp.BDHost = ped->decode(settings.value("BDHost").toString());
    sbdcp.Driver = ped->decode(settings.value("Driver").toString());
    mapBDConnectionParameters.insert(sbdcp.name,sbdcp);
    settings.endGroup();

    settings.beginGroup("BDConnection_mysql_Planning");
    sbdcp.name = ped->decode(settings.value("Name").toString());
    sbdcp.BDConnectionUser = ped->decode(settings.value("BDConnectionUser").toString());
    sbdcp.BDConnectionPassword = ped->decode(settings.value("BDConnectionPassword").toString());
    sbdcp.BD = ped->decode(settings.value("BD").toString());
    sbdcp.BDHost = ped->decode(settings.value("BDHost").toString());
    sbdcp.Driver = ped->decode(settings.value("Driver").toString());
    mapBDConnectionParameters.insert(sbdcp.name,sbdcp);
    settings.endGroup();

    settings.beginGroup("Connection");
    user = ped->decode(settings.value("ConnectionUser").toString());
    password = ped->decode(settings.value("ConnectionPassword").toString());
    settings.endGroup();

    settings.beginGroup("Queries");
    queriespath = ped->decode(settings.value("QueriesPath").toString());
    logspath = ped->decode(settings.value("LogsPath").toString());
    linuxpromt = ped->decode(settings.value("LinuxPropmt").toString());
#ifdef QT_DEBUG
    savelogs = settings.value("savelogs").toBool();
#endif
    settings.endGroup();

    settings.beginGroup("Topology");
    mnemonicspath = ped->decode(settings.value("MnemonicsPath").toString());
    topologiaconfpath = ped->decode(settings.value("topologiaconfpath").toString());
    topologiaiconpath = ped->decode(settings.value("topologiaiconpath").toString());
    settings.endGroup();

    settings.beginGroup("GroupRoots");
    mapGrupoRaizIP = settings.value("mapGrupoRaizIP").toMap();
    settings.endGroup();
}

void Properties::saveSettings()
{
    PropertiesEncoderDecoder *ped = (*ed)();

    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

    settings.beginGroup("Connection");
    settings.setValue("ConnectionUser",ped->encode(user));
    settings.setValue("ConnectionPassword",ped->encode(password));
    settings.endGroup();

    // settings.beginGroup("BDConnection");
    // settings.setValue("BDConnectionUser",ped->encode(bduser));
    // settings.setValue("BDConnectionPassword",ped->encode(bdpassword));
    // settings.setValue("BD",ped->encode(bd));
    // settings.setValue("BDHost",ped->encode(bdhost));
    // settings.endGroup();

    settings.beginGroup("Queries");
    settings.setValue("QueriesPath",ped->encode(queriespath));
    settings.setValue("LogsPath",ped->encode(logspath));
    settings.setValue("LinuxPropmt",ped->encode(linuxpromt));
    settings.setValue("savelogs",savelogs);
    settings.endGroup();

    settings.beginGroup("Topology");
    settings.setValue("MnemonicsPath",ped->encode(mnemonicspath));
    settings.setValue("topologiaconfpath",ped->encode(topologiaconfpath));
    settings.setValue("topologiaiconpath",ped->encode(topologiaiconpath));
    settings.endGroup();

    settings.beginGroup("GroupRoots");
    settings.setValue("mapGrupoRaizIP",mapGrupoRaizIP);
    settings.endGroup();
}

