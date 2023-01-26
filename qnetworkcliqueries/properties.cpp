#include "properties.h"
#include "funciones.h"

Properties * Properties::m_instance = nullptr;

Properties * Properties::Instance()
{
    if (!m_instance)
    {
        m_instance = new Properties();
        m_instance->loadSettings();
    }
    return m_instance;
}

Properties::Properties()
{
    savelogs=false;

    //https://www.digitalocean.com/community/tutorials/how-to-create-an-ssl-certificate-on-apache-for-centos-7
    //openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /etc/ssl/private/ufinet.key -out /etc/ssl/certs/ufinet.crt

    _skFile.setFileName(QStringLiteral(":/unet.key"));
    _ivFile.setFileName(QStringLiteral(":/unet.cert"));
    _skFile.open(QIODevice::ReadOnly);
    _ivFile.open(QIODevice::ReadOnly);
}

Properties::~Properties()
{
    saveSettings();
    _skFile.close();
    _ivFile.close();
    delete m_instance;
    m_instance = 0;
}

void Properties::loadSettings()
{
    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

    settings.beginGroup("Connection");
    user = decode(settings.value("ConnectionUser").toString());
    password = decode(settings.value("ConnectionPassword").toString());
    settings.endGroup();

    settings.beginGroup("BDConnection");
    bduser = decode(settings.value("BDConnectionUser").toString());
    bdpassword = decode(settings.value("BDConnectionPassword").toString());
    bd = decode(settings.value("BD").toString());
    bdhost = decode(settings.value("BDHost").toString());
    settings.endGroup();

    settings.beginGroup("Queries");
    queriespath = decode(settings.value("QueriesPath").toString());
    logspath = decode(settings.value("LogsPath").toString());
    linuxpromt = decode(settings.value("LinuxPropmt").toString());
#ifdef QT_DEBUG
    savelogs = settings.value("savelogs").toBool();
#endif
    settings.endGroup();

    settings.beginGroup("Topology");
    mnemonicspath = decode(settings.value("MnemonicsPath").toString());
    topologiaconfpath = decode(settings.value("topologiaconfpath").toString());
    topologiaiconpath = decode(settings.value("topologiaiconpath").toString());
    settings.endGroup();

    settings.beginGroup("GroupRoots");
    mapGrupoRaizIP = settings.value("mapGrupoRaizIP").toMap();
    settings.endGroup();
}

void Properties::saveSettings()
{
    QSettings settings;
    settings.setDefaultFormat(QSettings::NativeFormat);

    settings.beginGroup("Connection");
    settings.setValue("ConnectionUser",encode(user));
    settings.setValue("ConnectionPassword",encode(password));
    settings.endGroup();

    settings.beginGroup("BDConnection");
    settings.setValue("BDConnectionUser",encode(bduser));
    settings.setValue("BDConnectionPassword",encode(bdpassword));
    settings.setValue("BD",encode(bd));
    settings.setValue("BDHost",encode(bdhost));
    settings.endGroup();

    settings.beginGroup("Queries");
    settings.setValue("QueriesPath",encode(queriespath));
    settings.setValue("LogsPath",encode(logspath));
    settings.setValue("LinuxPropmt",encode(linuxpromt));
    settings.setValue("savelogs",savelogs);
    settings.endGroup();

    settings.beginGroup("Topology");
    settings.setValue("MnemonicsPath",encode(mnemonicspath));
    settings.setValue("topologiaconfpath",encode(topologiaconfpath));
    settings.setValue("topologiaiconpath",encode(topologiaiconpath));
    settings.endGroup();

    settings.beginGroup("GroupRoots");
    settings.setValue("mapGrupoRaizIP",mapGrupoRaizIP);
    settings.endGroup();
}

QFile &Properties::skFile()
{
    _skFile.reset();
    return _skFile;
}

QFile &Properties::ivFile()
{
    _ivFile.reset();
    return _ivFile;
}

